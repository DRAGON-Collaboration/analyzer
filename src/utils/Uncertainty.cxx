#include <cmath>
#include <iostream>
#include "Uncertainty.hxx"


template <class T>
T add_quadriture(const T& lhs, const T& rhs)
{
	return sqrt(lhs*lhs + rhs*rhs);
}

template <class T>
T add_quadriture_relative(const T& lhs, const T& lhsE, const T& rhs, const T& rhsE)
{
	T lhsRel = lhsE / lhs;
	T rhsRel = rhsE / rhs;
	return add_quadriture(lhsRel, rhsRel);
}

UDouble_t::UDouble_t(double nominal)
{
	fNominal = nominal;
	fLow = fHigh = sqrt(nominal);
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
	out.fLow  = add_quadriture_relative(fNominal, fLow,  rhs.fNominal, rhs.fLow);
	out.fHigh = add_quadriture_relative(fNominal, fHigh, rhs.fNominal, rhs.fHigh);

	out.fNominal *= rhs.fNominal;
 	out.fLow     *= out.fLow;
	out.fHigh    *= out.fHigh;

	return out;
}

UDouble_t UDouble_t::operator/ (const UDouble_t& rhs) const
{
  UDouble_t out (*this);
	out.fLow  = add_quadriture_relative(fNominal, fLow,  rhs.fNominal, rhs.fLow);
	out.fHigh = add_quadriture_relative(fNominal, fHigh, rhs.fNominal, rhs.fHigh);

	out.fNominal /= rhs.fNominal;
 	out.fLow     *= out.fLow;
	out.fHigh    *= out.fHigh;

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
	out.fLow     *= abs(rhs);
	out.fHigh    *= abs(rhs);

	return out;
}

UDouble_t UDouble_t::operator/ (const double& rhs) const
{
	UDouble_t out = *this * (1/rhs);
	return out;
}

void UDouble_t::Print() const
{
	if(fHigh == 0 && fLow == 0)
		std::cout << fNominal << "\n";
	else if(fHigh != fLow)
		std::cout << fNominal << "+" << fHigh << "/-" << fLow << "\n";
	else
		std::cout << fNominal << "+/-" << fHigh << "\n";
}

UDouble_t pow(const UDouble_t& z, double x)
{
	UDouble_t out = z;

	out.fNominal = pow(z.fNominal, x);
	out.fLow  = out.fNominal * abs(x) * z.fLow  / z.fNominal;
	out.fHigh = out.fNominal * abs(x) * z.fHigh / z.fNominal;

	return out;
}

UDouble_t pow(double x, const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = pow(x, z.fNominal);
	out.fLow  = out.fNominal * log(x) * z.fLow;
	out.fHigh = out.fNominal * log(x) * z.fHigh;

	return out;
}

UDouble_t log(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fHigh  = z.fHigh  / z.fNominal;
	out.fHigh = z.fHigh / z.fNominal;
	out.fNominal = log(z.fNominal);
}

UDouble_t exp(const UDouble_t& z)
{
	UDouble_t out = z;

	out.fNominal = exp(z.fNominal);
	out.fHigh  = z.fHigh  / out.fNominal;
	out.fHigh = z.fHigh / out.fNominal;
}


#ifdef USE_ROOT

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, Double_t* x, UDouble_t* y)
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

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, UDouble_t* x, UDouble_t* y)
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
