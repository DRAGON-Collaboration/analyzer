///
/// \file LinearFitter.hxx
/// \brief Defines a class to fit functions to a set of points with
///  asymmetric error bars in x and y.
#ifndef DRAGON_LINEAR_FITTER_HXX
#define DRAGON_LINEAR_FITTER_HXX
#include <memory>
#include <vector>
#include <TF1.h>
#include <TMinuit.h>

#include "Uncertainty.hxx"

class TGraph;

namespace dragon {

/// Generic linear fitter class. Uses MINOS to calculate asymmetric errors on
/// parameters and can handle asymmetric x- and y-axis errors.
class LinearFitter {
public:
	/// Ctor
	LinearFitter(int printLevel = -1);
	/// Fit a TGraph
	void Fit(TGraph* graph);
	/// Fit points defined by vectors
	void Fit(const std::vector<double>& x, const std::vector<double>& y,
					 const std::vector<double>& exl, const std::vector<double>& exh,
					 const std::vector<double>& eyl, const std::vector<double>& eyh);

	UDouble_t GetSlope() const  { return fSlope; }
	UDouble_t GetOffset() const { return fOffset; }
	TF1* GetFunction() const { return fFunction.get(); }
	TMinuit* GetMinuit() const { return fMinuit.get(); }

	/// Calculates chi2 using the asymmetric error method, see TGraph::Fit ROOT documentation
	static double Chi2err(double M, double B, double x, double y, double exl, double exh, double eyl, double eyh);

private:
	std::auto_ptr<TMinuit> fMinuit;
	UDouble_t fSlope;
	UDouble_t fOffset;
	std::auto_ptr<TF1> fFunction;
};

} // namespace dragon


#endif
