#ifndef DRAGON_HXX
#define DRAGON_HXX
#include "Modules.hxx"
#include "HeavyIon.hxx"
#include "Bgo.hxx"

/// Encloses all DRAGON classes
namespace dragon {

/// A complete Dragon event
class Dragon {
private:
// ===== Electronics Modules ====== //
	 /// Gamma ray + heavy ion modules
	 dragon::Modules modules; //#
	 
public:
// ===== Detector data ===== //
   /// Bgo detector
	 gamma::Gamma gamma;

	 /// Heavy ion detectors
	 hion::HeavyIon hi;


// ===== Methods ===== //
	 /// Constructor, initialize data
	 Dragon();

   /// Destructor, nothing to do
	 ~Dragon() { }

	 /// Reset all modules
	 void reset();

	 /// Set variable values from an odb file
	 void set_variables(const char* odb_file);

	 /// Unpack midas event data
	 void unpack(TMidasEvent& event);

   /// Calibrate data from a midas event
	 void calibrate();
};


} // namespace dragon


#endif
