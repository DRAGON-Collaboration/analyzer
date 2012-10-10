/*!
 * \file Cut.hxx
 * \author G. Christian
 * \brief Defines "Cut" (logical condition) classes and functions.
 * \details In order to achieve runtime assignment of logical conditions
 * to histograms, we need a way to be able to create an instance of a class
 * defining the logical condition in the CINT interpreter. This file defines
 * such a class as an abstract class with pure virtual operator() applying
 * the cut condition. Derived classes implenent cut conditions for the standard
 * set of equivalency and logical operators. Furthermore, by definition of template
 * functions and operator overrides, we can create a fairly natural and intuitive
 * syntax for specifying a cut condition that can be utilized in histogram definition
 * files. See rootana::Cut documentation for use suggestions.
 *
 * Although the classes defined in this file could be used in a variety of ways, the
 * intended use is for instances of rootana::Cut to be created with a combination of
 * the template free functions defined in this file and the logical operators defined
 * in rootana::Cut (which return a rootana::Cut instance). Here's a few examples:
 * \code
 * rootana::Cut cut1 = Less (1, 5);
 * cut1(); // returns true (1 < 5)
 * 
 * rootana::Cut cut2 = Less (5, 1) || GreaterEqual (2, 2);
 * cut2(); // true
 *
 * int i = 1, j = 9;
 * rootana::Cut cut3 = Less (i, 5) && Greater (j, 1);
 * cut3(); // true
 * i = 4;
 * j = 2;
 * cut3(); // false
 * \endcode
 */
#ifndef ROOTANA_CUT_HXX
#define ROOTANA_CUT_HXX
#include <functional>
#include <algorithm>
#include <vector>
#include <TCutG.h>
#include "utils/Error.hxx"


/// Point-by-point arguments for use with rootana::Condition2D
#define ROOTANA_CUT2D_POINT_ARGS double x0 = 0, double y0 = 0, double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0, double x3 = 0, double y3 = 0, double x4 = 0, double y4 = 0, double x5 = 0, double y5 = 0, double x6 = 0, double y6 = 0, double x7 = 0, double y7 = 0, double x8 = 0, double y8 = 0, double x9 = 0, double y9 = 0, double x10 = 0, double y10 = 0, double x11 = 0, double y11 = 0, double x12 = 0, double y12 = 0, double x13 = 0, double y13 = 0

namespace rootana {

/// Abstract class defining a logical condition.
class Condition {
public:
	/// Virtual "clone" method
	/*! Derived classes should return a \c new instance of themselves */
	virtual Condition* clone() const = 0;

	/// Defines the "cut" logical condition
	virtual bool operator() () const = 0;

	/// Empty
	virtual ~Condition(){ }

	/// Logical AND with another Condition
	bool operator&& (const Condition& other) const
		{ return operator()() && other(); }

	/// Logical OR with another Condition
	bool operator|| (const Condition& other) const
		{ return operator()() || other(); }

	/// Logical NOT of \c this Condition
	bool operator!() const
		{ return !( operator()() ); }
};

/// Implementation of rootana::Condition for equivalency operators
/*!
 * Compares the value of two basic data types, converted to double.
 * 
 * \tparam T1 type of the first value to compare.
 * \tparam T2 type of the second value to compare.
 * \tparam F "Functional" class defining the equivalency comparision
 * (e.g. std::less_than<double>, std::less_equal<double>, etc.)
 */
template <class T1, class T2, class F>
class Equivalency: public Condition {
private:
	const T1& fV1; ///< First comparison value
	const T2& fV2; ///< Second comparison value
public:
	/// Sets fV1 and fV2
	Equivalency(const T1& v1, const T2& v2):
		fV1(v1), fV2(v2) { }
	/// Returns \c new copy of \c this
	Condition* clone() const
		{ return new Equivalency(*this); }
	/// Applies comparison class's operator() to fV1 and fV2, each converted to double.
	bool operator() () const
		{ return F()( fV1, fV2 ); }
};

/// Two-dimensional "polygon" Condition
/*!
 * Allows application of non-rectangular cut conditions.
 * Stores a reference to two parameters (x-axis and y-axis)
 * defining a cartesian point and an array of points defining
 * a closed polygon. The parenthesis operator checks whether or
 * not the point is inside the polygon.
 * 
 * \tparam T1 type of the x-axis parameter
 * \tparam T2 type of the y-axis parameter
 */
template <class T1, class T2>
class Condition2D: public Condition {
private:
	const T1& fXpar; ///< X-axis parameter value
	const T2& fYpar; ///< Y-axis parameter value
	std::vector<double> fXpoints; ///< X-axis contour points
	std::vector<double> fYpoints; ///< Y-axis contour points
public:
	/// Construct from a TCutG
	Condition2D(const T1& xpar, const T2& ypar, TCutG& cutg);

	/// Construct from manual point pair entries
	Condition2D(const T1& xpar, const T2& ypar, ROOTANA_CUT2D_POINT_ARGS);

	/// Returns \c new copy of \c this
	Condition* clone() const
		{ return new Condition2D(*this); }

	/// Checks if parameter points are inside the polygon
	bool operator() () const
		{ 
			double parx = fXpar, pary = fYpar;
			return inside<double> (parx, pary, fXpoints.size(), &fXpoints[0], &fYpoints[0]);
		}

private:
	/// Checks if the points are inside the polygon
	template <typename T>
	bool inside(T xp, T yp, Int_t np, const T* x, const T* y) const
		{
			/*! \note Copied from TMath::IsInside(), which has no version
				taking const array arguments - otherwise I'd just use it directly. */
			Int_t i, j = np-1 ;
			Bool_t oddNodes = kFALSE;

			for (i=0; i<np; i++) {
				if ((y[i]<yp && y[j]>=yp) || (y[j]<yp && y[i]>=yp)) {
					if (x[i]+(yp-y[i])/(y[j]-y[i])*(x[j]-x[i])<xp) {
						oddNodes = !oddNodes;
					}
				}
				j=i;
			}

			return oddNodes;
		}
};

/// "Cut" class to be used with histograms, etc.
/*!
 * This is nothing more than a "smart pointer"-style wrapper for a polymorphic
 * Condition*, but it allows one to take advantage of runtime polymorphism
 * without having to worry about explicit memory management. It provides the
 * basic "smart pointer" functionality found in, e.g. std::auto_ptr, and also
 * forwards logical operators from Condition.
 * \note See Cut.hxx for usage suggestions.
 */
class Cut {
private:
	Condition* fCondition; ///< Internal polyporphic Condition pointer
public:
	/// Sets fCondition
	Cut (Condition* condition):
		fCondition(condition) { }

	/// Makes a "deep" copy of the class
	/*!
	 * Calls the virtual "clone" method to create a \c new instance
	 * of the specific class of fCondition and assigns it's ownership 
	 * to \c this
	 */
	Cut (const Cut& other)
		{ fCondition = other.fCondition->clone(); }

	/// See Cut (const Cut& other) (copy constructor)
	Cut& operator= (Cut& other)
		{ free_(); fCondition = other.fCondition->clone(); return *this; }

	/// Frees memory allocated to fCondition
	~Cut()
		{ free_(); }

	/// Returns the address of fCondition
	Condition* get() const
		{ return fCondition; }

	/// Dereferences fCondition (as pointer)
	Condition* operator->() const
		{ return fCondition; }

	/// Dereferences fCondition (as explicit reference)
	Condition& operator*() const
		{ return *fCondition; }

	/// Re-assigns fCondition, freeing any previouly allocated memory
	void reset(Condition* c)
		{ free_(); fCondition = c; }

	/// Re-asigns fCondition as a deep copy of another Cut
	void reset(const Cut& other)
		{ free_(); fCondition = other.fCondition->clone(); }

	/// "Cut" operator, forwards to fCondition->operator()
	bool operator() () const
		{ return fCondition->operator() (); }

	/// NOT operator, forwards to fConditon->operator!
	Cut operator!  () const;

	/// AND operator, forwards to fCondition->operator&&
	Cut operator&& (const Cut& other) const;

	/// OR operator, forwards to fCondition->operator||
	Cut operator|| (const Cut& other) const;

private:
	/// Frees (deletes) fCondition, if it's allocated
	void free_()
		{ if (fCondition) delete fCondition; }
};


/// LOgical NOT implementation of Condition
/*!
 * Negates an internally stored instnace of rootana::Cut
 */
class NegatedCondition: public Condition {
private:
	Cut fCut; ///< Internal Cut, to be negated
public:
	/// Sets fCut
	NegatedCondition(const Cut& c):
		fCut(c) { }

	/// Returns \c new instance of \c this
	Condition* clone() const
		{ return new NegatedCondition(*this); }

	/// Returns the logical NOT of fCut().
	bool operator() () const
		{ return !fCut(); }
};

/// Implementation of condition for binary logicals (AND, OR).
/*!
 * Stores two Cuts and applies a binary logical operator to them.
 * \tparam L Binary logical class (std::logical_and<Condition>,
 *  std::logical_or<condition)
 */
template <class L>
class LogicalCondition: public Condition {
private:
	Cut fCut1; ///< First cut
	Cut fCut2; ///< Second cut
public:
	/// Sets fCut1, fCut2
	LogicalCondition(const Cut& c1, const Cut& c2):
		fCut1(c1), fCut2(c2) { }

	/// Returns \c new instance of \c this
	Condition* clone() const
		{ return new LogicalCondition(*this); }

	/// Applies the operator() of template parameter L.
	bool operator() () const
		{ return L() (*fCut1, *fCut2); }
};


/* Some overloaded operators of rootana::Cut rely on classes defined later,
 so impement here */

inline Cut Cut::operator! () const
{ Cut c(new NegatedCondition(*this)); return c; }

inline Cut Cut::operator&& (const Cut& other) const
{ Cut c(new LogicalCondition<std::logical_and<Condition> > (*this, other)); return c; }

inline Cut Cut::operator|| (const Cut& other) const
{ Cut c(new LogicalCondition<std::logical_or<Condition> > (*this, other)); return c; }

// Also implement long Condition2D constructors here
inline rootana::Condition2D::Condition2D(const T1& xpar, const T2& ypar, TCutG& cutg):
	fXpar(xpar), fYpar(ypar), fXpoints(cutg.GetN()), fYpoints(cutg.GetN())
{
	/*!
	 * \param xpar X-axis parameter
	 * \param xyar Y-axis parameter
	 * \param cutg Reference to a TCutG defining the closed polygon condition.
	 */
	std::copy (cutg.GetX(), cutg.GetX() + fXpoints.size(), fXpoints.begin());
	std::copy (cutg.GetY(), cutg.GetY() + fYpoints.size(), fYpoints.begin());
}

inline rootana::Condition2D::Condition2D(const T1& xpar, const T2& ypar, ROOTANA_CUT2D_POINT_ARGS):
	fXpar(xpar), fYpar(ypar), fXpoints(0), fYpoints(0)
{
	/*!
	 * \param xpar X-axis parameter
	 * \param xyar Y-axis parameter
	 * \param ROOTANA_CUT2D_POINT_ARGS (x0, y0, x1, y1</b>, etc.) Series of point pairs defining the \e closed polygon.
	 * Up to 14 points are allowed (CINT limitation prevents more). Once a point pair is set to the
	 * the default argument, any others coming after it in the parameter list are ignored. Also see
	 * <a href="http://trshare.triumf.ca/~gchristian/dragon/analyzer/html/_cut_8hxx.html">
	 * ROOTANA_CUT2D_POINT_ARGS</a>
	 */
	double* px[14] = { &x0, &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8, &x9, &x10, &x11, &x12, &x13 };
	double* py[14] = { &y0, &y1, &y2, &y3, &y4, &y5, &y6, &y7, &y8, &y9, &y10, &y11, &y12, &y13 };
	int n = 0;
	while(1) {
		if (n > 13) break;
		if (*(px[n]) == 0 && *(py[n]) == 0) break;
		++n;
	}

	for (int i=0; i< n; ++i) {
		fXpoints.push_back(*(px[i]));
		fYpoints.push_back(*(py[i]));
	}
	if (fXpoints.size() && fYpoints.size()) {
		if ( (*(fXpoints.end()-1) != *(fXpoints.begin())) ||
				 (*(fYpoints.end()-1) != *(fYpoints.begin())) ) {
			dragon::err::Warning("Condition2D") << "Non-closed polygon specification, points:";
			for (size_t i=0; i< fXpoints.size()-1; ++i) {
				std::cerr << "(" << fXpoints.at(i) << ", " << fYpoints.at(i) << "), ";
			}
			std::cerr << "(" << fXpoints.at(fXpoints.size()-1) << ", " << fYpoints.at(fXpoints.size()-1) << ")\n";
		}
	}
	else {
		dragon::err::Warning("Condition2D") << "Empty polygon specification";
	}
}

} // namespace rootana



/* Free functions */

/// "Less-than" (<) [ uses std::less<double> ]
template <class T1, class T2>
inline rootana::Cut Less(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::less<double> >(v1, v2)); return c_; }

/// "Equal-to" (==) [ uses std::equal_to<double> ]
template <class T1, class T2> 
inline rootana::Cut Equal(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::equal_to<double> >(v1, v2)); return c_; }

/// "Greater-than" (>) [ uses std::greater<double> ]
template <class T1, class T2> 
inline rootana::Cut Greater(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::greater<double> >(v1, v2)); return c_; }

/// "Not-equal" (!=) [ uses std::not_equal_to<double> ]
template <class T1, class T2> 
inline rootana::Cut NotEqual(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::not_equal_to<double> >(v1, v2)); return c_; }

/// "Less-than or equal-to" (<=) [ uses std::less_equal<double ]
template <class T1, class T2> 
inline rootana::Cut LessEqual(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::less_equal<double> >(v1, v2)); return c_; }

/// "Greater-than or equal-to" (>=) [ uses std::greater_equal<double ]
template <class T1, class T2> 
inline rootana::Cut GreaterEqual(const T1& v1, const T2& v2)
{ rootana::Cut c_(new rootana::Equivalency<T1, T2, std::greater_equal<double> >(v1, v2)); return c_; }

/// 2D cut (from TCutG)
template <class T1, class T2>
inline rootana::Cut Cut2D(const T1& xpar, const T2& ypar, TCutG& cutg)
{	rootana::Cut c_(new rootana::Condition2D<T1, T2>(xpar, ypar, cutg)); return c_; }

/// 2D cut (from manual point pair entries)
template <class T1, class T2>
inline rootana::Cut Cut2D(const T1& xpar, const T2& ypar, ROOTANA_CUT2D_POINT_ARGS)
{
	rootana::Cut c_(new rootana::Condition2D<T1, T2> (xpar, ypar, x0, y0, x1, y1, x2, y2, x3, y3, x4,
																										y4, x5, y5, x6, y6, x7, y7, x8, y8, x9, y9, x10,
																										y10, x11, y11, x12, y12, x13, y13));
	return c_;
}

/// Logical NOT (!)
inline rootana::Cut Not(const rootana::Cut& t1)
{ return t1.operator!(); }

/// Logical AND (&&)
inline rootana::Cut And(const rootana::Cut& t1, const rootana::Cut& t2)
{ return t1.operator&&(t2); }

/// Logical OR (||)
inline rootana::Cut Or(const rootana::Cut& t1, const rootana::Cut& t2)
{ return t1.operator||(t2); }



#endif
