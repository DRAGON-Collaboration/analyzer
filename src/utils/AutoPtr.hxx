///
/// \file AutoPtr.hxx
/// \brief Simple smart pointer class
/// \details I would just use auto_ptr, but for earlier versions
/// of ROOT it causes problems to have a persistent auto_ptr member
///
#ifndef DRAGON_UTILS_AUTO_PTR_HXX
#define DRAGON_UTILS_AUTO_PTR_HXX
#include <cassert>

namespace dragon { namespace utils {

/// Simple smart pointer class
template <class T>
class AutoPtr {
public:
	AutoPtr(): fObj(0)     { }
	AutoPtr(T* t): fObj(t) { }
	~AutoPtr() { reset(0); }
	void reset(T* obj)
		{
			if(fObj) delete fObj;
			fObj = obj;
		}
	T* get() const { return fObj; }
	T* operator->() const { return get(); }
	T& operator*()  const { assert(fObj); return *fObj; }

private:
	AutoPtr(const AutoPtr& other): fObj(new T(*other)) { }
	AutoPtr& operator= (const AutoPtr& other) { reset(new T(*other)); return *this; }

private:
	T* fObj;
};

} }


#endif
