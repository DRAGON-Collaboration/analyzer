///
/// \file Uncertainty.hxx
/// \brief Defines a class for storing and propagating gaussian uncertainties.
///
/// \todo Rewrite the implementation of this class using TPython and ufloat package.
/// Example:
/// \code
/// TPython python;
/// python.Exec("from uncertainties import ufloat");
/// python.Exec("u = ufloat((2, pow(2, 0.5)))");
/// Double_t un = python.Eval("u.nominal_value");
/// Double_t us = python.Eval("u.std_dev()");
/// fNominal = un;
/// fErr     = us;
/// \endcode
///
#ifndef UNCERTAINTY_HXX
#define UNCERTAINTY_HXX
#ifndef __MAKECINT__
#include <iostream>
#endif

class UDouble_t {
public:
	UDouble_t(double nominal, double err_low, double err_high, double sys_low, double sys_high);
	UDouble_t(double nominal, double err_low, double err_high = 0.);
	UDouble_t(double nominal = 0.);

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

	UDouble_t& operator= (const double& val) { SetNominal(val); SetRelErr(0); return *this; }

	double GetNominal()    const { return fNominal;         }
	double GetErrLow()     const { return fErr[0];          }
	double GetErrHigh()    const { return fErr[1];          }
	double GetRelErrLow()  const { return fErr[0]/fNominal; }
	double GetRelErrHigh() const { return fErr[1]/fNominal; }

	void SetNominal(double nominal) { fNominal = nominal;      }
	void SetErrLow(double err)      { fErr[0]  = err;          }
	void SetErrHigh(double err)     { fErr[1] = err;           }
	void SetRelErrLow(double err)   { fErr[0]  = err*fNominal; }
	void SetRelErrHigh(double err)  { fErr[1] = err*fNominal;  }
	void SetErr(double err)    { SetErrLow(err);    SetErrHigh(err);    }
	void SetRelErr(double err) { SetRelErrLow(err); SetRelErrHigh(err); }

	double GetSysErrLow()  const { return fSys[0]; }
	double GetSysErrHigh() const { return fSys[1]; }
	double GetRelSysErrLow() const  { return GetSysErrLow()  / fNominal; }
	double GetRelSysErrHigh() const { return GetSysErrHigh() / fNominal; }

	void SetSysErr(double low, double high = -1)
		{ fSys[0] = low; fSys[1] = high < 0 ? fSys[0] : high; }
	void SetRelSysErr(double low, double high = -1)
		{ fSys[0] = low*fNominal; fSys[1] = high < 0 ? fSys[0] : high*fNominal; }

	void Print() const;

public:
	static UDouble_t pow(const UDouble_t& z, double x);
	static UDouble_t pow(double z, const UDouble_t& x);
	static UDouble_t log(const UDouble_t& z);
	static UDouble_t exp(const UDouble_t& z);
	static UDouble_t sqrt(const UDouble_t& z) { return pow(z, 0.5); }
	static UDouble_t abs(const UDouble_t& z)  { return sqrt(pow(z,2.)); }
	static UDouble_t Mean(long n, const UDouble_t* u);
	static UDouble_t StdDev(long n, const UDouble_t* u);

private:
	double fNominal;
	double fErr[2]; // 0 = low, 1 = high
	double fSys[2]; // 0 = low, 1 = high

	friend std::ostream& operator<< (std::ostream&, const UDouble_t&);
};

std::ostream& operator<< (std::ostream&, const UDouble_t&);

inline UDouble_t operator+ (const double& lhs, const UDouble_t& rhs) { return  rhs + lhs; }
inline UDouble_t operator- (const double& lhs, const UDouble_t& rhs) { UDouble_t rhs1(rhs); return -rhs1 + lhs; }
inline UDouble_t operator* (const double& lhs, const UDouble_t& rhs) { return  rhs * lhs; }
inline UDouble_t operator/ (const double& lhs, const UDouble_t& rhs) { return  lhs * UDouble_t::pow(rhs, -1.); }


#ifdef USE_ROOT
#include <TMath.h>
#include <Rtypes.h>
#include <TGraphAsymmErrors.h>

TGraphAsymmErrors* PlotUncertainties(Int_t npoints, const Double_t*  x, const UDouble_t* y);
TGraphAsymmErrors* PlotUncertainties(Int_t npoints, const UDouble_t* x, const UDouble_t* y);

#endif

#endif
