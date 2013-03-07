/*!
 * \file DataPointer.hxx
 * \author G. Christian
 * \brief Defines a class wrapping the addresses of basic data types.
 */
#ifndef ROOTANA_DATA_POINTER_HXX
#define ROOTANA_DATA_POINTER_HXX
#include <cassert>

namespace rootana {

/// Abstract interface for generic pointer-to-data classes
/*!
 * This interface is inherited by template classes which implement
 * the pure virtual get() and length() methods for a given basic data type.
 *
 * The class also provides static template "creation" functions, allowing the
 * appropriate derived class to be constructed without explicitly specifying the
 * data type.
 */
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
	double get(unsigned index = 0) const;
	/// Returns zero
	unsigned length() const { return 0; }
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

inline double rootana::DataPointerNull::get(unsigned) const
{
	assert ( 0 && "rootana::DataPointerNull::get(): May not call this function!" );
}

#endif
