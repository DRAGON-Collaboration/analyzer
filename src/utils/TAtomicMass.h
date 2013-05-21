#include <map>
#include <memory>
#include <fstream>
#include <iostream>

struct Nucleus_t {
	char fSymbol[3];
	int fA;
	int fZ;
	int fN;
};

struct MassExcess_t {
	double fValue; // keV
	double fError; // keV
	bool fExtrapolated;
};

struct CompareNucleus_t {
	bool operator() (const Nucleus_t& lhs, const Nucleus_t& rhs) const
		{ 
			if(lhs.fZ == rhs.fZ)
				return lhs.fA < rhs.fA;
			return lhs.fZ < rhs.fZ;
		}
};

class TAtomicMassTable {
public:
	typedef std::map<Nucleus_t, MassExcess_t, CompareNucleus_t> Map_t;

public:
	TAtomicMassTable();
	TAtomicMassTable(const char* file);
	const Nucleus_t* GetNucleus(int Z, int A) const;
	const MassExcess_t* GetMassExcess(int Z, int A) const;
	void SetMassExcess(int Z, int A, double value, double error, bool extrapolated = false);

	double AtomicMassExcess(int Z, int A) const;
	double AtomicMassExcessError(int Z, int A) const;

	double NuclearMass(int Z, int A) const;
	double NuclearMassError(int Z, int A) const;

	double NuclearMassAMU(int Z, int A) const { return NuclearMass(Z, A) / AMU(); }
	double NuclearMassErrorAMU(int Z, int A) const { return NuclearMassError(Z, A) / AMU(); }

	double IonMass(int Z, int A, int chargeState) const { return NuclearMass(Z, A) + ElectronMass()*(Z-chargeState); }
	double IonMassError(int Z, int A, int charge) const { return NuclearMassError(Z, A); }

	double IonMassAMU(int Z, int A, int chargeState) const { return IonMass(Z, A, chargeState) / AMU(); }
	double IonMassErrorAMU(int Z, int A, int chargeState) const { return NuclearMassErrorAMU(Z, A); }

	static double AMU() { return 931494.061; }
	static double ElectronMass() { return 0.510998910; }

private:
	void ParseFile(const char* = 0);
	bool ParseLine(const std::string&, Nucleus_t*, MassExcess_t*) const;

private:
	Map_t fMassData;
	std::auto_ptr<std::istream> fFile;
};
