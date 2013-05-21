#include <cmath>
#include <iostream>
#include "Uncertainty.hxx"


template <class T>
T add_quadriture(const T& lhs, const T& rhs)
{
	return sqrt(lhs*lhs + rhs*rhs);
}

UDouble_t::UDouble_t(double nominal)
{
	fNominal = nominal;
	fLow = fHigh = ::sqrt(nominal);
}

UDouble_t::UDouble_t(double nominal, double err_low, double err_high)
{
	fNominal = nominal;
	fLow = err_low;
	fHigh = err_high != 0 ? err_high : err_low;
}

UDouble_t UDouble_t::operator+ (const UDouble_t& rhs) const
{
	UDouble_t out (*this);
	out.fLow  = add_quadriture(fLow, rhs.fLow);
	out.fHigh = add_quadriture(fHigh, rhs.fHigh);
	out.fNominal += rhs.fNominal;
	
	return out;
}

UDouble_t UDouble_t::operator- (const UDouble_t& rhs) const
{
  UDouble_t out (*this);
	out.fLow  = add_quadriture(fLow, rhs.fLow);
	out.fHigh = add_quadriture(fHigh, rhs.fHigh);
	out.fNominal -= rhs.fNominal;

	return out;
}

UDouble_t UDouble_t::operator* (const UDouble_t& rhs) const
{
  UDouble_t out (*this);
	double lowRel  = add_quadriture(GetRelErrLow(),  rhs.GetRelErrLow());
	double highRel = add_quadriture(GetRelErrHigh(), rhs.GetRelErrHigh());

	out.SetNominal(fNominal * rhs.fNominal);
	out.SetRelErrLow(lowRel);
	out.SetRelErrHigh(highRel);

	return out;
}

UDouble_t UDouble_t::operator/ (const UDouble_t& rhs) const
{
  UDouble_t out (*this);
	double lowRel  = add_quadriture(GetRelErrLow(),  rhs.GetRelErrLow());
	double highRel = add_quadriture(GetRelErrHigh(), rhs.GetRelErrHigh());

	out.SetNominal(fNominal / rhs.fNominal);
	out.SetRelErrLow(lowRel);
	out.SetRelErrHigh(highRel);

	return out;
}

UDouble_t UDouble_t::operator+ (const double& rhs) const
{
	UDouble_t out (*this);
	out.fNominal += rhs;

	return out;
}

UDouble_t UDouble_t::operator- (const double& rhs) const
{
	UDouble_t out (*this);
	out.fNominal -= rhs;

	return out;
}

UDouble_t UDouble_t::operator* (const double& rhs) const
{
	UDouble_t out (*this);
	out.fNominal *= rhs;
	out.fLow     *= fabs(rhs);
	out.fHigh    *= fabs(rhs);

	return out;
}

UDouble_t UDouble_t::operator/ (const double& rhs) const
{
	UDouble_t out = *this * (1/rhs);
	return out;
}

void UDouble_t::Print() const
{
	std::cout << *this << "\n";
}


std::ostream& operator<< (std::ostream& strm, const UDouble_t& ud)
{
	if(ud.fHigh == 0 && ud.fLow == 0)
		strm << ud.fNominal;
	else if(ud.fHigh != ud.fLow)
		strm << ud.fNominal << " +" << ud.fHigh << " / -" << ud.fLow;
	else
		strm << ud.fNominal << " +/- " << ud.fHigh;
}

UDouble_t UDouble_t::pow(const UDouble_t& z, double x)
{
	UDouble_t out = z;

	out.fNominal = ::pow(z.fNominal, x);
	out.fLow  = fabs(out.fNominal * fabs(x) * z.fLow  / z.fNominal);
	out.fHigh = fabs(out.fNominal * fabs(x) * z.fHigh / z.fNominal);

	return out;
}

UDouble_t UDouble_t::pow(double x, const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = ::pow(x, z.fNominal);
	out.fLow  = fabs(out.fNominal * ::log(x) * z.fLow);
	out.fHigh = fabs(out.fNominal * ::log(x) * z.fHigh);

	return out;
}

UDouble_t UDouble_t::log(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fHigh  = z.fHigh  / z.fNominal;
	out.fHigh = z.fHigh / z.fNominal;
	out.fNominal = ::log(z.fNominal);
}

UDouble_t UDouble_t::exp(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = ::exp(z.fNominal);
	out.fHigh  = z.fHigh  / out.fNominal;
	out.fHigh = z.fHigh / out.fNominal;
}

UDouble_t UDouble_t::Mean(long n, const UDouble_t* u)
{
	const UDouble_t* const end = u+n;
	UDouble_t mean(0, 0);
	while(u < end)
		mean += *u++;
	mean /= (double)n;
	return mean;
}

UDouble_t UDouble_t::StdDev(long n, const UDouble_t* u)
{
	const UDouble_t* const end = u+n;
	UDouble_t tot(0,0), tot2(0,0), adouble;

	while(u < end) {
		adouble = *u++;
		tot  += adouble;
		tot2 += adouble*adouble;
	}

	double n1 = 1./n;
	UDouble_t mean = tot*n1;
	UDouble_t stddev = UDouble_t::sqrt(UDouble_t::abs(tot2*n1 - mean*mean));
	return stddev;
}

#ifdef USE_ROOT

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, const Double_t* x, const UDouble_t* y)
{
	Double_t* nom   = new Double_t[npoints];
	Double_t* yerrl = new Double_t[npoints];
	Double_t* yerrh = new Double_t[npoints];
	Double_t* xerrl = new Double_t[npoints];
	Double_t* xerrh = new Double_t[npoints];

	for(Int_t i=0; i< npoints; ++i) {
		nom[i]  = y[i].GetNominal();
		xerrl[i] = 0;
		xerrh[i] = 0;
		yerrl[i] = y[i].GetErrLow();
		yerrh[i] = y[i].GetErrHigh();
	}

	TGraphAsymmErrors* out = new TGraphAsymmErrors(npoints, x, nom, xerrl, xerrh, yerrl, yerrh);
	delete[] nom;
	delete[] xerrl;
	delete[] xerrh;
	delete[] yerrl;
	delete[] yerrh;

	return out;
}

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, const UDouble_t* x, const UDouble_t* y)
{
	Double_t* xnom  = new Double_t[npoints];
	Double_t* ynom  = new Double_t[npoints];
	Double_t* yerrl = new Double_t[npoints];
	Double_t* yerrh = new Double_t[npoints];
	Double_t* xerrl = new Double_t[npoints];
	Double_t* xerrh = new Double_t[npoints];

	for(Int_t i=0; i< npoints; ++i) {
		xnom[i]  = x[i].GetNominal();
		ynom[i]  = y[i].GetNominal();
		xerrl[i] = x[i].GetErrLow();
		xerrh[i] = x[i].GetErrHigh();
		yerrl[i] = y[i].GetErrLow();
		yerrh[i] = y[i].GetErrHigh();
	}

	TGraphAsymmErrors* out = new TGraphAsymmErrors(npoints, xnom, ynom, xerrl, xerrh, yerrl, yerrh);
	delete[] xnom;
	delete[] ynom;
	delete[] xerrl;
	delete[] xerrh;
	delete[] yerrl;
	delete[] yerrh;

	return out;
}

#endif
