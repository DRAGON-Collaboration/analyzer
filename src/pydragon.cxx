///
/// \file pydragon.cxx
/// \author G. Christian
/// \brief Unpack into c structs for porting to python.
///
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "midas/libMidasInterface/TMidasStructs.h"
#include "midas/libMidasInterface/TMidasFile.h"
#include "midas/Database.hxx"
#include "utils/definitions.h"
#include "Vme.hxx"
#include "Unpack.hxx"
#include "Dragon.hxx"
#include "Sonik.hxx"
#include "pydragon.hxx"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
namespace py = pybind11;


PYBIND11_MODULE(pydragon, m) {
	//
	// DOCSTRING
	m.doc() = "pybind11 DRAGON analyzer plugin"; // optional module docstring

	//
	// FUNCTIONS
	m.def("setup",      &setup,      "set up analyzer, open MIDAS file");
	m.def("read_event", &read_event, "read a single event from MIDAS file");
	m.def("flush_queue",&flush_queue,"flush a single event from the timestamp matching queue");
	m.def("close_file", &close_file, "close MIDAS file");

	// 
	// CONSTANTS
	m.attr("DRAGON_HEAD_EVENT")         = py::int_(DRAGON_HEAD_EVENT);
	m.attr("DRAGON_HEAD_SCALER")        = py::int_(DRAGON_HEAD_SCALER);
	m.attr("DRAGON_TAIL_EVENT")         = py::int_(DRAGON_TAIL_EVENT);
	m.attr("DRAGON_TAIL_SCALER")        = py::int_(DRAGON_TAIL_SCALER);
	m.attr("DRAGON_COINC_EVENT")        = py::int_(DRAGON_COINC_EVENT);
	m.attr("DRAGON_TSTAMP_DIAGNOSTICS") = py::int_(DRAGON_TSTAMP_DIAGNOSTICS);
	m.attr("DRAGON_RUN_PARAMETERS")     = py::int_(DRAGON_RUN_PARAMETERS);
	m.attr("DRAGON_AUX_SCALER")         = py::int_(DRAGON_AUX_SCALER);
	m.attr("DRAGON_EPICS_EVENT")        = py::int_(DRAGON_EPICS_EVENT);
	m.attr("DRAGON_EPICS_SCALER")       = py::int_(DRAGON_EPICS_SCALER);
	m.attr("DRAGON_SCALER_READ_PERIOD") = py::int_(DRAGON_SCALER_READ_PERIOD);
	m.attr("DRAGON_TSC_FREQ")           = py::float_(DRAGON_TSC_FREQ);
	m.attr("FE_HEAD")                   = py::int_(FE_HEAD);
	m.attr("FE_TAIL")                   = py::int_(FE_TAIL);
	m.attr("MIDAS_BOR")                 = py::int_(MIDAS_BOR);
	m.attr("MIDAS_EOR")                 = py::int_(MIDAS_EOR);
	m.attr("READ_EOF")                  = py::int_(READ_EOF);
	
	//
	// HEADER CLASSES
	py::class_<TMidas_EVENT_HEADER>(m, "Header")
		.def(py::init<>())
		.def_readwrite("fEventId",&TMidas_EVENT_HEADER::fEventId)
		.def_readwrite("fTriggerMask",&TMidas_EVENT_HEADER::fTriggerMask)
		.def_readwrite("fSerialNumber",&TMidas_EVENT_HEADER::fSerialNumber)
		.def_readwrite("fTimeStamp",&TMidas_EVENT_HEADER::fTimeStamp)
		.def_readwrite("fDataSize",&TMidas_EVENT_HEADER::fDataSize)
		; // class TMidas_EVENT_HEADER

	py::class_<vme::Io32::Tsc4>(m,"Tsc4")
		.def(py::init<>())
		.def_readwrite("trig_time",&vme::Io32::Tsc4::trig_time)
		.def_property_readonly("n_fifo", [](py::object& obj){
			vme::Io32::Tsc4& o = obj.cast<vme::Io32::Tsc4&>();
			return py::array{4, o.n_fifo, obj};
		})
		; // class vme::Io32::Tsc4
	
	py::class_<vme::Io32>(m, "Io32")
		.def(py::init<>())
		.def_readwrite("header",&vme::Io32::header)
		.def_readwrite("trig_count",&vme::Io32::trig_count)
		.def_readwrite("tstamp",&vme::Io32::tstamp)
		.def_readwrite("start",&vme::Io32::start)
		.def_readwrite("end",&vme::Io32::end)
		.def_readwrite("latency",&vme::Io32::latency)
		.def_readwrite("read_time",&vme::Io32::read_time)
		.def_readwrite("busy_time",&vme::Io32::busy_time)
		.def_readwrite("trigger_latch",&vme::Io32::trigger_latch)
		.def_readwrite("which_trigger",&vme::Io32::which_trigger)
		.def_readwrite("tsc4",&vme::Io32::tsc4)
		; // class vme::Io32

	//
	// HEAD CLASSES
	
	//Bgo
	py::class_<dragon::Bgo>(m, "Bgo")
		.def(py::init<>())
//		.def_readonly_static("MAX_CHANNELS", &30)
		.def_property_readonly("ecal", [](py::object& obj){
			dragon::Bgo& o = obj.cast<dragon::Bgo&>();
			return py::array{30, o.ecal, obj};
		})
		.def_property_readonly("tcal", [](py::object& obj){
			dragon::Bgo& o = obj.cast<dragon::Bgo&>();
			return py::array{30, o.tcal, obj};
		})
		.def_property_readonly("esort", [](py::object& obj){
			dragon::Bgo& o = obj.cast<dragon::Bgo&>();
			return py::array{30, o.esort, obj};
		})
		.def_readwrite("sum", &dragon::Bgo::sum)
		.def_readwrite("hit0", &dragon::Bgo::hit0)
		.def_readwrite("x0", &dragon::Bgo::x0)
		.def_readwrite("y0", &dragon::Bgo::y0)
		.def_readwrite("z0", &dragon::Bgo::z0)
		.def_readwrite("t0", &dragon::Bgo::t0)
		;

	// Head
	py::class_<dragon::Head>(m, "Head")
		.def(py::init<>())
//		.def_readonly_static("MAX_RF_HITS", &dragon::Head::MAX_RF_HITS)
		// .def_readwrite("eshort0", &dragon::Head::eshort0)
		// .def_readwrite("psd0", &dragon::Head::psd0)
		// .def_property_readonly("short_gate", [](py::object& obj){
		// 	dragon::Head& o = obj.cast<dragon::Head&>();
		// 	return py::array{32, o.short_gate, obj};
		// })
		// .def_property_readonly("eshort", [](py::object& obj){
		// 	dragon::Head& o = obj.cast<dragon::Head&>();
		// 	return py::array{30, o.eshort, obj};
		// })
		// .def_property_readonly("psd", [](py::object& obj){
		// 	dragon::Head& o = obj.cast<dragon::Head&>();
		// 	return py::array{30, o.psd, obj};
		// })
		.def_readwrite("tcal0", &dragon::Head::tcal0)
		.def_readwrite("tcalx", &dragon::Head::tcalx)
		.def_readwrite("tcal_rf", &dragon::Head::tcal_rf)
		// .def_property_readonly("rftof", [](py::object& obj){
		// 	dragon::Head& o = obj.cast<dragon::Head&>();
		// 	return py::array{5, o.rftof, obj};
		// })
		.def_readwrite("bgo",&dragon::Head::bgo)
		.def_readwrite("header",&dragon::Head::header)
		.def_readwrite("io32",&dragon::Head::io32)
		.def_readwrite("trf",&dragon::Head::trf)
		; // class dragon::Head

		
		//
		// TAIL CLASSES
		// Dsssd
	py::class_<dragon::Dsssd>(m, "Dsssd")
		.def(py::init<>())
		.def_readwrite("efront",&dragon::Dsssd::efront)
		.def_readwrite("eback",&dragon::Dsssd::eback)
		.def_readwrite("hit_front",&dragon::Dsssd::hit_front)
		.def_readwrite("hit_back",&dragon::Dsssd::hit_back)
		.def_readwrite("tfront",&dragon::Dsssd::tfront)
		.def_readwrite("tback",&dragon::Dsssd::tback)
		.def_property_readonly("ecal", [](py::object& obj){
			dragon::Dsssd& o = obj.cast<dragon::Dsssd&>();
			return py::array{32, o.ecal, obj};
		})
		; // class dragon::Dsssd

	py::class_<dragon::IonChamber>(m, "IonChamber")
		.def(py::init<>())
		.def_readwrite("sum",&dragon::IonChamber::sum)
		.def_property_readonly("anode", [](py::object& obj){
			dragon::IonChamber& o = obj.cast<dragon::IonChamber&>();
			return py::array{5, o.anode, obj};
		})
		.def_property_readonly("tcal", [](py::object& obj){
			dragon::IonChamber& o = obj.cast<dragon::IonChamber&>();
			return py::array{4, o.tcal, obj};
		})
		; // class dragon::IonChamber

	py::class_<dragon::NaI>(m, "NaI")
		.def(py::init<>())
		.def_property_readonly("ecal", [](py::object& obj){
			dragon::NaI& o = obj.cast<dragon::NaI&>();
			return py::array{4, o.ecal, obj};
		})
		; // class dragon::NaI

	py::class_<dragon::Ge>(m, "Ge")
		.def(py::init<>())		
		.def_readwrite("ecal",&dragon::Ge::ecal)
		; // class dragon::Ge

	py::class_<dragon::Mcp>(m, "Mcp")
		.def(py::init<>())
		.def_property_readonly("anode", [](py::object& obj){
			dragon::Mcp& o = obj.cast<dragon::Mcp&>();
			return py::array{4, o.anode, obj};
		})
		.def_property_readonly("tcal", [](py::object& obj){
			dragon::Mcp& o = obj.cast<dragon::Mcp&>();
			return py::array{4, o.tcal, obj};
		})
		.def_readwrite("esum",&dragon::Mcp::esum)
		.def_readwrite("tac",&dragon::Mcp::tac)
		.def_readwrite("x",&dragon::Mcp::x)
		.def_readwrite("y",&dragon::Mcp::y)			
		; // class dragon::Mcp

	py::class_<dragon::SurfaceBarrier>(m, "SurfaceBarrier")
		.def(py::init<>())
		.def_property_readonly("ecal", [](py::object& obj){
			dragon::SurfaceBarrier& o = obj.cast<dragon::SurfaceBarrier&>();
			return py::array{2, o.ecal, obj};
		})
		; // class dragon::SurfaceBarrier

	py::class_<dragon::HiTof>(m, "HiTof")
		.def(py::init<>())
		.def_readwrite("mcp",&dragon::HiTof::mcp)
		.def_readwrite("mcp_dsssd",&dragon::HiTof::mcp_dsssd)
		.def_readwrite("mcp_ic",&dragon::HiTof::mcp_ic)
		; // class dragon::HiTof
		
	py::class_<dragon::TdcChannel<5> >(m, "TdcChannel")
		.def(py::init<>())
		.def_property_readonly("leading", [](py::object& obj){
			dragon::TdcChannel<5>& o = obj.cast<dragon::TdcChannel<5>& >();
			return py::array{5, o.leading, obj};
		})
		.def_property_readonly("trailing", [](py::object& obj){
			dragon::TdcChannel<5>& o = obj.cast<dragon::TdcChannel<5>& >();
			return py::array{5, o.trailing, obj};
		})
		; // class dragon::TdcChannel

	// Tail
	py::class_<dragon::Tail>(m, "Tail")
		.def(py::init<>())
		.def_readwrite("dsssd",&dragon::Tail::dsssd)
		.def_readwrite("ic",&dragon::Tail::ic)
		.def_readwrite("nai",&dragon::Tail::nai)
		.def_readwrite("ge",&dragon::Tail::ge)
		.def_readwrite("mcp",&dragon::Tail::mcp)
		.def_readwrite("sb",&dragon::Tail::sb)
		.def_readwrite("tof",&dragon::Tail::tof)
		.def_readwrite("trf",&dragon::Tail::trf)
		.def_readwrite("tcal_rf",&dragon::Tail::tcal_rf)
		.def_readwrite("tcal0",&dragon::Tail::tcal0)
		.def_readwrite("tcalx",&dragon::Tail::tcalx)
		.def_readwrite("header",&dragon::Tail::header)
		.def_readwrite("io32",&dragon::Tail::io32)
		; // class dragon::Tail

	//
	// COINCIDENCE
	py::class_<dragon::Coinc>(m,"Coinc")
		.def(py::init<>())
		.def_readwrite("head",&dragon::Coinc::head)
		.def_readwrite("tail",&dragon::Coinc::tail)
		.def_readwrite("xtrig",&dragon::Coinc::xtrig)
		.def_readwrite("xtofh",&dragon::Coinc::xtofh)
		.def_readwrite("xtoft",&dragon::Coinc::xtoft)
		; // class dragon::Coinc
	
	//
	// SCALERS
	py::class_<dragon::Scaler>(m,"Scaler")
		.def(py::init<>())
		.def_property_readonly("count", [](py::object& obj){
			dragon::Scaler& o = obj.cast<dragon::Scaler& >();
			return py::array{17, o.count, obj};
		})
		.def_property_readonly("sum", [](py::object& obj){
			dragon::Scaler& o = obj.cast<dragon::Scaler& >();
			return py::array{17, o.sum, obj};
		})
		.def_property_readonly("rate", [](py::object& obj){
			dragon::Scaler& o = obj.cast<dragon::Scaler& >();
			return py::array{17, o.rate, obj};
		})		
		; // class dragon::Scaler

	//
	// EPICS
	py::class_<dragon::Epics>(m,"Epics")
		.def(py::init<>())
		.def_readwrite("header",&dragon::Epics::header)
		.def_readwrite("ch",&dragon::Epics::ch)
		.def_readwrite("val",&dragon::Epics::val)
		; // class dragon::Epics

	//
	// TIMESTAMP DIAGNOSTICS
	py::class_<tstamp::Diagnostics>(m,"TimestampDiagnostics")
		.def(py::init<>())
		.def_readwrite("fTime0",&tstamp::Diagnostics::fTime0)
		.def_readwrite("size",&tstamp::Diagnostics::size)
		.def_readwrite("n_coinc",&tstamp::Diagnostics::n_coinc)
		.def_readwrite("coinc_rate",&tstamp::Diagnostics::coinc_rate)
		.def_readwrite("time_diff",&tstamp::Diagnostics::time_diff)
		.def_property_readonly("n_singles", [](py::object& obj){
			tstamp::Diagnostics& o = obj.cast<tstamp::Diagnostics& >();
			return py::array{10, o.n_singles, obj};
		})		
		.def_property_readonly("singles_rate", [](py::object& obj){
			tstamp::Diagnostics& o = obj.cast<tstamp::Diagnostics& >();
			return py::array{10, o.singles_rate, obj};
		})
		; // class tstamp::Diagnostics
}

namespace {
bool arg_return = false;
const char* const msg_use =
	"usage: mid2root <input file> [-o <output file>] [-v <xml odb>] [-histos <*.xml> ] "
	"[--singles] [--overwrite] [--quiet <n>] [--help]\n";

typedef std::vector<std::string> strvector_t;
typedef int32_t Int_t;
typedef bool Bool_t;
}

//
/// Encloses mid2root helper functions
namespace m2r {

//
// Skip documenting a bunch of internal stuff

//
// Error handling stuff
std::ostream cnul (0);

class strm_ref {
public:
	strm_ref(std::ostream& strm):
		pstrm(&strm) {}
	strm_ref():
		pstrm(&cnul) {}
	strm_ref& operator= (std::ostream& strm)
    { pstrm = &strm; return *this; }
	template <class T>
	std::ostream& operator<< (const T& t)
    { return *pstrm << t; }
	std::ostream& flush()
    { return std::flush(*pstrm); }
private:
	std::ostream* pstrm;
};

inline std::ostream& flush(strm_ref& strm)
{ return strm.flush(); }

strm_ref cout = std::cout;
strm_ref cwar = std::cerr;
strm_ref cerr = std::cerr;

void SetQuietLevel(int level)
{
	if (level < 1) {
		gErrorIgnoreLevel = 0;
		m2r::cout = std::cout;
		m2r::cwar = std::cerr;
		m2r::cerr = std::cerr;
	}
	else if (level == 1) {
		gErrorIgnoreLevel = 1001;
		m2r::cout = m2r::cnul;
		m2r::cwar = std::cerr;
		m2r::cerr = std::cerr;
	}
	else if (level == 2) {
		gErrorIgnoreLevel = 2001;
		m2r::cout = m2r::cnul;
		m2r::cwar = m2r::cnul;
		m2r::cerr = std::cerr;
	}
	else {
		gErrorIgnoreLevel = level*1000 + 1;
		m2r::cout = m2r::cnul;
		m2r::cwar = m2r::cnul;
		m2r::cerr = m2r::cnul;
	}
}

int GetQuietLevel()
{
	for (int i = 3; i > 0; --i) {
		if (gErrorIgnoreLevel - i*1000 > 0)
			return i;
	}
	return 0;
}

/// Program options
struct Options_t {
	std::string fIn;
	std::string fOut;
	std::string fOdb;
	std::string fHistos;
	bool fOverwrite;
	bool fSingles;
	bool fSonik;
	Options_t(): fOverwrite(false), fSingles(false), fSonik(false) {}
};


/// Print in-place event counter
void static_counter(Int_t n, Int_t nupdate = 1000, bool force = false)
{
	if(n == 0) {
		m2r::cout << "Events converted: ";
		m2r::flush(m2r::cout);
	}
	if(n % nupdate != 0 && !force) return;

	static int nOut = 0;
	std::stringstream out; out << n;
	if(nOut) {
		for(int i=0; i< nOut; ++i)
			m2r::cout << "\b";
	}
	nOut = out.str().size();
	m2r::cout << n;
	m2r::flush(m2r::cout);
}

/// Print a usage message
int usage(const char* what = 0)
{
	const char* msg =
		"Run 'mid2root --help' for more information.\n";
	m2r::cerr << msg_use << msg << std::endl;
	if (what) m2r::cerr << "Error: " << what << ".\n";
	arg_return = true;
	return 1;
}

/// Print a help message
int help()
{
	m2r::cerr
		<< "\nmid2root: A program to convert DRAGON experiment data from MIDAS format into ROOT Trees.\n" << msg_use;

	const char* msg_args =
		"\n"
		"Program arguments:\n"
		"\n"
		"\t<input file>:     Specifies the MIDAS file to convert [required].\n"
		"\n"
		"\t-o <output file>: Specify the output file. If not set, the output file\n"
		"\t                  will have the same name as the input file, but with the extension\n"
		"\t                  converted to \'.root\'. If the environment variable $DH is set, and\n"
		"\t                  the directory $DH/rootfiles exists, the default output is written to\n"
		"\t                  that directory. Otherwise, it is written to the present working directory.\n"
		"\n"
		"\t-v <xml odb>:     Specify an XML file containing the full '/dragon' ODB tree, which defines all\n"
		"\t                  variables to be used in the program. Default is to take variable values from the\n"
		"\t                  ODB dump of the input MIDAS file (i.e. variables will reflect the state of the ODB\n"
		"\t                  when the run was taken).\n"
		"\n"
		"\t-histos <*.xml>:  Specify an XML file denoting histograms to fill and save during the unpacking.\n"
		"\t                  The XML file format should be the same as those created by ROOTBEER. If the DRAGON\n"
		"\t                  package was compiled with USE_ROOTBEER turned off, then this option is not available.\n"
		"\t                  In case it is specified but not available, the program will terminate with an error message.\n"
		"\n"
		"\t--sonik:          Unpack in \"SONIK\" mode. Treat tail data as if coming from the SONIK scattering detectors,\n"
		"\t                  rather than from the DRAGON end detectors.\n"
		"\n"
		"\t--singles:        Unpack in singles mode. This means that every head and tail event is analyzed as a singles\n"
		"\t                  event only. In this mode, the buffering in a queue and timestamp matching routines are\n"
		"\t                  skipped completely.\n"
		"\n"
		"\t--overwrite:      Overwrite any existing output files without asking the user.\n"
		"\n"
		"\t--quiet <n>:      Suppress program output messages. Followed by a numeral specifying the level of\n"
		"\t                  quietness: 1 suppresses only informational messages, 2 supresses information and\n"
		"\t                  warnings, and >=3 suppresses all output (including errors). The default setting is 0,\n"
		"\t                  which prints all messages.\n"
		"\n"
		"\t--help:           Print this help message and exit.\n"
		;
	m2r::cerr << msg_args << std::endl;
	arg_return = true;
	return 0;
}


/// Parse command line arguments
int process_args(const std::vector<std::string>& argv, Options_t* options)
{
	auto argc = argv.size();
	strvector_t args((&argv[0])+1, (&argv[0])+argc);
	strvector_t::iterator iarg = args.begin();

	//
	// Look for input file
	for(; iarg != args.end(); ++iarg) {
		if(iarg->substr(0, 2) == "--")
			continue;
		if((iarg-1 >= args.begin()) && *(iarg-1) == "--quiet")
			continue;
		options->fIn = *iarg;
		break;
	}

	//
	// Check other arguments
	for(iarg = args.begin(); iarg != args.end(); ++iarg) {
		if (*iarg == options->fIn) { // Input file, already set
			continue;
		}
		else if (*iarg == "--help") { // Help message
			return help();
		}
		// else if (*iarg == "-o") { // Output file
		// 	if (++iarg == args.end()) return usage("output file not specified");
		// 	options->fOut = *iarg;
		// }
		else if (*iarg == "-v") { // Variables file
			if (++iarg == args.end()) return usage("variables file not specified");
			options->fOdb = *iarg;
		}
		// else if (*iarg == "-histos") { // Histograms file

		// 	if (++iarg == args.end()) return usage("histogram xml file not specified");
		// 	options->fHistos = *iarg;
		// }
		else if (*iarg == "--singles") { // Singles mode
			options->fSingles = true;
		}
		// else if (*iarg == "--sonik") { // SONIK mode
		// 	options->fSonik = true;
		// }
		// else if (*iarg == "--overwrite") { // Overwrite flag
		// 	options->fOverwrite = true;
		// }
		else if (*iarg == "--quiet") { // Quiet flag
			if (++iarg == args.end()) return usage("quietness level not specified");
			m2r::SetQuietLevel(atoi(iarg->c_str()));
		}
		else { // Unknown flag
			std::string what = "unknown flag \'";
			what += *iarg; what += "\'";
			usage(what.c_str());
			return 1;
		}
	}

	if (options->fIn.empty()) // Didn't find input file
		return usage("no input file specified");

	return 0;
}

} // namespace m2r

namespace {
// Local variables for unpacking
m2r::Options_t options;
std::unique_ptr<TMidasFile> fin(nullptr);

const int nIds = 9;

dragon::Head head;
dragon::Tail tail;
dragon::Coinc coinc;
dragon::Epics epics;
dragon::Scaler head_scaler;
dragon::Scaler tail_scaler;
dragon::Scaler aux_scaler;
dragon::RunParameters runpar;
tstamp::Diagnostics tsdiag;
Sonik sonik;

const int eventIds[nIds] = {
	DRAGON_HEAD_EVENT,
	DRAGON_HEAD_SCALER,
	DRAGON_TAIL_EVENT,
	DRAGON_TAIL_SCALER,
	DRAGON_COINC_EVENT,
	DRAGON_AUX_SCALER,
	DRAGON_EPICS_EVENT,
	DRAGON_TSTAMP_DIAGNOSTICS,
	DRAGON_RUN_PARAMETERS
};
const std::string eventTitles[nIds] = {
	"Head singles event.",
	"Head scaler event.",
	"Tail singles event.",
	"Tail scaler event.",
	"Coincidence event.",
	"Aux scaler event.",
	"Epics event.",
	"Timestamp diagnostics.",
	"Global run parameters."
};
void* addr[nIds] = {
	&head,
	&head_scaler,
	&tail,
	&tail_scaler,
	&coinc,
	&aux_scaler,
	&epics,
	&tsdiag,
	&runpar
};
void *psonik = &sonik;

std::unique_ptr<dragon::Unpacker> unpack(nullptr);
// std::unique_ptr<midas::Database> db0(nullptr); // run start
// std::unique_ptr<midas::Database> db1(nullptr); // run stop

}


int setup(py::list args)
{
	auto argv = args.cast<std::vector<std::string>>();
	int arg_result = m2r::process_args(argv, &options);
	if(arg_return) {
		std::cerr << "Error processing options!\n";
		return arg_result;
	}

	//
	// Open input file
	fin.reset(new TMidasFile);
	if (fin->Open(options.fIn.c_str()) == false) {
		m2r::cerr
			<< "Error: Couldn't open the file \'" << options.fIn
			<< "\': \"" << fin->GetLastError() << ".\"\n\n";
		return 1;
	}
	//
	// Handle odb variables file
	if (options.fOdb.empty()) {
		options.fOdb = options.fIn; // No file specified, use ODB dump in midas file
	}
	else { // Check if it exists
		{
			std::ifstream dummy(options.fOdb.c_str());
			if(!dummy.good()) {
				m2r::cerr
				<< "Error: The specified variables file \'" << options.fOdb
				<< "\' does not exist.\n\n";
			return 1;
			}
		}
	}

	unpack.reset(
		new dragon::Unpacker(
			&head, &tail, &coinc, &epics,
			&head_scaler, &tail_scaler, &aux_scaler,
			&runpar, &tsdiag, options.fSingles
			) );

	//
	// Set coincidence variables
	if(!options.fSingles) {
		bool coincSuccess;
		double coincWindow = 10, queueTime = 4;
		{
			midas::Database db (options.fOdb.c_str());
			coincSuccess = db.ReadValue("/dragon/coinc/variables/window", coincWindow);
			if (coincSuccess)
				coincSuccess = db.ReadValue("/dragon/coinc/variables/buffer_time", queueTime);
		}
		if (coincSuccess) {
			unpack->SetCoincWindow(coincWindow);
			unpack->SetQueueTime(queueTime);
		}
		m2r::cout
			<< "\nUnpacker parameters: coincidence window = " << unpack->GetCoincWindow() << " usec., "
			<< "queue time = " << unpack->GetQueueTime() << " sec.\n\n";
	}
	else {
		m2r::cout << "\nRunning in singles mode.\n\n";
	}

	//
	// Begin-of-run initialization
	unpack->HandleBor(options.fOdb.c_str());
	return 0;
};

namespace { void sort_event(
	const std::vector<Int_t>& which,
	py::list& output_codes,
	py::list& output_data)
{
	for(const auto& id : which){
		output_codes.append(id);
		switch(id) {
		case DRAGON_HEAD_EVENT:
			output_data.append(head);
			break;
		case DRAGON_HEAD_SCALER:
			output_data.append(head_scaler);
			break;
		case DRAGON_TAIL_EVENT:
			output_data.append(tail);
			break;
		case DRAGON_TAIL_SCALER:
			output_data.append(tail_scaler);
			break;
		case DRAGON_COINC_EVENT:
			output_data.append(coinc);
			break;
		case DRAGON_AUX_SCALER:
			output_data.append(aux_scaler);
			break;
		case DRAGON_EPICS_EVENT:
			output_data.append(epics);
			break;
		case DRAGON_TSTAMP_DIAGNOSTICS:
			output_data.append(tsdiag);
			break;
		case DRAGON_RUN_PARAMETERS:
//				output_data.append(runpar);
			output_data.append(pybind11::none());
			break;				
		default:
			break;
		}
	}
} }

//int read_event()
py::list read_event()
{
	py::list output;
	py::list output_codes;
	py::list output_data;
	
	//
	// Read event from MIDAS file
	TMidasEvent temp;
	bool success = fin->Read(&temp);
	if (!success) {
		output_codes.append(READ_EOF);
	}
	else if (temp.GetEventId() == MIDAS_BOR) {
		//
		// Read ODB tree if MIDAS_EOR buffer
		
		//db0.reset(new midas::Database(temp.GetData(), temp.GetDataSize()));
		output_codes.append(MIDAS_BOR);
		output_data.append(temp.GetData());
	}
	else if (temp.GetEventId() == MIDAS_EOR) {
		//db1.reset(new midas::Database(temp.GetData(), temp.GetDataSize()));
		output_codes.append(MIDAS_EOR);
		output_data.append(temp.GetData());
	}
	else {
		//
		// Unpack into our classes
		std::vector<Int_t> which = unpack->UnpackMidasEvent(
			temp.GetEventHeader(),
			temp.GetData()
			);
		sort_event(which,output_codes,output_data);
	}

	output.append(output_codes);
	output.append(output_data);
	return output;
	
	
// 	//
// 	// Check which classes have data, fill trees for those that do
// 	// Also fill histograms if appropriate
// 	for (int i=0; i< nIds; ++i) {
// 		std::vector<Int_t>::iterator it =
// 			std::find(which.begin(), which.end(), eventIds[i]);
// 		if(it != which.end()) {
// 			// if(trees[i]) {
// 			// 	trees[i]->Fill();
// 			// }
			

			
// 			if(options.fSonik && eventIds[i] == DRAGON_TAIL_EVENT) {
// 				sonik.reset();
// 				sonik.read_data(tail.v785, tail.v1190);
// 				sonik.calculate();
// 				// t0->Fill();
// 			}
// 		}
// 	}
// //	return READ_EVENT;
// 	output.append(READ_EVENT);
// 	return output;
}

py::list flush_queue()
{
	py::list output;
	py::list output_codes;
	py::list output_data;

	if(options.fSingles) {
		output_codes.append(READ_EOF);
	}
	else {
		size_t qsize = unpack->FlushQueueIterative(); // Fills classes implicitly
		if (qsize == 0) {
			output_codes.append(READ_EOF);
		}
		else {
			std::vector<Int_t> which = unpack->GetUnpackedCodes();
			sort_event(which,output_codes,output_data);
		}
	}
	
	output.append(output_codes);
	output.append(output_data);
	
	// // Explicitly fill TTrees
	// std::vector<Int_t> which = unpack->GetUnpackedCodes();
	// for (int i=0; i< nIds; ++i) {
	// 	std::vector<Int_t>::iterator it =
	// 		std::find(which.begin(), which.end(), eventIds[i]);
	// 	if(it != which.end()) {
	// 		// if(trees[i]) {
	// 		// 	trees[i]->Fill();
	// 		// }
	// 		if(options.fSonik && eventIds[i] == DRAGON_TAIL_EVENT) {
	// 			sonik.reset();
	// 			sonik.read_data(tail.v785, tail.v1190);
	// 			sonik.calculate();
	// 			// t0->Fill();
	// 		}
	// 	}
	// }

	
	return output;
}

void close_file()
{
	fin->Close();
	unpack.reset(nullptr);
	fin.reset(nullptr);
}
