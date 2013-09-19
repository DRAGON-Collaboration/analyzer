#ifndef T_ATOMIC_MASS_H
#define T_ATOMIC_MASS_H
#include <map>
#include <memory>
#include <fstream>
#include <iostream>

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

/// Helper for comparing Nucleus_t objects
struct CompareNucleus_t {
	bool operator() (const Nucleus_t& lhs, const Nucleus_t& rhs) const
		{ 
			if(lhs.fZ == rhs.fZ)
				return lhs.fA < rhs.fA;
			return lhs.fZ < rhs.fZ;
		}
};

/// Class to extract and calculate atomic mass information from an AME file
class TAtomicMassTable {
public:
	typedef std::map<Nucleus_t, MassExcess_t, CompareNucleus_t> Map_t;

public:
	/// Empty constructor
	TAtomicMassTable();
	/// Construct and open AME file
	TAtomicMassTable(const char* file);
	/// Opens a new AME file
	void SetFile(const char* filename);

	/// Returns struct containing nucleas information w/ Z, A
	const Nucleus_t* GetNucleus(int Z, int A) const;
	/// Returns mass excess information for nucleus w/ Z, A
	const MassExcess_t* GetMassExcess(int Z, int A) const;
	/// Manually sets the mass excess for a given nucleus
	void SetMassExcess(int Z, int A, double value, double error, bool extrapolated = false);

	/// Returns the _atomic_ mass excess value for a given nucleus, in keV/c^2
	double AtomicMassExcess(int Z, int A) const;
	/// Returns the _atomic_ mass excess error for a given nucleus, in keV/c^2
	double AtomicMassExcessError(int Z, int A) const;

	/// Returns the _nuclear_ (fully ionized) mass value for a given nucleus, in keV/c^2
	double NuclearMass(int Z, int A) const;
	/// Returns the _nuclear_ (fully ionized) mass error for a given nucleus, in keV/c^2
	double NuclearMassError(int Z, int A) const;

	/// Returns te _nuclear_ (fully ionized) mass value for a given nucleus, in AMU
	double NuclearMassAMU(int Z, int A) const { return NuclearMass(Z, A) / AMU(); }
	/// Returns te _nuclear_ (fully ionized) mass error for a given nucleus, in AMU
	double NuclearMassErrorAMU(int Z, int A) const { return NuclearMassError(Z, A) / AMU(); }

	/// Returns te ion (partially ionized) mass value for a given nucleus, in keV/c^2
	double IonMass(int Z, int A, int chargeState) const { return NuclearMass(Z, A) + ElectronMass()*(Z-chargeState); }
	/// Returns te ion (partially ionized) mass error for a given nucleus, in keV/c^2
	double IonMassError(int Z, int A, int charge) const { return NuclearMassError(Z, A); }

	/// Returns te ion (partially ionized) mass value for a given nucleus, in AMU
	double IonMassAMU(int Z, int A, int chargeState) const { return IonMass(Z, A, chargeState) / AMU(); }
	/// Returns te ion (partially ionized) mass error for a given nucleus, in AMU
	double IonMassErrorAMU(int Z, int A, int chargeState) const { return NuclearMassErrorAMU(Z, A); }

	/// Returns the AMU -> keV/c^2 conversion
	static double AMU() { return 931494.061; }
	/// Returns the electron mass in keV/c^2
	static double ElectronMass() { return 510.998910; }

private:
	void ParseFile(const char* = 0);
	bool ParseLine(const std::string&, Nucleus_t*, MassExcess_t*) const;

private:
	Map_t fMassData;
	std::auto_ptr<std::istream> fFile;
};

#endif
