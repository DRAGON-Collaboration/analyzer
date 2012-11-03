#ifndef BOOST_ARRAY_INDEXING_HPP
#define BOOST_ARRAY_INDEXING_HPP
#include <cstdio>
#include <algorithm>

namespace array_indexing {

template <typename T>
inline std::vector<T> make_vector(T* array, size_t length)
{
	std::vector<T> out(length);
	std::copy(array, array+length, out.begin());
	return out;
}

template <typename T, size_t Length>
inline std::vector<T> make_vector()
{
	std::vector<T> out(Length);
	return out;
}


template <class T>
struct wrapper {
	wrapper(): data(0), length(0) { }
	wrapper(T* data_, size_t length_): data(data_), length(length_) { }
	T* data;
	size_t length;
};

} // namespace array_indexing



namespace boost {

namespace python {

template <class T, bool NoProxy = false>
class array_indexing_suite: public vector_indexing_suite<std::vector<T>, NoProxy> {
public:
	array_indexing_suite():
		boost::python::vector_indexing_suite<std::vector<T>, NoProxy> ()
		{  }
	array_indexing_suite(T* array, size_t length):
		boost::python::vector_indexing_suite<std::vector<T>, NoProxy> (array_indexing::make_vector(array, length))
		{  }
};

template <class Parent, class Type, size_t Length, bool NoProxy = false>
class array_indexing_suite2: public vector_indexing_suite<std::vector<Type>, NoProxy> {
public:
	array_indexing_suite2():
		boost::python::vector_indexing_suite<std::vector<Type>, NoProxy> (array_indexing::make_vector<Type, Length>())
		{	}
};

} // namespace python

} // namespace boost


#endif
