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
#include "Cut.hxx"



/// \brief Encloses all rootana-specific classes
namespace rootana {

/// Abstract interface for Rootana histograms
/*!
 * Provides a pure virtual interface with functions needed in the standard
 * ROOTANA framework.
 * 
 * Also handles application of cuts for derived classes
 */
class HistBase {
private:
	/// Cut (gate) condition
	rootana::Cut fCut; //!

public:
	/// Sets fCut to NULL, otherwise empty
	HistBase():	fCut(0) { }
	/// Empty
	virtual ~HistBase() { printf ("bye\n"); }
	/// Sets the cut (gate) condition
	void set_cut(const Cut& cut);
  /// Applies the cut condition
	bool apply_cut();

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
class Hist: public T, public HistBase {
protected:
	const DataPointer* fParamx; //!///< X-axis parameter
	const DataPointer* fParamy; //!///< Y-axis parameter
	const DataPointer* fParamz; //!///< Z-axis parameter

public:
	/// Default constructor for streamer
	Hist(): T(), HistBase() { }
	/// 1d (TH1D) constructor
	Hist(const char* name, const char* title, Int_t nbins, Double_t low, Double_t high, const DataPointer* param);
	/// 2d (TH2D) constructor
	Hist(const char* name, const char* title,
			 Int_t nbinsx, Double_t lowx, Double_t highx,
			 Int_t nbinsy, Double_t lowy, Double_t highy,
			 const DataPointer* paramx, const DataPointer* paramy);
	/// 3d (TH3D) constructor
	Hist(const char* name, const char* title,
			 Int_t nbinsx, Double_t lowx, Double_t highx,
			 Int_t nbinsy, Double_t lowy, Double_t highy,
			 Int_t nbinsz, Double_t lowz, Double_t highz,
			 const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz);
	/// Frees memory allocated to fHist, and fParamx (y, z)
	virtual ~Hist();

private:
	/// Copy constructor
	Hist(const Hist& other);
	/// Equivalency operator
	Hist& operator= (const Hist& other);

public:
	/// Calls TH1::Fill using fParamx, fParamy, fParamz
	virtual Int_t fill();
	/// Calls TH1::Write()
	virtual void write()
		{ this->Write(); }
	/// Calls TH1::GetName()
	virtual const char* name() const
		{ return this->GetName(); }
	/// Calls TH1::Clear()
	virtual void clear()
		{ this->Clear(); }
	/// Calls TH1::SetDirectory
	virtual void set_directory(TDirectory* directory)
		{ this->SetDirectory(directory); }

	/// CINT ClassDef
	ClassDef(Hist, 0);
};

/// Specialized case of TH2D that displays "summary" information
/*!
 *  By "summary", we mean that each y-axis bin corresponds to a single parameter,
 *  and the x-axis corresponds to parameter values. Currently this is only supported
 *  for arrays, not individual separate parameters.
 */
class SummaryHist: public Hist<TH2D> {
public:
	/// Default constructor for streamer
	SummaryHist(): Hist<TH2D>() { }
	/// Sets x-axis binning from \e hist, y-axis from \e paramArray
	SummaryHist(const char* name, const char* title, Int_t nbins, Double_t low, Double_t high, const DataPointer* paramArray);
	/// Empty, ~Hist<TH2D> takes care of everything
	virtual ~SummaryHist() { }
	/// Override the fill() method to act appropriately for summary histograms
	virtual Int_t fill();

	/// CINT ClassDef
	ClassDef(SummaryHist, 0);
};


// INLINE IMPLEMENTATIONS //

inline void rootana::HistBase::set_cut(const Cut& cut)
{
	fCut.reset(cut);
}

inline bool rootana::HistBase::apply_cut()
{
	/*! \returns true if fCut.get() == 0, otherwise, the value of fCut.operator() */
	return !fCut.get() ? true: fCut();
}

template <class T>
inline rootana::Hist<T>::~Hist()
{
	delete fParamx;
	delete fParamy;
	delete fParamz;
}

template <class T>
inline rootana::Hist<T>::Hist(const Hist& other):
	T(*other)
{
	fParamx = new DataPointer(other.fParamx);
	fParamy = new DataPointer(other.fParamy);
	fParamz = new DataPointer(other.fParamz);
}

template <class T>
inline rootana::Hist<T>& rootana::Hist<T>::operator= (const Hist& other)
{
	T::operator= (other);
	fParamx = new DataPointer(other.fParamx);
	fParamy = new DataPointer(other.fParamy);
	fParamz = new DataPointer(other.fParamz);
	return *this;
}

template <class T>
inline rootana::Hist<T>::Hist(const char* name, const char* title,
		 Int_t nbins, Double_t low, Double_t high,
		 const DataPointer* param):
	T(), fParamx(param), fParamy(DataPointer::New()), fParamz(DataPointer::New())
{
	T::SetName(name);
	T::SetTitle(title);
	T::SetBins(nbins, low, high);
}

template <class T>
inline rootana::Hist<T>::Hist(const char* name, const char* title,
															Int_t nbinsx, Double_t lowx, Double_t highx,
															Int_t nbinsy, Double_t lowy, Double_t highy,
															const DataPointer* paramx, const DataPointer* paramy):
	fParamx(paramx), fParamy(paramy), fParamz(DataPointer::New())
{
	T::SetName(name);
	T::SetTitle(title);
	T::SetBins(nbinsx, lowx, highx, nbinsy, lowy, highy);
}

template <class T>
inline rootana::Hist<T>::Hist(const char* name, const char* title,
															Int_t nbinsx, Double_t lowx, Double_t highx,
															Int_t nbinsy, Double_t lowy, Double_t highy,
															Int_t nbinsz, Double_t lowz, Double_t highz,
															const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz):
	fParamx(paramx), fParamy(paramy), fParamz(paramz)
{
	T::SetName(name);
	T::SetTitle(title);
	T::SetBins(nbinsx, lowx, highx, nbinsy, lowy, highy, nbinsz, lowz, highz);
}

namespace {
/// Specialized fill() for TH1D
inline Int_t do_fill(rootana::Hist<TH1D>* hist,
										 const rootana::DataPointer* paramx,
										 const rootana::DataPointer* paramy,
										 const rootana::DataPointer* paramz)
{
	/*! Fills the histogram if x param is valid and fCut is satisfied */
	if ( is_valid(paramx->get()) && hist->apply_cut() )
		return hist->Fill (paramx->get());
	else return 0;
}

/// Specialized fill() for TH2D
inline Int_t do_fill(rootana::Hist<TH2D>* hist,
										 const rootana::DataPointer* paramx,
										 const rootana::DataPointer* paramy,
										 const rootana::DataPointer* paramz)
{
	/*! Fills the histogram if x,y params are valid and fCut is satisfied */
	if ( is_valid(paramx->get(), paramy->get()) && hist->apply_cut() )
		return hist->Fill (paramx->get(), paramy->get());
	else return 0;
}

/// Specialized fill() for TH3D
inline Int_t do_fill(rootana::Hist<TH3D>* hist,
										 const rootana::DataPointer* paramx,
										 const rootana::DataPointer* paramy,
										 const rootana::DataPointer* paramz)
{
	/*! Fills the histogram if x,y,z params are valid and fCut is satisfied */
	if (is_valid(paramx->get(), paramy->get(), paramz->get()) && hist->apply_cut() )
		return hist->Fill (paramx->get(), paramy->get(), paramz->get());
	else return 0;
}

inline TH2D* get_summary_2d(TH1D* xaxis, const DataPointer* param)
{
	const std::string name  = xaxis->GetName();
	const std::string title = xaxis->GetTitle();
	const Int_t  nX = xaxis->GetNbinsX();
	const double lX = xaxis->GetXaxis()->GetBinUpEdge(0);
	const double hX = xaxis->GetXaxis()->GetBinUpEdge(nX);
	const Int_t  nY = param->length();
	const double lY = 0.;
	const double hY = param->length();
	delete xaxis;
	return new TH2D(name.c_str(), title.c_str(), nX, lX, hX, nY, lY, hY);
}

}


template <class T>
inline Int_t rootana::Hist<T>::fill()
{
	return do_fill(this, fParamx, fParamy, fParamz);
}

inline rootana::SummaryHist::SummaryHist(const char* name, const char* title, Int_t nbins, Double_t low, Double_t high, const DataPointer* paramArray):
	Hist<TH2D>(name, title, nbins, low, high, paramArray->length(), 0, paramArray->length(), paramArray, DataPointer::New())
{ 
	/*!
	 *  Calls Hist<TH2D>::Hist() with appropriate arguments for a summary histogram.
	 *  Takes name, title, and x-axis binning from \e hist, then deletes it.
	 *  Takes y-axis binning from paramArray, and sets it as the internal data pointer.
	 *  Sets the expected y-axis data poiner to DataPointerNull.
	 */
}

inline Int_t rootana::SummaryHist::fill()
{
	/*! If fCut is satisfied, fills bin-by-bin whenever the corresponding param is valid */
	Int_t filled = 0;
	if (!apply_cut()) return filled;
	for (Int_t bin = 0; bin < this->GetYaxis()->GetNbins(); ++bin) {
		if (is_valid(fParamx->get(bin))) {
			this->Fill (fParamx->get(bin), bin);
			filled = 1;
		}
	}
	return filled;
}

} // namespace rootana

#endif
