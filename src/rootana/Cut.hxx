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


