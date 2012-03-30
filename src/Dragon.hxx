#ifndef DRAGON_HXX
#define DRAGON_HXX
#include "Bgo.hxx"

namespace dragon {

class Dragon {
public:
	 Bgo bgo;

public:
	 Dragon();
	 ~Dragon() { }
	 void reset();
	 void set(const char* odb_file);
	 void unpack(TMidasEvent& event);
};


} // namespace dragon


#endif
