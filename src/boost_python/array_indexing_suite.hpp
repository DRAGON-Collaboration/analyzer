//  (C) Copyright Joel de Guzman 2003.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ARRAY_INDEXING_SUITE_JDG20036_HPP
# define ARRAY_INDEXING_SUITE_JDG20036_HPP

# include <boost/python/suite/indexing/indexing_suite.hpp>
# include <boost/python/suite/indexing/container_utils.hpp>
# include <boost/python/iterator.hpp>

#include <iostream>

#define ARR_NOT_AVAILABLE(name)																					\
	do { std::cout << "Method: " << name << " isn't availble for a static array\n"; } while(false)

namespace boost { namespace python { namespace test {
            
// Forward declaration
template <class Type, size_t Length, class Container, bool NoProxy, class DerivedPolicies>
class array_indexing_suite;
    
namespace detail
{
template <class Type, size_t Length, class Container, bool NoProxy>
class final_array_derived_policies 
	: public array_indexing_suite<Type, Length, Container, 
																NoProxy, final_array_derived_policies<Type, Length, Container, NoProxy> > {};
}

// The array_indexing_suite class is a predefined indexing_suite derived 
// class for wrapping std::array (and std::array like) classes. It provides
// all the policies required by the indexing_suite (see indexing_suite).
// Example usage:
//
//  class X {...};
//
//  ...
//
//      class_<std::array<X> >("XVec")
//          .def(array_indexing_suite<std::array<X> >())
//      ;
//
// By default indexed elements are returned by proxy. This can be
// disabled by supplying *true* in the NoProxy template parameter.
//
template <
	class Type,
	size_t Length,
	class Container = Type[Length],
	bool NoProxy = false,
	class DerivedPolicies 
	= detail::final_array_derived_policies<Type, Length, Container, NoProxy> >
class array_indexing_suite 
	: public indexing_suite<Container, DerivedPolicies, NoProxy>
{
public:

	typedef size_t index_type;
	typedef Type   key_type;
	typedef Type   data_type;
	typedef Type*  object;
    
	template <class Class>
	static void 
	extension_def(Class& cl)
		{
			cl
				.def("append", &base_append)
				.def("extend", &base_extend)
				;
		}
        
	static 
	typename mpl::if_<
		is_class<data_type>
		, data_type&
		, data_type
		>::type
	get_item(Container& container, index_type i)
		{ 
			return container[i];
		}

	static object 
	get_slice(Container& container, index_type from, index_type to)
		{ 
			return container + from;
		}

	static void 
	set_item(Container& container, index_type i, data_type const& v)
		{ 
			container[i] = v;
		}

	static void 
	set_slice(Container& container, index_type from, 
						index_type to, data_type const& v)
		{ 
			if (from > to) {
				return;
			}
			else {
				for(index_type i=from; i< to; ++i)
					container[i] = v;
			}
		}

	template <class Iter>
	static void 
	set_slice(Container& container, index_type from, 
						index_type to, Iter first, Iter last)
		{ 
			if (from > to) {
				container.insert(container.begin()+from, first, last);
			}
			else {
				container.erase(container.begin()+from, container.begin()+to);
				container.insert(container.begin()+from, first, last);
			}
		}

	static void 
	delete_item(Container& container, index_type i)
		{ 
			ARR_NOT_AVAILABLE("delete_item");
		}
        
	static void 
	delete_slice(Container& container, index_type from, index_type to)
		{
			ARR_NOT_AVAILABLE("delete_item");
		}
        
	static size_t
	size(Container& container)
		{
			return Length;
		}
        
	static bool
	contains(Container& container, key_type const& key)
		{
			return std::find(container, container + Length, key)
				!= (container + Length);
		}
        
	static index_type
	get_min_index(Container& /*container*/)
		{ 
			return 0;
		}

	static index_type
	get_max_index(Container& container)
		{ 
			return Length;
		}
      
	static bool 
	compare_index(Container& /*container*/, index_type a, index_type b)
		{
			return a < b;
		}
        
	static index_type
	convert_index(Container& container, PyObject* i_)
		{ 
			ARR_NOT_AVAILABLE("convert_index");
			return 0;
		}
      
	static void 
	append(Container& container, data_type const& v)
		{ 
			ARR_NOT_AVAILABLE("append");
		}
        
	template <class Iter>
	static void 
	extend(Container& container, Iter first, Iter last)
		{ 
			ARR_NOT_AVAILABLE("extend");
		}
        
private:
    
	static void
	base_append(Container& container, object v)
		{
			ARR_NOT_AVAILABLE("append");
		}
        
	static void
	base_extend(Container& container, object v)
		{
			ARR_NOT_AVAILABLE("extend");
		}
};
       
}}} // namespace boost::python 

#endif // ARRAY_INDEXING_SUITE_JDG20036_HPP
