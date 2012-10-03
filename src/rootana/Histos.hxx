#ifndef DRAGON_ROOTANA_HISTOS_HXX
#define DRAGON_ROOTANA_HISTOS_HXX
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>

/// Encloses all rootana-specific classes
namespace rootana {

template <class T>
class Hist {
private:
	T* fHist;
	const double* const fParamx;
	const double* const fParamy;
	const double* const fParamz;
public:
	Hist(T* hist, const double& param);
	Hist(T* hist, const double& paramx, const double& paramy);
	Hist(T* hist, const double& paramx, const double& paramy, const double& paramz);
	~Hist();

	Int_t Fill();

	T* operator->() { return fHist; }
	T* Get() { return fHist; }
	T& operator*() { return *fHist; }

private:
	Hist(const Hist& other) { }
	Hist& operator= (const Hist& other) { return *this; }
};

}

template <class T>
rootana::Hist<T>::~Hist()
{
	delete fHist;
}

namespace rootana {

template <>
rootana::Hist<TH1D>::Hist(TH1D* hist, const double& param):
	fHist(hist), fParamx(&param), fParamy(0), fParamz(0)
{ }

template <>
rootana::Hist<TH2D>::Hist(TH2D* hist, const double& paramx, const double& paramy):
	fHist(hist), fParamx(&paramx), fParamy(&paramy), fParamz(0)
{ }

template <>
rootana::Hist<TH3D>::Hist(TH3D* hist, const double& paramx, const double& paramy, const double& paramz):
	fHist(hist), fParamx(&paramx), fParamy(&paramy), fParamz(&paramz)
{ }

template <>
Int_t rootana::Hist<TH1D>::Fill()
{
	return fHist->Fill (*fParamx);
}

template <>
Int_t rootana::Hist<TH2D>::Fill()
{
	return fHist->Fill (*fParamx, *fParamy);
}

template<>
Int_t rootana::Hist<TH3D>::Fill()
{
	return fHist->Fill (*fParamx, *fParamy, *fParamz);
}

}

#endif
