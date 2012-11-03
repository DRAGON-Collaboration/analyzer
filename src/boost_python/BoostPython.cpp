/// \file BoostPython.cpp
/// \author G. Christian
/// \brief Creates python wrappers for dragon classes using boost::python.
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python.hpp>

#include <algorithm>

#include "midas/Database.hxx"
#include "utils/Functions.hxx"
#include "utils/Error.hxx"
#include "vme/V1190.hxx"
#include "vme/V792.hxx"
#include "dragon/Channels.h"
#include "dragon/Bgo.hxx"

#include "ArrayIndexing.hpp"

#include "array_indexing.hpp"
#include "array_indexing_suite.hpp"

#include <numpy/arrayobject.h>

#include <iostream>


boost::python::test::array_indexing_suite<double, 30> test123;

struct Test2 {
	double d[30];
	boost::python::object py_get_data() {
    npy_intp shape[1] = { 30 };
    npy_intp strides[1] = { 1 };
    boost::python::handle<> array(
			PyArray_New(&PyArray_Type, 1, shape, NPY_FLOAT, strides,
									d, sizeof(double), NPY_CARRAY, NULL)
			);
    return boost::python::object(array);
	}
};

template <class C, class T, size_t N, T (C::*ptr)[N]>
class ArrayMember {
public:
  ArrayMember(C& r): rTest(r) { }
  void print(int index)
    { std::cout << (rTest.*ptr)[index] << std::endl; }
private:
  C& rTest;
};

class MyVector: public std::vector<double> {
public:
	MyVector(double* array, size_t length):
		std::vector<double>(length)
		{
			std::copy(array, array+length, this->begin());
		}
};

class Test {
public:
	double arr[30];
	std::vector<double> v;
	Test(): v(30) { arr[2] = 32; }
};

void export_vector()
{
	boost::python::class_<std::vector<double> >("DoubleVec")
		.def(boost::python::vector_indexing_suite<std::vector<double> >())
		.def(boost::python::init<size_t>())
		.def("resize", &std::vector<double>::resize)
		;

	boost::python::class_<ArrayMember<Test, double, 30, &Test::arr> >("DoubleArr",
																																		boost::python::init<Test&>())
		.def("Print", &ArrayMember<Test, double, 30, &Test::arr>::print)
		;

}



void export_test()
{
	// boost::python::class_<double (Test::*)[30]>("TestArray")
	// 	.def(boost::python::array_indexing_suite2<Test, double, 30>())
	// 	;
	boost::python::class_<Test>("Test")
		.def_readwrite("v", &Test::v)
		// .def_readwrite("arr", array_index::wrapper<double>(&Test::arr, 30))
		;
}

namespace dragon {

const int BGO_MAX_CHANNELS = dragon::Bgo::MAX_CHANNELS;

void export_Bgo()
{
  boost::python::class_<Bgo>("Bgo")
		.def("reset", &dragon::Bgo::reset)
		.def("read_data", &dragon::Bgo::read_data)
		.def("calculate", &dragon::Bgo::calculate)
		.def_readonly("MAX_CHANNELS", &dragon::BGO_MAX_CHANNELS)
		.def_readwrite("sum", &dragon::Bgo::sum)
		.def_readwrite("x0", &dragon::Bgo::x0)
		.def_readwrite("y0", &dragon::Bgo::y0)
		.def_readwrite("z0", &dragon::Bgo::z0)
		.def_readwrite("t0", &dragon::Bgo::t0)
    ;
}

} // namespace dragon


BOOST_PYTHON_MODULE(dragon)
{
	dragon::export_Bgo();
	export_vector();
	export_test();
}
