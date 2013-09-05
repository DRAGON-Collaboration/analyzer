#include <string>
#include <algorithm>
#include "TAtomicMass.h"


namespace {
inline Nucleus_t make_nucleus_az(int Z, int A) {
	Nucleus_t nuc; nuc.fA = A; nuc.fZ = Z;
	return nuc;
} }


TAtomicMassTable::TAtomicMassTable(const char* file):
	fFile(0)
{
	SetFile(file);
}

TAtomicMassTable::TAtomicMassTable():
	fFile(0)
{
#ifdef DRAGON_AME12_FILENAME
	SetFile(DRAGON_AME12_FILENAME);
#endif
}

void TAtomicMassTable::SetFile(const char* filename)
{
	fFile.reset(new std::ifstream(filename));
	if(fFile->good())
		ParseFile(filename);
	else {
		ParseFile(filename);
		fFile.reset(0);
	}
}

const Nucleus_t* TAtomicMassTable::GetNucleus(int Z, int A) const
{
	Nucleus_t nuc = make_nucleus_az(Z, A);
	Map_t::const_iterator it = fMassData.find(nuc);
	return it == fMassData.end() ? 0 : &(it->first);
}

const MassExcess_t* TAtomicMassTable::GetMassExcess(int Z, int A) const
{
	Nucleus_t nuc = make_nucleus_az(Z, A);
	Map_t::const_iterator it = fMassData.find(nuc);
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
	if(!fFile->good()) {
		if(name)
			std::cerr << "Error: couldn't find database file: \"" << name << "\"\n";
		else
			std::cerr << "Error parsing data!\n";
		return;
	}

	const int num_headers = 39;
	int linenum = 0;
	std::string line;
	while(std::getline(*fFile, line)) {
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
