///
/// \file Uncertainty.hxx
/// \brief Defines a class for storing and propagating gaussian uncertainties.
///
#ifndef UNCERTAINTY_HXX
#define UNCERTAINTY_HXX

class UDouble_t {
public:
	UDouble_t(double nominal, double err_low, double err_high = 0.);
	UDouble_t(double nomilal = 0.);

	operator double() const { return fNominal; }
	UDouble_t operator+(const UDouble_t& rhs) const;
	UDouble_t operator-(const UDouble_t& rhs) const;
	UDouble_t operator*(const UDouble_t& rhs) const;
	UDouble_t operator/(const UDouble_t& rhs) const;

	UDouble_t operator+(const double& rhs) const;
	UDouble_t operator-(const double& rhs) const;
	UDouble_t operator*(const double& rhs) const;
	UDouble_t operator/(const double& rhs) const;

	UDouble_t& operator+= (const UDouble_t& rhs) { *this = *this + rhs; return *this; }
	UDouble_t& operator-= (const UDouble_t& rhs) { *this = *this - rhs; return *this; }
	UDouble_t& operator*= (const UDouble_t& rhs) { *this = *this * rhs; return *this; }
	UDouble_t& operator/= (const UDouble_t& rhs) { *this = *this / rhs; return *this; }

	UDouble_t& operator+= (const double& rhs) { *this = *this + rhs; return *this; }
	UDouble_t& operator-= (const double& rhs) { *this = *this - rhs; return *this; }
	UDouble_t& operator*= (const double& rhs) { *this = *this * rhs; return *this; }
	UDouble_t& operator/= (const double& rhs) { *this = *this / rhs; return *this; }

	UDouble_t& operator- () { (*this) *= -1; return *this; }

	double GetNominal() const { return fNominal; }
	double GetErrLow()  const { return fLow;     }
	double GetErrHigh() const { return fHigh;    }
	void Print() const;

private:
	double fNominal;
	double fLow;
	double fHigh;

	friend UDouble_t pow(const UDouble_t&, double);
	friend UDouble_t pow(double, const UDouble_t&);
	friend UDouble_t log(const UDouble_t&);
	friend UDouble_t exp(const UDouble_t&);
};

UDouble_t pow(const UDouble_t& z, double x);
UDouble_t pow(double z, const UDouble_t& x);
UDouble_t log(const UDouble_t& z);
UDouble_t exp(const UDouble_t& z);

inline UDouble_t operator+ (const double& lhs, const UDouble_t& rhs) { return  rhs + lhs; }
inline UDouble_t operator- (const double& lhs, const UDouble_t& rhs) { return -rhs + lhs; }
inline UDouble_t operator* (const double& lhs, const UDouble_t& rhs) { return  rhs * lhs; }
inline UDouble_t operator/ (const double& lhs, const UDouble_t& rhs) { return  lhs * pow(rhs, -1.); }


#ifdef USE_ROOT
#include <RTypes.h>
#include <TGraphAsymmErrors.h>

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, Double_t*  x, UDouble_t* y);
TGraphAsymmErrors* PlotUncertainties(Int_t npoints, UDouble_t* x, UDouble_t* y);

#pragma link C++ class std::vector<UDouble_t>+;

#endif

#endif
