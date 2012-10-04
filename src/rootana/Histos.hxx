/*!
 * \file Histos.hxx
 * \author G. Christian
 * \brief Defines some classes for creating hitograms in rootana.
 */
#ifndef DRAGON_ROOTANA_HISTOS_HXX
#define DRAGON_ROOTANA_HISTOS_HXX
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TDirectory.h>
#include "utils/Valid.hxx"
#include "DataPointer.hxx"

/// Encloses all rootana-specific classes
namespace rootana {

/// Abstract interface for Rootana histograms
/*!
 * Provides a pure virtual interface with functions needed in the standard
 * ROOTANA framework.
 */
class HistBase {
public:
	/// Empty
	HistBase() { }
	/// Empty
	virtual ~HistBase() { }
	/// Fills the histogram with appropriate data
	virtual Int_t fill() = 0;
	/// Writes the histogram to disk
	virtual void write() = 0;
	/// Clears the histogram
	virtual void clear() = 0;
	/// Returns histogram name
	virtual const char* name() const = 0;
	/// Sets owner TDirectory
	virtual void set_directory(TDirectory*) = 0;
};

/// Rootana histogram class
/*!
 * Wraps a normal TH*D, along with pointer(s) to the parameters with
 * which the histogram is filled. Dereference operators provide "smart pointer"
 * syntax.
 * \tparam T Type of ROOT histogram being wrapped. Only TH1D, TH2D, and TH3D
 * are allowed, and must be chosen with the right constructor.
 */
template <class T>
class Hist: public HistBase {
protected:
	T* fHist; ///< Internal ROOT histogram
	const DataPointer* fParamx; ///< X-axis parameter
	const DataPointer* fParamy; ///< Y-axis parameter
	const DataPointer* fParamz; ///< Z-axis parameter

public:
	/// 1d (TH1D) constructor
	Hist(T* hist, const DataPointer* param);
	/// 2d (TH2D) constructor
	Hist(T* hist, const DataPointer* paramx, const DataPointer* paramy);
	/// 3d (TH3D) constructor
	Hist(T* hist, const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz);
	/// Frees memory allocated to fHist, and fParamx (y, z)
	virtual ~Hist();

private:
	/// Copy constructor
	Hist(const Hist& other);
	/// Equivalency operator
	Hist& operator= (const Hist& other);

public:
	/// Grants access to the internal histogram
	T* get() { return fHist; }
	/// Arrow operator, for pointer-like behavior
	T* operator->() { return fHist; }
	/// Dereference operator, for pointer-like bahavior
	T& operator*() { return *fHist; }

	/// Calls TH1::Fill using fParamx, fParamy, fParamz
	virtual Int_t fill();
	/// Calls TH1::Write()
	virtual void write()
		{ fHist->Write(); }
	/// Calls TH1::GetName()
	virtual const char* name() const
		{ return fHist->GetName(); }
	/// Calls TH1::Clear()
	virtual void clear()
		{ fHist->Clear(); }
	/// Calls TH1::SetDirectory
	virtual void set_directory(TDirectory* directory)
		{ fHist->SetDirectory(directory); }
};

/// Specialized case of TH2D that displays "summary" information
/*!
 *  By "summary", we mean that each y-axis bin corresponds to a single parameter,
 *  and the x-axis corresponds to parameter values. Currently this is only supported
 *  for arrays, not individual separate parameters.
 */
class SummaryHist: public Hist<TH2D> {
public:
	/// Mimics TH1D constructor (use x-axis bins), with array specification determining y-axis binning
	SummaryHist(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray);
	/// Empty, ~Hist<TH2D> takes care of everything
	virtual ~SummaryHist() { }
	/// Override the fill() method to act appropriately for summary histograms
	virtual Int_t fill();
};


// INLINE IMPLEMENTATIONS //

template <class T>
inline rootana::Hist<T>::~Hist()
{
	delete fHist;
	delete fParamx;
	delete fParamy;
	delete fParamz;
}

template <class T>
inline rootana::Hist<T>::Hist(const Hist& other):
	fParamx(other.fParamx), fParamy(other.fParamy), fParamz(other.fParamz)
{
	fHist = new T (*(other.fHist));
}

template <class T>
inline rootana::Hist<T>& rootana::Hist<T>::operator= (const Hist& other)
{
	fParamx = other.fParamx;
	fParamy = other.fParamy;
	fParamz = other.fParamz;
	fHist = new T (*(other.fHist));
	return *this;
}

/// Specialized constructor for TH1D
template <>
inline rootana::Hist<TH1D>::Hist(TH1D* hist, const DataPointer* param):
	fHist(hist), fParamx(param), fParamy(DataPointer::New()), fParamz(DataPointer::New())
{ }

/// Specialized constructor for TH2D
template <>
inline rootana::Hist<TH2D>::Hist(TH2D* hist, const DataPointer* paramx, const DataPointer* paramy):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(DataPointer::New())
{ 
	fHist->SetOption("COLZ");
}

/// Specialized constructor for TH3D
template <>
inline rootana::Hist<TH3D>::Hist(TH3D* hist, const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(paramz)
{ }

/// Specialized fill() for TH1D
template <>
inline Int_t rootana::Hist<TH1D>::fill()
{
	if (is_valid(fParamx->get()))
		return fHist->Fill (fParamx->get());
	else return 0;
}

/// Specialized fill() for TH2D
template <>
inline Int_t rootana::Hist<TH2D>::fill()
{
	if (is_valid(fParamx->get(), fParamy->get()))
		return fHist->Fill (fParamx->get(), fParamy->get());
	else return 0;
}

/// Specialized fill() for TH3D
template<>
inline Int_t rootana::Hist<TH3D>::fill()
{
	return fHist->Fill (fParamx->get(), fParamy->get(), fParamz->get());
}

inline rootana::SummaryHist::SummaryHist(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray):
	Hist<TH2D>(new TH2D(name, title, nbins, low, high, paramArray->length(), 0, paramArray->length()), paramArray, DataPointer::New())
{ 
	/*! Calls Hist<TH2D>::Hist() with appropriate arguments for a summary histogram. */
}

inline Int_t rootana::SummaryHist::fill()
{
	Int_t filled = 0;
	for (Int_t bin = 0; bin < fHist->GetYaxis()->GetNbins(); ++bin) {
		if (is_valid(fParamx->get(bin))) {
			fHist->Fill (fParamx->get(bin), bin);
			filled = 1;
		}
	}
	return filled;
}

} // namespace rootana

#endif
