#include <cmath>
#include <iostream>
#include "Uncertainty.hxx"


template <class T>
T add_quadriture(const T& lhs, const T& rhs)
{
	return sqrt(lhs*lhs + rhs*rhs);
}

UDouble_t::UDouble_t(double nominal, double err_low, double err_high, double sys_low, double sys_high)
{
	fNominal = nominal;
	fErr[0] = err_low;
	fErr[1] = err_high;
	fSys[0] = sys_low;
	fSys[1] = sys_high;
}

UDouble_t::UDouble_t(double nominal)
{
	fNominal = nominal;
	fErr[0] = fErr[1] = ::sqrt(nominal);
	fSys[0] = fSys[1] = 0.;
}

UDouble_t::UDouble_t(double nominal, double err_low, double err_high)
{
	fNominal = nominal;
	fErr[0] = err_low;
	fErr[1] = err_high != 0 ? err_high : err_low;
	fSys[0] = fSys[1] = 0.;
}

UDouble_t UDouble_t::operator+ (const UDouble_t& rhs) const
{
	UDouble_t out (*this);

	out.fErr[0]  = add_quadriture(fErr[0], rhs.fErr[0]);
	out.fErr[1]  = add_quadriture(fErr[1], rhs.fErr[1]);

	out.fSys[0]  = add_quadriture(fSys[0], rhs.fSys[0]);
	out.fSys[1]  = add_quadriture(fSys[1], rhs.fSys[1]);

	out.fNominal += rhs.fNominal;
	
	return out;
}

UDouble_t UDouble_t::operator- (const UDouble_t& rhs) const
{
  UDouble_t out (*this);

	out.fErr[0]  = add_quadriture(fErr[0], rhs.fErr[0]);
	out.fErr[1] = add_quadriture(fErr[1], rhs.fErr[1]);

	out.fSys[0]  = add_quadriture(fSys[0], rhs.fSys[0]);
	out.fSys[1] = add_quadriture(fSys[1], rhs.fSys[1]);

	out.fNominal -= rhs.fNominal;

	return out;
}

UDouble_t UDouble_t::operator* (const UDouble_t& rhs) const
{
  UDouble_t out (*this);

	double lowRel  = add_quadriture(GetRelErrLow(),  rhs.GetRelErrLow());
	double highRel = add_quadriture(GetRelErrHigh(), rhs.GetRelErrHigh());

	double lowRelS  = add_quadriture(GetRelSysErrLow(),  rhs.GetRelSysErrLow());
	double highRelS = add_quadriture(GetRelSysErrHigh(), rhs.GetRelSysErrHigh());

	out.SetNominal(fNominal * rhs.fNominal);

	out.SetRelErrLow(lowRel);
	out.SetRelErrHigh(highRel);

	out.SetRelSysErr(lowRelS, highRelS);
	
	return out;
}

UDouble_t UDouble_t::operator/ (const UDouble_t& rhs) const
{
  UDouble_t out (*this);

	double lowRel  = add_quadriture(GetRelErrLow(),  rhs.GetRelErrLow());
	double highRel = add_quadriture(GetRelErrHigh(), rhs.GetRelErrHigh());

	double lowRelS  = add_quadriture(GetRelSysErrLow(),  rhs.GetRelSysErrLow());
	double highRelS = add_quadriture(GetRelSysErrHigh(), rhs.GetRelSysErrHigh());

	out.SetNominal(fNominal / rhs.fNominal);

	out.SetRelErrLow(lowRel);
	out.SetRelErrHigh(highRel);

	out.SetRelSysErr(lowRelS, highRelS);

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

	out.fErr[0]    *= fabs(rhs);
	out.fErr[1]    *= fabs(rhs);

	out.fSys[0]    *= fabs(rhs);
	out.fSys[1]    *= fabs(rhs);

	return out;
}

UDouble_t UDouble_t::operator/ (const double& rhs) const
{
	UDouble_t out = *this * (1./rhs);
	return out;
}

void UDouble_t::Print() const
{
	std::cout << *this << "\n";
}


std::ostream& operator<< (std::ostream& strm, const UDouble_t& ud)
{
	if(ud.fErr[1] == 0 && ud.fErr[0] == 0) {
		strm << ud.fNominal;
	}
	else if(ud.fSys[0] == 0 && ud.fSys[1] == 0) {
		if(ud.fErr[1] != ud.fErr[0])
			strm << ud.fNominal << " +" << ud.fErr[1] << " / -" << ud.fErr[0];
		else
			strm << ud.fNominal << " +/- " << ud.fErr[1];
	} else {
		if(ud.fErr[1] != ud.fErr[0])
			strm << ud.fNominal << " +" << ud.fErr[1] << " / -" << ud.fErr[0] << " (stat)";
		else
			strm << ud.fNominal << " +/- " << ud.fErr[1] << " (stat)";
		if(ud.fSys[1] != ud.fSys[0])
			strm << " +" << ud.fSys[1] << " / -" << ud.fSys[0] << " (sys)";
		else
			strm << " +/- " << ud.fSys[1] << " (sys)";
	}
}

UDouble_t UDouble_t::pow(const UDouble_t& z, double x)
{
	UDouble_t out = z;

	out.fNominal = ::pow(z.fNominal, x);

	out.fErr[0] = fabs(out.fNominal * fabs(x) * z.fErr[0]  / z.fNominal);
	out.fErr[1] = fabs(out.fNominal * fabs(x) * z.fErr[1] / z.fNominal);

	out.fSys[0] = fabs(out.fNominal * fabs(x) * z.fSys[0]  / z.fNominal);
	out.fSys[1] = fabs(out.fNominal * fabs(x) * z.fSys[1] / z.fNominal);

	return out;
}

UDouble_t UDouble_t::pow(double x, const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = ::pow(x, z.fNominal);

	out.fErr[0]  = fabs(out.fNominal * ::log(x) * z.fErr[0]);
	out.fErr[1] = fabs(out.fNominal * ::log(x) * z.fErr[1]);

	out.fSys[0]  = fabs(out.fNominal * ::log(x) * z.fSys[0]);
	out.fSys[1] = fabs(out.fNominal * ::log(x) * z.fSys[1]);

	return out;
}

UDouble_t UDouble_t::log(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fErr[1]  = z.fErr[1]  / z.fNominal;
	out.fErr[1] = z.fErr[1] / z.fNominal;

	out.fSys[1]  = z.fSys[1]  / z.fNominal;
	out.fSys[1] = z.fSys[1] / z.fNominal;

	out.fNominal = ::log(z.fNominal);
}

UDouble_t UDouble_t::exp(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = ::exp(z.fNominal);

	out.fErr[1]  = z.fErr[1]  / out.fNominal;
	out.fErr[1] = z.fErr[1] / out.fNominal;

	out.fSys[1]  = z.fSys[1]  / out.fNominal;
	out.fSys[1] = z.fSys[1] / out.fNominal;
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
		yerrl[i] = add_quadriture(y[i].GetErrLow(),  y[i].GetSysErrLow());
		yerrh[i] = add_quadriture(y[i].GetErrHigh(), y[i].GetSysErrHigh());
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
		xerrl[i] = add_quadriture(x[i].GetErrLow(),  x[i].GetSysErrLow());
		xerrh[i] = add_quadriture(x[i].GetErrHigh(), x[i].GetSysErrHigh());
		yerrl[i] = add_quadriture(y[i].GetErrLow(),  y[i].GetSysErrLow());
		yerrh[i] = add_quadriture(y[i].GetErrHigh(), y[i].GetSysErrHigh());
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
