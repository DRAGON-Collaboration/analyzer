/*!
 * \file Histos.hxx
 * \author G. Christian
 * \brief Defines some classes for creating hitograms in rootana.
 */
#ifndef DRAGON_ROOTANA_HISTOS_HXX
#define DRAGON_ROOTANA_HISTOS_HXX
#include <cassert>
#include <TColor.h>
#include <TDirectory.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include "utils/Valid.hxx"

/// Encloses all rootana-specific classes
namespace rootana {

/// Abstract interface for generic pointer-to-data classes
class DataPointer {
public:
	/// Empty
	DataPointer() { }
	/// Empty
	virtual ~DataPointer() { }
	/// Returns the data value as a double
	/*! \param index Array index, defaults to zero for single values */
	virtual double get (unsigned index = 0) const = 0;
	/// Returns array length
	virtual unsigned length() const = 0;
	/// Create a NULL instance
	static DataPointer* New ();
	/// Create from a single value
	template <typename T>
	static DataPointer* New (T& value);
	/// Create from an array
	template <typename T>
	static DataPointer* New (T* array, unsigned length);
};

/// Generic pointer-to-data class
/*!
 * Stores the address of a generic basic type, and converts the
 * value to a double using the inherited vitrual get() method.
 */
template <typename T>
class DataPointerT: public DataPointer {
private:
	/// Internal pointer-to-data
	const T* const fData;
	/// Length of the array pointer by fData
	const unsigned fLength;
public:
	/// Sets fData to the address of it's argument, fLength to zero
	DataPointerT(T& value): fData(&value), fLength(1) { }
	/// Sets fData & fLength to the argument values
	DataPointerT(T* array, unsigned length): fData(array), fLength(length) { }
	/// Returns the data value as a double
	double get(unsigned index = 0) const;
	/// Returns array length
	unsigned length() const { return fLength; }
};

/// Type corresponding to a NULL DataPointer
class DataPointerNull: public DataPointer { 
public:
	DataPointerNull() { }
	/// Runtime failure if we try to call get()
	double get(unsigned index = 0) const
		{ assert(0&&"Can't call this!"); return 0; }
	/// Returns zero
	unsigned length() const { return 0; }
};

/// Base histogram class
/*!
 * Abstract interface for Rootana histograms
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
	/// Sets owner TDirectory
	virtual void set_directory(TDirectory*) = 0;
	/// Creation function for "summary" histograms
	static HistBase* NewSummary(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray);
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
private:
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
	/// "Summary" hist (TH2D) constructor
	Hist(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray);
	/// Frees memory allocated to fHist
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
	Int_t fill();
	/// Calls TH1::Write()
	void write() { fHist->Write(); }
	/// Calls TH1::Clear()
	void clear() { fHist->Clear(); }
	/// Calls TH1::SetDirectory
	void set_directory(TDirectory* directory)
		{ fHist->SetDirectory(directory); }
};

} // namespace rootana

template <typename T>
inline double rootana::DataPointerT<T>::get(unsigned index) const
{
	assert(index < fLength);
	return *(fData + index);
}

inline rootana::DataPointer* rootana::DataPointer::New()
{
	return new DataPointerNull();
}

template <typename T>
inline rootana::DataPointer* rootana::DataPointer::New(T& value)
{
	return new DataPointerT<T>(value);
}

template <typename T>
inline rootana::DataPointer* rootana::DataPointer::New(T* array, unsigned length)
{
	return new DataPointerT<T>(array, length);
}

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
{	fHist = new T (*(other.fHist)); }

template <class T>
inline rootana::Hist<T>& rootana::Hist<T>::operator= (const Hist& other)
{
	fParamx = other.fParamx;
	fParamy = other.fParamy;
	fParamz = other.fParamz;
	fHist = new T (*(other.fHist));
	return *this;
}

namespace {
void rainbow_colz(TH2D* hist)
{
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;

	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	hist->SetContour(NCont);
	hist->SetOption("COLZ");
} }

namespace rootana {

template <>
inline rootana::Hist<TH1D>::Hist(TH1D* hist, const DataPointer* param):
	fHist(hist), fParamx(param), fParamy(DataPointer::New()), fParamz(DataPointer::New())
{ }

template <>
inline rootana::Hist<TH2D>::Hist(TH2D* hist, const DataPointer* paramx, const DataPointer* paramy):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(DataPointer::New())
{ 
	rainbow_colz(fHist);
}

template <>
inline rootana::Hist<TH3D>::Hist(TH3D* hist, const DataPointer* paramx, const DataPointer* paramy, const DataPointer* paramz):
	fHist(hist), fParamx(paramx), fParamy(paramy), fParamz(paramz)
{ }

template<>
inline rootana::Hist<TH2D>::Hist(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray):
	fParamx (paramArray), fParamy(DataPointer::New()), fParamz(DataPointer::New())
{
	fHist = new TH2D(name, title, nbins, low, high, paramArray->length(), 0, paramArray->length());
	rainbow_colz(fHist);
}

template <>
inline Int_t rootana::Hist<TH1D>::fill()
{
	if (is_valid(fParamx->get()))
		return fHist->Fill (fParamx->get());
	else return 0;
}

template <>
inline Int_t rootana::Hist<TH2D>::fill()
{
	// "Normal" 2D version
	if (fParamy->length()) {
		if (is_valid(fParamx->get(), fParamy->get()))
			return fHist->Fill (fParamx->get(), fParamy->get());
		else return 0;
	}

	// "Summary" version
	int filled = 0;
	for (int bin = 0 ; bin < fHist->GetYaxis()->GetNbins(); ++bin) {
		if (is_valid(fParamx->get(bin))) {
			filled = 1;
			fHist->Fill (fParamx->get(bin), bin);
		}
	}
	return filled;
}

template<>
inline Int_t rootana::Hist<TH3D>::fill()
{
	return fHist->Fill (fParamx->get(), fParamy->get(), fParamz->get());
}

inline HistBase* HistBase::NewSummary(const char* name, const char* title, Int_t nbins, double low, double high, const DataPointer* paramArray)
{
	return new Hist<TH2D>(name, title, nbins, low, high, paramArray);
}

}

#endif
