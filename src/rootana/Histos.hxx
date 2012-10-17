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
	rootana::Cut fCut; ///< Cut (gate) condition

public:
	/// Sets fCut to NULL, otherwise empty
	HistBase():	fCut(0) { }
	/// Empty
	virtual ~HistBase() { }
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
	/// Sets histogram name
	virtual void set_name(const char* name) = 0;
	/// Returns histogram name
	virtual const char* name() const = 0;
	/// Sets owner TDirectory
	virtual void set_directory(TDirectory*) = 0;
	/// For testing
	void test() { printf ("test\n"); }
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
	TDirectory* fHistOwner;     /// Directory owning fHist

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
	/// Calls TH1::SetName()
	virtual void set_name(const char* name)
		{ fHist->SetName(name); }
	/// Calls TH1::GetName()
	virtual const char* name() const
		{ return fHist->GetName(); }
	/// Calls TH1::Clear()
	virtual void clear()
		{ fHist->Clear(); }
	/// Calls TH1::SetDirectory
	virtual void set_directory(TDirectory* directory)
		{
			fHist->SetDirectory(directory);
			fHistOwner = directory;
		}
};

/// Specialized case of TH2D that displays "summary" information
/*!
 *  By "summary", we mean that each y-axis bin corresponds to a single parameter,
 *  and the x-axis corresponds to parameter values. Currently this is only supported
 *  for arrays, not individual separate parameters.
 */
class SummaryHist: public Hist<TH2D> {
public:
	/// Sets x-axis binning from \e hist, y-axis from \e paramArray
	SummaryHist(TH1D* hist, const DataPointer* paramArray);
	/// Empty, ~Hist<TH2D> takes care of everything
	virtual ~SummaryHist() { }
	/// Override the fill() method to act appropriately for summary histograms
	virtual Int_t fill();
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
	/*!
	 * \note Delete fHist only if it is not owned by a directory; otherwise, the
	 * owning diretory must be allowed to take care of things.
	 */
	delete fHist;
	delete fParamx;
	delete fParamy;
	delete fParamz;
}

template <class T>
inline rootana::Hist<T>::Hist(const Hist& other)
{
	fParamx = new DataPointer(other.fParamx);
	fParamy = new DataPointer(other.fParamy);
	fParamz = new DataPointer(other.fParamz);
	fHist   = new T (*(other.fHist));
	fHistOwner = fHist->GetDirectory();
}

template <class T>
inline rootana::Hist<T>& rootana::Hist<T>::operator= (const Hist& other)
{
	fParamx = new DataPointer(other.fParamx);
	fParamy = new DataPointer(other.fParamy);
	fParamz = new DataPointer(other.fParamz);
	fHist   = new T (*(other.fHist));
	fHistOwner = fHist->GetDirectory();
	return *this;
}

/// Specialized constructor for TH1
template <>
inline rootana::Hist<TH1D>::Hist(TH1D* hist, const DataPointer* param):
	fHist(hist), fParamx(param), fParamy(DataPointer::New()), fParamz(DataPointer::New())
{
	fHistOwner = hist ? hist->GetDirectory() : 0;
}

/// Specialized constructor for TH2D
template <>
inline rootana::Hist<TH2D>::Hist(TH2D* hist, const DataPointer* paramx, const DataPointer* paramy):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(DataPointer::New())
{ 
	fHistOwner = hist ? hist->GetDirectory() : 0;
	if(fHist) fHist->SetOption("COLZ");
}

/// Specialized constructor for TH3D
template <>
inline rootana::Hist<TH3D>::Hist(TH3D* hist, const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(paramz)
{
 	fHistOwner = hist ? hist->GetDirectory() : 0;
}

/// Specialized fill() for TH1D
template <>
inline Int_t rootana::Hist<TH1D>::fill()
{
	/*! Fills the histogram if x param is valid and fCut is satisfied */
	if ( is_valid(fParamx->get()) && apply_cut() )
		return fHist->Fill (fParamx->get());
	else return 0;
}

/// Specialized fill() for TH2D
template <>
inline Int_t rootana::Hist<TH2D>::fill()
{
	/*! Fills the histogram if x,y params are valid and fCut is satisfied */
	if ( is_valid(fParamx->get(), fParamy->get()) && apply_cut() )
		return fHist->Fill (fParamx->get(), fParamy->get());
	else return 0;
}

/// Specialized fill() for TH3D
template<>
inline Int_t rootana::Hist<TH3D>::fill()
{
	/*! Fills the histogram if x,y,z params are valid and fCut is satisfied */
	if (is_valid(fParamx->get(), fParamy->get(), fParamz->get()) && apply_cut() )
		return fHist->Fill (fParamx->get(), fParamy->get(), fParamz->get());
	else return 0;
}

namespace {
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
} }

inline rootana::SummaryHist::SummaryHist(TH1D* hist, const DataPointer* paramArray):
	Hist<TH2D>(get_summary_2d(hist, paramArray), paramArray, DataPointer::New())
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
