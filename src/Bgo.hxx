#ifndef DRAGON_BGO_HXX
#define DRAGON_BGO_HXX
#include "vme/vme.hxx"

namespace dragon {

class Bgo {
private:
	 vme::caen::V792 qdc; //!

public:
	 class Variables {
	 public:
			int ch[30];
			double slope [30];
			double offset[30];
			Variables();
			~Variables() { }
			void set(const char* odb_file);
	 } variables; //!
	 
	 int32_t evt_count; //#
	 int16_t qraw[30];  //#
	 int16_t traw[30];  //#
	 double  qcal[30];  //#

			
public:
	 Bgo();
	 ~Bgo() { }
	 void reset();
	 void unpack(TMidasEvent& event);
};
	 

} // namespace dragon


#endif
