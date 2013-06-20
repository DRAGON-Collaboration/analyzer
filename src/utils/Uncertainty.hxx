///
/// \file Uncertainty.hxx
/// \brief Defines a class for storing and propagating gaussian uncertainties.
///
#ifndef UNCERTAINTY_HXX
#define UNCERTAINTY_HXX
#include <iostream>

class UDouble_t {
public:
	UDouble_t(double nominal, double err_low, double err_high = 0.);
	UDouble_t(double nomilal = 0.);

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

	double GetNominal()    const { return fNominal;       }
	double GetErrLow()     const { return fLow;           }
	double GetErrHigh()    const { return fHigh;          }
	double GetRelErrLow()  const { return fLow/fNominal;  }
	double GetRelErrHigh() const { return fHigh/fNominal; }

	void SetNominal(double nominal) { fNominal = nominal;   }
	void SetErrLow(double err)      { fLow  = err;          }
	void SetErrHigh(double err)     { fHigh = err;          }
	void SetRelErrLow(double err)   { fLow  = err*fNominal; }
	void SetRelErrHigh(double err)  { fHigh = err*fNominal; }
	void SetErr(double err)    { SetErrLow(err);    SetErrHigh(err);    }
	void SetRelErr(double err) { SetRelErrLow(err); SetRelErrHigh(err); }

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
	double fLow;
	double fHigh;
	
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

#pragma link C++ class std::vector<UDouble_t>+;

#endif

#endif
