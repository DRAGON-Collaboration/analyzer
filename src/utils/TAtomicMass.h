///
/// \file TAtomicMass.h
/// \author G. Christian
/// \brief Defines the TAtomicMass class
///
#ifndef T_ATOMIC_MASS_H
#define T_ATOMIC_MASS_H
#include <map>
#include <memory>
#include <fstream>
#include <iostream>
#ifdef USE_ROOT
#include <TObject.h>
#endif


/// Class to extract and calculate atomic mass information from an AME file
class TAtomicMassTable
#ifdef USE_ROOT
	: public TObject
#endif
{
public:
  /// Contains information on a nucleus
	struct Nucleus_t {
		/// Symbol, e.g. 3He
		char fSymbol[3];
		/// Mass number
		int fA;
		/// Proton number
		int fZ;
		/// Neutron number
		int fN;
	};

  /// Contains mass excess information from AME file
	struct MassExcess_t {
		/// Nominal value of the mass excess (in keV)
		double fValue; // keV
		/// Error of the mass excess (in keV)
		double fError; // keV
		/// True if the AME entry is an extrapolation, false if not
		bool fExtrapolated;
	};

	/// Functor for map comparison
	struct CompareNucleus_t {
		bool operator() (const Nucleus_t& lhs, const Nucleus_t& rhs) const;
	};	

#ifndef DOXYGEN_SKIP
	
private:

	typedef std::map<Nucleus_t, MassExcess_t, CompareNucleus_t> Map_t;

#endif // DOXYGEN_SKIP

public:
	/// Empty constructor
	TAtomicMassTable();
	/// Construct and open AME file
	TAtomicMassTable(const char* file);
	/// Opens a new AME file
	void SetFile(const char* filename);

	/// Returns struct containing nucleas information w/ Z, A
	const Nucleus_t* GetNucleus(int Z, int A) const;
	/// Returns struct containing nucleas information from its symbol, i.e. "11Li"
	const Nucleus_t* GetNucleus(const char* symbol) const;

	/// Returns mass excess information for nucleus w/ Z, A
	const MassExcess_t* GetMassExcess(int Z, int A) const;
	/// Returns mass excess information for nucleus from its symbol, i.e. "11Li"
	const MassExcess_t* GetMassExcess(const char* symbol) const;

	/// Manually sets the mass excess for a given nucleus
	void SetMassExcess(int Z, int A, double value, double error, bool extrapolated = false);

	/// Returns the _atomic_ mass excess value for a given nucleus, in keV/c^2
	double AtomicMassExcess(int Z, int A) const; //keV/c^2
	/// Returns the _atomic_ mass excess error for a given nucleus, in keV/c^2
	double AtomicMassExcessError(int Z, int A) const; // keV/c^2
	/// Returns the _atomic_ mass excess value for a given nucleus, in keV/c^2
	double AtomicMassExcess(const char* symbol) const; //keV/c^2
	/// Returns the _atomic_ mass excess error for a given nucleus, in keV/c^2
	double AtomicMassExcessError(const char* symbol) const; // keV/c^2

	/// Returns the _nuclear_ (fully ionized) mass value for a given nucleus, in keV/c^2
	double NuclearMass(int Z, int A) const; // keV/c^2
	/// Returns the _nuclear_ (fully ionized) mass error for a given nucleus, in keV/c^2
	double NuclearMassError(int Z, int A) const; // keV/c^2
	/// Returns the _nuclear_ (fully ionized) mass value for a given nucleus, in keV/c^2
	double NuclearMass(const char* symbol) const; // keV/c^2
	/// Returns the _nuclear_ (fully ionized) mass error for a given nucleus, in keV/c^2
	double NuclearMassError(const char* symbol) const; // keV/c^2

	/// Returns the _nuclear_ (fully ionized) mass value for a given nucleus, in AMU
	double NuclearMassAMU(int Z, int A) const { return NuclearMass(Z, A) / AMU(); } // AMU
	/// Returns the _nuclear_ (fully ionized) mass error for a given nucleus, in AMU
	double NuclearMassErrorAMU(int Z, int A) const { return NuclearMassError(Z, A) / AMU(); } //AMU
	/// Returns the _nuclear_ (fully ionized) mass value for a given nucleus, in AMU
	double NuclearMassAMU(const char* symbol) const { return NuclearMass(symbol) / AMU(); } // AMU
	/// Returns the _nuclear_ (fully ionized) mass error for a given nucleus, in AMU
	double NuclearMassErrorAMU(const char* symbol) const { return NuclearMassError(symbol) / AMU(); } //AMU

	/// Returns the ion (partially ionized) mass value for a given nucleus, in keV/c^2
	double IonMass(int Z, int A, int chargeState) const { return NuclearMass(Z, A) + ElectronMass()*(Z-chargeState); } // keV/c^2
	/// Returns the ion (partially ionized) mass error for a given nucleus, in keV/c^2
	double IonMassError(int Z, int A, int charge) const { return NuclearMassError(Z, A); } // keV/c^2
	/// Returns the ion (partially ionized) mass value for a given nucleus, in keV/c^2
	double IonMass(const char* symbol, int chargeState) const; // keV/c^2
	/// Returns the ion (partially ionized) mass error for a given nucleus, in keV/c^2
	double IonMassError(const char* symbol, int charge) const { return NuclearMassError(symbol); } // keV/c^2

	/// Returns the ion (partially ionized) mass value for a given nucleus, in AMU
	double IonMassAMU(int Z, int A, int chargeState) const { return IonMass(Z, A, chargeState) / AMU(); } // AMU
	/// Returns the ion (partially ionized) mass error for a given nucleus, in AMU
	double IonMassErrorAMU(int Z, int A, int chargeState) const { return NuclearMassErrorAMU(Z, A); } // AMU
	/// Returns the ion (partially ionized) mass value for a given nucleus, in AMU
	double IonMassAMU(const char* symbol, int chargeState) const { return IonMass(symbol, chargeState) / AMU(); } // AMU
	/// Returns the ion (partially ionized) mass error for a given nucleus, in AMU
	double IonMassErrorAMU(const char* symbol, int chargeState) const { return NuclearMassErrorAMU(symbol); } // AMU

	/// Calculate Q-value for a nuclear reaction
	double QValue(int Zt, int At, int Zb, int Ab, int Ze, int Ae, bool print = true) const;
	/// Calculate Q-value for a nuclear reaction (using symbols)
	double QValue(const char* beam, const char* target, const char* ejectile, bool print = true) const;


	/// Returns the AMU -> keV/c^2 conversion
	static double AMU() { return 931494.061; } // AMU->keV/c^2
	/// Returns the electron mass in keV/c^2
	static double ElectronMass() { return 510.998910; } // keV/c^2

private:
	void ParseFile(const char* = 0);
	bool ParseLine(const std::string&, Nucleus_t*, MassExcess_t*) const;

private:
	Map_t fMassData;

#ifdef USE_ROOT
	ClassDef(TAtomicMassTable, 1);
#endif
};


/// Global instance
extern TAtomicMassTable* gAtomicMassTable;

#endif

/* Local Variables:  */
/* mode: c++         */
/* End:              */
