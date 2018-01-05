///
/// \file TAtomicMass.cxx
/// \author G. Christian
/// \brief Implements the TAtomicMass class
///
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "TAtomicMass.h"


namespace {
	inline TAtomicMassTable::Nucleus_t make_nucleus_az(int Z, int A) {
		TAtomicMassTable::Nucleus_t nuc;
		nuc.fA = A;
		nuc.fZ = Z;
		nuc.fN = A-Z;
		sprintf(nuc.fSymbol, "XX");

		return nuc;
	}

	class MatchSymbol {
		std::string fSymbol;
	public:
		MatchSymbol(const char* symbol): fSymbol(symbol) {
			if(fSymbol == "n") fSymbol = "1n";
			if(fSymbol == "p") fSymbol = "1H";
			if(fSymbol == "d") fSymbol = "2H";
			if(fSymbol == "t") fSymbol = "3H";
			if(fSymbol == "a") fSymbol = "4He";
		}
		bool operator() (const std::pair<TAtomicMassTable::Nucleus_t,
						 TAtomicMassTable::MassExcess_t>& element)
		{
			const TAtomicMassTable::Nucleus_t& nuc = element.first;
			std::stringstream symbol;
			symbol << nuc.fA << nuc.fSymbol;
			return symbol.str() == fSymbol;
		}
	}; }


bool TAtomicMassTable::CompareNucleus_t::operator()
	(const Nucleus_t& lhs, const Nucleus_t& rhs) const
{
	if(lhs.fZ == rhs.fZ)
		return lhs.fA < rhs.fA;
	return lhs.fZ < rhs.fZ;
}

TAtomicMassTable::TAtomicMassTable(const char* file)
{
	SetFile(file);
}

TAtomicMassTable::TAtomicMassTable()
{
#ifdef AMEPP_DEFAULT_FILE
	SetFile(AMEPP_DEFAULT_FILE);
#endif
}

void TAtomicMassTable::SetFile(const char* filename)
{
	ParseFile(filename);
}

const TAtomicMassTable::Nucleus_t* TAtomicMassTable::GetNucleus(int Z, int A) const
{
	Nucleus_t nuc = make_nucleus_az(Z, A);
	Map_t::const_iterator it = fMassData.find(nuc);
	return it == fMassData.end() ? 0 : &(it->first);
}

const TAtomicMassTable::MassExcess_t* TAtomicMassTable::GetMassExcess(int Z, int A) const
{
	Nucleus_t nuc = make_nucleus_az(Z, A);
	Map_t::const_iterator it = fMassData.find(nuc);
	return it == fMassData.end() ? 0 : &(it->second);
}

const TAtomicMassTable::Nucleus_t* TAtomicMassTable::GetNucleus(const char* symbol) const
{
	Map_t::const_iterator it = std::find_if(fMassData.begin(), fMassData.end(), MatchSymbol(symbol));
	return it == fMassData.end() ? 0 : &(it->first);
}

const TAtomicMassTable::MassExcess_t* TAtomicMassTable::GetMassExcess(const char* symbol) const
{
	Map_t::const_iterator it = std::find_if(fMassData.begin(), fMassData.end(), MatchSymbol(symbol));
	return it == fMassData.end() ? 0 : &(it->second);
}


void TAtomicMassTable::SetMassExcess(int Z, int A, double value, double error, bool extrapolated)
{
	Nucleus_t nuc = make_nucleus_az(Z, A);
	Map_t::iterator it = fMassData.find(nuc);
	if(it != fMassData.end()) {
		it->second.fValue = value;
		it->second.fError = error;
		it->second.fExtrapolated = extrapolated;
	}
}

void TAtomicMassTable::ParseFile(const char* name)
{
	std::ifstream ffile(name);

	if(!ffile.good()) {
		if(name)
			std::cerr << "Error: couldn't find database file: \"" << name << "\"\n";
		else
			std::cerr << "Error parsing data!\n";
		return;
	}

	const int num_headers = 39;
	int linenum = 0;
	std::string line;
	while(std::getline(ffile, line)) {
		if(linenum++ < num_headers) // skip headers
			continue;
		Nucleus_t nuc;
		MassExcess_t me;
		if(ParseLine(line, &nuc, &me) == false) return;
		std::pair<Map_t::iterator, bool> insrt = fMassData.insert(std::make_pair(nuc, me));
		if(!insrt.second) {
			std::cerr << "Error: duplicate nucleus! A, N, Z = "
					  << nuc.fA << ", " << nuc.fZ << ", " << nuc.fN << " (old): "
					  << insrt.first->first.fA << ", " << insrt.first->first.fZ << ", " << insrt.first->first.fN << "\n";
			return;
		}
	}
}


bool TAtomicMassTable::ParseLine(const std::string& line, Nucleus_t* nuc, MassExcess_t* me) const
{
	size_t lengths[11] = { 1, 3, 5, 5, 5, 1, 3, 4, 1, 13, 11 };
	std::string fields[11];

	size_t sum = 0;
	for(int i=0; i< 11; ++i) {
		fields[i] = line.substr(sum, lengths[i]);
		sum += lengths[i];
		// std::cout << fields[i] << " | "; if(i==10) std::cout << "\n";
	}

	// N collumn 2
	nuc->fN = atoi(fields[2].c_str());
	// Z collumn 3
	nuc->fZ = atoi(fields[3].c_str());
	// A collumn 4
	nuc->fA = atoi(fields[4].c_str());
	if(nuc->fA != nuc->fZ + nuc->fN) {
		std::cerr << "Error: A != N+Z : A, N, Z = "
				  << nuc->fA << ", " << nuc->fZ << ", " << nuc->fN << "\n";
		return false;
	}

	// symbol collumn 6
	{
		if(fields[6].size() != 3) {
			std::cerr << "Error: fields[6].size != 3, == " << fields[6].size() << "\n";
			return false;
		}
		int pos = 0;
		for(size_t i=0; i< fields[6].size(); ++i) {
			if(fields[6][i] != ' ')
				nuc->fSymbol[pos++] = fields[6][i];
		}
		nuc->fSymbol[pos] = '\0';
	}

	// mass excess collumn 9
	{
		me->fExtrapolated = false;
		if(fields[9].find("#") < fields[9].size()) {
			me->fExtrapolated = true;
			fields[9] = fields[9].substr(0, fields[9].find("#"));
		}

		me->fValue = atof(fields[9].c_str());
	}

	// mass excess error collumn 10
	if(me->fExtrapolated)
		fields[10] = fields[10].substr(0, fields[10].find("#"));
	me->fError = atof(fields[10].c_str());

	return true;
}

double TAtomicMassTable::AtomicMassExcess(int Z, int A) const
{
	const MassExcess_t* pmass = GetMassExcess(Z, A);
	return pmass ? pmass->fValue : 0;
}

double TAtomicMassTable::AtomicMassExcessError(int Z, int A) const
{
	const MassExcess_t* pmass = GetMassExcess(Z, A);
	return pmass ? pmass->fError : 0;
}

double TAtomicMassTable::NuclearMass(int Z, int A) const
{
	const MassExcess_t* pmass = GetMassExcess(Z, A);
	if(!pmass) return 0;

	return pmass->fValue + A*AMU() - Z*ElectronMass();
}

double TAtomicMassTable::NuclearMassError(int Z, int A) const
{
	return AtomicMassExcessError(Z, A);
}

double TAtomicMassTable::AtomicMassExcess(const char* symbol) const
{
	const MassExcess_t* pmass = GetMassExcess(symbol);
	return pmass ? pmass->fValue : 0;
}

double TAtomicMassTable::AtomicMassExcessError(const char* symbol) const
{
	const MassExcess_t* pmass = GetMassExcess(symbol);
	return pmass ? pmass->fError : 0;
}

double TAtomicMassTable::NuclearMass(const char* symbol) const
{
	const MassExcess_t* pmass = GetMassExcess(symbol);
	if(!pmass) return 0;

	const Nucleus_t* nuc = GetNucleus(symbol);
	if(!nuc) return 0;

	return pmass->fValue + nuc->fA*AMU() - nuc->fZ*ElectronMass();
}

double TAtomicMassTable::NuclearMassError(const char* symbol) const
{
	return AtomicMassExcessError(symbol);
}

double TAtomicMassTable::IonMass(const char* symbol, int chargeState) const
{
	const Nucleus_t* nuc = GetNucleus(symbol);
	if(!nuc) return 0;

	return NuclearMass(symbol) + ElectronMass()*(nuc->fZ - chargeState);
}

double TAtomicMassTable::QValue(int Zt, int At, int Zb, int Ab, int Ze, int Ae, bool print) const
{
	int Ztot = Zt + Zb;
	int Atot = At + Ab;
	int Zr = Ztot - Ze;
	int Ar = Atot - Ae;

	double qval = (NuclearMass(Zb, Ab) + NuclearMass(Zt, At)) - (NuclearMass(Zr, Ar) + NuclearMass(Ze, Ae));

	if(print) {
		const Nucleus_t* recoil   = GetNucleus(Zr, Ar);
		const Nucleus_t* ejectile = GetNucleus(Ze, Ae);
		const Nucleus_t* beam     = GetNucleus(Zb, Ab);
		const Nucleus_t* target   = GetNucleus(Zt, At);
		if(!recoil) {
			std::cerr << "Invalid recoil!\n";
			return 0;
		}
		if(!ejectile) {
			if(Ze == 0 && Ae == 0){
				std::cout << "\tQ value for "
						  <<        target->fA   << target->fSymbol
						  << "(" << beam->fA     << beam->fSymbol
						  << "," << "\u03B3"
						  << ")" << recoil->fA   << recoil->fSymbol
						  << ": " << qval << " keV.\n\n";
				return qval;
			}
			else{
				std::cerr << "Invalid ejectile!\n";
				return 0;
			}
		}
		if(!beam) {
			std::cerr << "Invalid beam!\n";
			return 0;
		}
		if(!target) {
			std::cerr << "Invalid target!\n";
			return 0;
		}

		std::cout << "\tQ value for "
				  <<        target->fA   << target->fSymbol
				  << "(" << beam->fA     << beam->fSymbol
				  << "," << ejectile->fA << ejectile->fSymbol
				  << ")" << recoil->fA   << recoil->fSymbol
				  << ": " << qval << " keV.\n\n";
	}

	return qval;
}


double TAtomicMassTable::QValue(const char* beam, const char* target, const char* ejectile, bool print) const
{
	std::string gamma("g");
	std::string ej(ejectile);

	const Nucleus_t* b = GetNucleus(beam);
	if(!b) {
		std::cerr << "Invalid beam!\n";
		return 0;
	}
	const Nucleus_t* t = GetNucleus(target);
	if(!t) {
		std::cerr << "Invalid target!\n";
		return 0;
	}
	if(ej == gamma){
		std::cout << "Radiative Capture reaction!\n";
		return QValue(b->fZ, b->fA, t->fZ, t->fA, 0, 0, print);
	}
	const Nucleus_t* e = GetNucleus(ejectile);
	if(!e) {
		std::cerr << "Invalid ejectile!\n";
		return 0;
	}

	return QValue(b->fZ, b->fA, t->fZ, t->fA, e->fZ, e->fA, print);
}





//
// Global instance
TAtomicMassTable* gAtomicMassTable = new TAtomicMassTable();
