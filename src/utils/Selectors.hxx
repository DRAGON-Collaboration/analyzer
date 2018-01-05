///
/// \file Selectors.hxx
/// \brief Defines generic and specific TSelector classes for use with
/// DRAGON data.
///
#ifndef DRAGON_ROOT_SELECTORS_HXX
#define DRAGON_ROOT_SELECTORS_HXX
#include <TCut.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TSelector.h>

#include "midas/libMidasInterface/TMidasStructs.h"
#include "Uncertainty.hxx"
#include "Constants.hxx"
#include "Dragon.hxx"
#include "Vme.hxx"


namespace midas { class Database; }


namespace dragon {

//! Generic selector class
class ASelector : public TSelector {
public :
	TTree          *fChain;   //! <pointer to the analyzed TTree or TChain
public:
	ASelector(TTree * /*tree*/ =0) : fChain(0) { }
	virtual ~ASelector() { if(fChain) fChain->ResetBranchAddresses(); }
	virtual Int_t   Version() const { return 2; }
	virtual void    Begin(TTree *tree);
	virtual void    SlaveBegin(TTree *) { }
	virtual void    Init(TTree *tree);
	virtual Bool_t  Notify();
	virtual Bool_t  Process(Long64_t entry);
	virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
	virtual void    SetOption(const char *option) { fOption = option; }
	virtual void    SetObject(TObject *obj) { fObject = obj; }
	virtual void    SetInputList(TList *input) { fInput = input; }
	virtual TList  *GetOutputList() const { return fOutput; }
	virtual void    SlaveTerminate() { }
	virtual void    Terminate();

	ClassDef(ASelector,0);
};


//! Selector class for head singles events.

/// The intended use of this class is for users to derive from it and then
/// implement the following methods:
///
///    - Begin():        called every time a loop on the tree starts,
///                      a convenient place to create your histograms.
///    - Process():      called for each event, in this function you decide what
///                      to read and fill your histograms.
///    - Terminate():    called at the end of the loop on the tree,
///                      a convenient place to draw/fit your histograms.
///
/// Optionally, derived classes can also implement Notify() and Init().
///
/// For an example on how to use this class, see the file examples/Selectors.cxx
///
class HeadSelector : public ASelector {
public :
	// Declaration of leaf types
	//dragon::Head    *gamma;
	UShort_t        header_fEventId;
	UShort_t        header_fTriggerMask;
	UInt_t          header_fSerialNumber;
	UInt_t          header_fTimeStamp;
	UInt_t          header_fDataSize;
	UInt_t          io32_header;
	UInt_t          io32_trig_count;
	UInt_t          io32_tstamp;
	UInt_t          io32_start;
	UInt_t          io32_end;
	UInt_t          io32_latency;
	UInt_t          io32_read_time;
	UInt_t          io32_busy_time;
	UInt_t          io32_trigger_latch;
	UInt_t          io32_which_trigger;
	Int_t           io32_tsc4_n_fifo[4];
	Double_t        io32_tsc4_trig_time;
	Short_t         v792_n_ch;
	Int_t           v792_count;
	Bool_t          v792_overflow;
	Bool_t          v792_underflow;
	Short_t         v792_data[32];
	Short_t         v1190_n_ch;
	Int_t           v1190_count;
	Short_t         v1190_word_count;
	Short_t         v1190_trailer_word_count;
	Short_t         v1190_event_id;
	Short_t         v1190_bunch_id;
	Short_t         v1190_status;
	Short_t         v1190_type;
	Int_t           v1190_extended_trigger;
	std::vector<unsigned int> v1190_fifo0_measurement;
	std::vector<unsigned short> v1190_fifo0_channel;
	std::vector<unsigned short> v1190_fifo0_number;
	std::vector<unsigned int> v1190_fifo1_measurement;
	std::vector<unsigned short> v1190_fifo1_channel;
	std::vector<unsigned short> v1190_fifo1_number;
	Double_t        bgo_ecal[30];
	Double_t        bgo_tcal[30];
	Double_t        bgo_esort[30];
	Double_t        bgo_sum;
	Int_t           bgo_hit0;
	Double_t        bgo_x0;
	Double_t        bgo_y0;
	Double_t        bgo_z0;
	Double_t        bgo_t0;
	Double_t        tcal0;
	Double_t        tcalx;
	Double_t        tcal_rf;

	// List of branches
	TBranch        *b_gamma_header_fEventId;   //!
	TBranch        *b_gamma_header_fTriggerMask;   //!
	TBranch        *b_gamma_header_fSerialNumber;   //!
	TBranch        *b_gamma_header_fTimeStamp;   //!
	TBranch        *b_gamma_header_fDataSize;   //!
	TBranch        *b_gamma_io32_header;   //!
	TBranch        *b_gamma_io32_trig_count;   //!
	TBranch        *b_gamma_io32_tstamp;   //!
	TBranch        *b_gamma_io32_start;   //!
	TBranch        *b_gamma_io32_end;   //!
	TBranch        *b_gamma_io32_latency;   //!
	TBranch        *b_gamma_io32_read_time;   //!
	TBranch        *b_gamma_io32_busy_time;   //!
	TBranch        *b_gamma_io32_trigger_latch;   //!
	TBranch        *b_gamma_io32_which_trigger;   //!
	TBranch        *b_gamma_io32_tsc4_n_fifo;   //!
	TBranch        *b_gamma_io32_tsc4_trig_time;   //!
	TBranch        *b_gamma_v792_n_ch;   //!
	TBranch        *b_gamma_v792_count;   //!
	TBranch        *b_gamma_v792_overflow;   //!
	TBranch        *b_gamma_v792_underflow;   //!
	TBranch        *b_gamma_v792_data;   //!
	TBranch        *b_gamma_v1190_n_ch;   //!
	TBranch        *b_gamma_v1190_count;   //!
	TBranch        *b_gamma_v1190_word_count;   //!
	TBranch        *b_gamma_v1190_trailer_word_count;   //!
	TBranch        *b_gamma_v1190_event_id;   //!
	TBranch        *b_gamma_v1190_bunch_id;   //!
	TBranch        *b_gamma_v1190_status;   //!
	TBranch        *b_gamma_v1190_type;   //!
	TBranch        *b_gamma_v1190_extended_trigger;   //!
	TBranch        *b_gamma_v1190_fifo0_measurement;   //!
	TBranch        *b_gamma_v1190_fifo0_channel;   //!
	TBranch        *b_gamma_v1190_fifo0_number;   //!
	TBranch        *b_gamma_v1190_fifo1_measurement;   //!
	TBranch        *b_gamma_v1190_fifo1_channel;   //!
	TBranch        *b_gamma_v1190_fifo1_number;   //!
	TBranch        *b_gamma_bgo_ecal;   //!
	TBranch        *b_gamma_bgo_tcal;   //!
	TBranch        *b_gamma_bgo_esort;   //!
	TBranch        *b_gamma_bgo_sum;   //!
	TBranch        *b_gamma_bgo_hit0;   //!
	TBranch        *b_gamma_bgo_x0;   //!
	TBranch        *b_gamma_bgo_y0;   //!
	TBranch        *b_gamma_bgo_z0;   //!
	TBranch        *b_gamma_bgo_t0;   //!
	TBranch        *b_gamma_tcal0;   //!
	TBranch        *b_gamma_tcalx;   //!
	TBranch        *b_gamma_tcal_rf;   //!

	HeadSelector(TTree * /*tree*/ =0) : ASelector(0) { }
	virtual ~HeadSelector() { }
	virtual void    Init(TTree *tree);

	ClassDef(HeadSelector,0);
};


//! Selector class for tail singles events.

/// The intended use of this class is for users to derive from it and then
/// implement the following methods:
///
///    - Begin():        called every time a loop on the tree starts,
///                      a convenient place to create your histograms.
///    - Process():      called for each event, in this function you decide what
///                      to read and fill your histograms.
///    - Terminate():    called at the end of the loop on the tree,
///                      a convenient place to draw/fit your histograms.
///
/// Optionally, derived classes can also implement Notify() and Init().
///
/// For an example on how to use this class, see the file examples/Selectors.cxx
///
class TailSelector : public ASelector {
public :
	// Declaration of leaf types
	//dragon::Tail    *hi;
	UShort_t        header_fEventId;
	UShort_t        header_fTriggerMask;
	UInt_t          header_fSerialNumber;
	UInt_t          header_fTimeStamp;
	UInt_t          header_fDataSize;
	UInt_t          io32_header;
	UInt_t          io32_trig_count;
	UInt_t          io32_tstamp;
	UInt_t          io32_start;
	UInt_t          io32_end;
	UInt_t          io32_latency;
	UInt_t          io32_read_time;
	UInt_t          io32_busy_time;
	UInt_t          io32_trigger_latch;
	UInt_t          io32_which_trigger;
	Int_t           io32_tsc4_n_fifo[4];
	Double_t        io32_tsc4_trig_time;
	vme::V792       v785[2];
	Short_t         v1190_n_ch;
	Int_t           v1190_count;
	Short_t         v1190_word_count;
	Short_t         v1190_trailer_word_count;
	Short_t         v1190_event_id;
	Short_t         v1190_bunch_id;
	Short_t         v1190_status;
	Short_t         v1190_type;
	Int_t           v1190_extended_trigger;
	std::vector<unsigned int> v1190_fifo0_measurement;
	std::vector<unsigned short> v1190_fifo0_channel;
	std::vector<unsigned short> v1190_fifo0_number;
	std::vector<unsigned int> v1190_fifo1_measurement;
	std::vector<unsigned short> v1190_fifo1_channel;
	std::vector<unsigned short> v1190_fifo1_number;
	Double_t        dsssd_ecal[32];
	Double_t        dsssd_efront;
	Double_t        dsssd_eback;
	UInt_t          dsssd_hit_front;
	UInt_t          dsssd_hit_back;
	Double_t        dsssd_tfront;
	Double_t        dsssd_tback;
	Double_t        ic_anode[5];
	Double_t        ic_tcal[4];
	Double_t        ic_sum;
	Double_t        nai_ecal[2];
	Double_t        ge_ecal;
	Double_t        mcp_anode[4];
	Double_t        mcp_tcal[2];
	Double_t        mcp_esum;
	Double_t        mcp_tac;
	Double_t        mcp_x;
	Double_t        mcp_y;
	Double_t        sb_ecal[2];
	Double_t        tof_mcp;
	Double_t        tof_mcp_dsssd;
	Double_t        tof_mcp_ic;
	Double_t        tcal_rf;
	Double_t        tcal0;
	Double_t        tcalx;

	// List of branches
	TBranch        *b_hi_header_fEventId;   //!
	TBranch        *b_hi_header_fTriggerMask;   //!
	TBranch        *b_hi_header_fSerialNumber;   //!
	TBranch        *b_hi_header_fTimeStamp;   //!
	TBranch        *b_hi_header_fDataSize;   //!
	TBranch        *b_hi_io32_header;   //!
	TBranch        *b_hi_io32_trig_count;   //!
	TBranch        *b_hi_io32_tstamp;   //!
	TBranch        *b_hi_io32_start;   //!
	TBranch        *b_hi_io32_end;   //!
	TBranch        *b_hi_io32_latency;   //!
	TBranch        *b_hi_io32_read_time;   //!
	TBranch        *b_hi_io32_busy_time;   //!
	TBranch        *b_hi_io32_trigger_latch;   //!
	TBranch        *b_hi_io32_which_trigger;   //!
	TBranch        *b_hi_io32_tsc4_n_fifo;   //!
	TBranch        *b_hi_io32_tsc4_trig_time;   //!
	TBranch        *b_hi_v785;   //!
	TBranch        *b_hi_v1190_n_ch;   //!
	TBranch        *b_hi_v1190_count;   //!
	TBranch        *b_hi_v1190_word_count;   //!
	TBranch        *b_hi_v1190_trailer_word_count;   //!
	TBranch        *b_hi_v1190_event_id;   //!
	TBranch        *b_hi_v1190_bunch_id;   //!
	TBranch        *b_hi_v1190_status;   //!
	TBranch        *b_hi_v1190_type;   //!
	TBranch        *b_hi_v1190_extended_trigger;   //!
	TBranch        *b_hi_v1190_fifo0_measurement;   //!
	TBranch        *b_hi_v1190_fifo0_channel;   //!
	TBranch        *b_hi_v1190_fifo0_number;   //!
	TBranch        *b_hi_v1190_fifo1_measurement;   //!
	TBranch        *b_hi_v1190_fifo1_channel;   //!
	TBranch        *b_hi_v1190_fifo1_number;   //!
	TBranch        *b_hi_dsssd_ecal;   //!
	TBranch        *b_hi_dsssd_efront;   //!
	TBranch        *b_hi_dsssd_eback;   //!
	TBranch        *b_hi_dsssd_hit_front;   //!
	TBranch        *b_hi_dsssd_hit_back;   //!
	TBranch        *b_hi_dsssd_tfront;   //!
	TBranch        *b_hi_dsssd_tback;   //!
	TBranch        *b_hi_ic_anode;   //!
	TBranch        *b_hi_ic_tcal;   //!
	TBranch        *b_hi_ic_sum;   //!
	TBranch        *b_hi_nai_ecal;   //!
	TBranch        *b_hi_ge_ecal;   //!
	TBranch        *b_hi_mcp_anode;   //!
	TBranch        *b_hi_mcp_tcal;   //!
	TBranch        *b_hi_mcp_esum;   //!
	TBranch        *b_hi_mcp_tac;   //!
	TBranch        *b_hi_mcp_x;   //!
	TBranch        *b_hi_mcp_y;   //!
	TBranch        *b_hi_sb_ecal;   //!
	TBranch        *b_hi_tof_mcp;   //!
	TBranch        *b_hi_tof_mcp_dsssd;   //!
	TBranch        *b_hi_tof_mcp_ic;   //!
	TBranch        *b_hi_tcal_rf;   //!
	TBranch        *b_hi_tcal0;   //!
	TBranch        *b_hi_tcalx;   //!

	TailSelector(TTree * /*tree*/ =0) : ASelector(0) { }
	virtual ~TailSelector() { }
	virtual void    Init(TTree *tree);

	ClassDef(TailSelector,0);
};


//! Selector class for coincidence events.

/// The intended use of this class is for users to derive from it and then
/// implement the following methods:
///
///    - Begin():        called every time a loop on the tree starts,
///                      a convenient place to create your histograms.
///    - Process():      called for each event, in this function you decide what
///                      to read and fill your histograms.
///    - Terminate():    called at the end of the loop on the tree,
///                      a convenient place to draw/fit your histograms.
///
/// Optionally, derived classes can also implement Notify() and Init().
///
/// For an example on how to use this class, see the file examples/Selectors.cxx
///
class CoincSelector : public ASelector {
public :
	// Declaration of leaf types
	//dragon::Coinc   *coinc;
	UShort_t        head_header_fEventId;
	UShort_t        head_header_fTriggerMask;
	UInt_t          head_header_fSerialNumber;
	UInt_t          head_header_fTimeStamp;
	UInt_t          head_header_fDataSize;
	UInt_t          head_io32_header;
	UInt_t          head_io32_trig_count;
	UInt_t          head_io32_tstamp;
	UInt_t          head_io32_start;
	UInt_t          head_io32_end;
	UInt_t          head_io32_latency;
	UInt_t          head_io32_read_time;
	UInt_t          head_io32_busy_time;
	UInt_t          head_io32_trigger_latch;
	UInt_t          head_io32_which_trigger;
	Int_t           head_io32_tsc4_n_fifo[4];
	Double_t        head_io32_tsc4_trig_time;
	Short_t         head_v792_n_ch;
	Int_t           head_v792_count;
	Bool_t          head_v792_overflow;
	Bool_t          head_v792_underflow;
	Short_t         head_v792_data[32];
	Short_t         head_v1190_n_ch;
	Int_t           head_v1190_count;
	Short_t         head_v1190_word_count;
	Short_t         head_v1190_trailer_word_count;
	Short_t         head_v1190_event_id;
	Short_t         head_v1190_bunch_id;
	Short_t         head_v1190_status;
	Short_t         head_v1190_type;
	Int_t           head_v1190_extended_trigger;
	std::vector<unsigned int> head_v1190_fifo0_measurement;
	std::vector<unsigned short> head_v1190_fifo0_channel;
	std::vector<unsigned short> head_v1190_fifo0_number;
	std::vector<unsigned int> head_v1190_fifo1_measurement;
	std::vector<unsigned short> head_v1190_fifo1_channel;
	std::vector<unsigned short> head_v1190_fifo1_number;
	Double_t        head_bgo_ecal[30];
	Double_t        head_bgo_tcal[30];
	Double_t        head_bgo_esort[30];
	Double_t        head_bgo_sum;
	Int_t           head_bgo_hit0;
	Double_t        head_bgo_x0;
	Double_t        head_bgo_y0;
	Double_t        head_bgo_z0;
	Double_t        head_bgo_t0;
	Double_t        head_tcal0;
	Double_t        head_tcalx;
	Double_t        head_tcal_rf;
	UShort_t        tail_header_fEventId;
	UShort_t        tail_header_fTriggerMask;
	UInt_t          tail_header_fSerialNumber;
	UInt_t          tail_header_fTimeStamp;
	UInt_t          tail_header_fDataSize;
	UInt_t          tail_io32_header;
	UInt_t          tail_io32_trig_count;
	UInt_t          tail_io32_tstamp;
	UInt_t          tail_io32_start;
	UInt_t          tail_io32_end;
	UInt_t          tail_io32_latency;
	UInt_t          tail_io32_read_time;
	UInt_t          tail_io32_busy_time;
	UInt_t          tail_io32_trigger_latch;
	UInt_t          tail_io32_which_trigger;
	Int_t           tail_io32_tsc4_n_fifo[4];
	Double_t        tail_io32_tsc4_trig_time;
	vme::V792       tail_v785[2];
	Short_t         tail_v1190_n_ch;
	Int_t           tail_v1190_count;
	Short_t         tail_v1190_word_count;
	Short_t         tail_v1190_trailer_word_count;
	Short_t         tail_v1190_event_id;
	Short_t         tail_v1190_bunch_id;
	Short_t         tail_v1190_status;
	Short_t         tail_v1190_type;
	Int_t           tail_v1190_extended_trigger;
	std::vector<unsigned int> tail_v1190_fifo0_measurement;
	std::vector<unsigned short> tail_v1190_fifo0_channel;
	std::vector<unsigned short> tail_v1190_fifo0_number;
	std::vector<unsigned int> tail_v1190_fifo1_measurement;
	std::vector<unsigned short> tail_v1190_fifo1_channel;
	std::vector<unsigned short> tail_v1190_fifo1_number;
	Double_t        tail_dsssd_ecal[32];
	Double_t        tail_dsssd_efront;
	Double_t        tail_dsssd_eback;
	UInt_t          tail_dsssd_hit_front;
	UInt_t          tail_dsssd_hit_back;
	Double_t        tail_dsssd_tfront;
	Double_t        tail_dsssd_tback;
	Double_t        tail_ic_anode[5];
	Double_t        tail_ic_tcal[4];
	Double_t        tail_ic_sum;
	Double_t        tail_nai_ecal[2];
	Double_t        tail_ge_ecal;
	Double_t        tail_mcp_anode[4];
	Double_t        tail_mcp_tcal[2];
	Double_t        tail_mcp_esum;
	Double_t        tail_mcp_tac;
	Double_t        tail_mcp_x;
	Double_t        tail_mcp_y;
	Double_t        tail_sb_ecal[2];
	Double_t        tail_tof_mcp;
	Double_t        tail_tof_mcp_dsssd;
	Double_t        tail_tof_mcp_ic;
	Double_t        tail_tcal_rf;
	Double_t        tail_tcal0;
	Double_t        tail_tcalx;
	Double_t        xtrig;
	Double_t        xtofh;
	Double_t        xtoft;

	// List of branches
	TBranch        *b_coinc_head_header_fEventId;   //!
	TBranch        *b_coinc_head_header_fTriggerMask;   //!
	TBranch        *b_coinc_head_header_fSerialNumber;   //!
	TBranch        *b_coinc_head_header_fTimeStamp;   //!
	TBranch        *b_coinc_head_header_fDataSize;   //!
	TBranch        *b_coinc_head_io32_header;   //!
	TBranch        *b_coinc_head_io32_trig_count;   //!
	TBranch        *b_coinc_head_io32_tstamp;   //!
	TBranch        *b_coinc_head_io32_start;   //!
	TBranch        *b_coinc_head_io32_end;   //!
	TBranch        *b_coinc_head_io32_latency;   //!
	TBranch        *b_coinc_head_io32_read_time;   //!
	TBranch        *b_coinc_head_io32_busy_time;   //!
	TBranch        *b_coinc_head_io32_trigger_latch;   //!
	TBranch        *b_coinc_head_io32_which_trigger;   //!
	TBranch        *b_coinc_head_io32_tsc4_n_fifo;   //!
	TBranch        *b_coinc_head_io32_tsc4_trig_time;   //!
	TBranch        *b_coinc_head_v792_n_ch;   //!
	TBranch        *b_coinc_head_v792_count;   //!
	TBranch        *b_coinc_head_v792_overflow;   //!
	TBranch        *b_coinc_head_v792_underflow;   //!
	TBranch        *b_coinc_head_v792_data;   //!
	TBranch        *b_coinc_head_v1190_n_ch;   //!
	TBranch        *b_coinc_head_v1190_count;   //!
	TBranch        *b_coinc_head_v1190_word_count;   //!
	TBranch        *b_coinc_head_v1190_trailer_word_count;   //!
	TBranch        *b_coinc_head_v1190_event_id;   //!
	TBranch        *b_coinc_head_v1190_bunch_id;   //!
	TBranch        *b_coinc_head_v1190_status;   //!
	TBranch        *b_coinc_head_v1190_type;   //!
	TBranch        *b_coinc_head_v1190_extended_trigger;   //!
	TBranch        *b_coinc_head_v1190_fifo0_measurement;   //!
	TBranch        *b_coinc_head_v1190_fifo0_channel;   //!
	TBranch        *b_coinc_head_v1190_fifo0_number;   //!
	TBranch        *b_coinc_head_v1190_fifo1_measurement;   //!
	TBranch        *b_coinc_head_v1190_fifo1_channel;   //!
	TBranch        *b_coinc_head_v1190_fifo1_number;   //!
	TBranch        *b_coinc_head_bgo_ecal;   //!
	TBranch        *b_coinc_head_bgo_tcal;   //!
	TBranch        *b_coinc_head_bgo_esort;   //!
	TBranch        *b_coinc_head_bgo_sum;   //!
	TBranch        *b_coinc_head_bgo_hit0;   //!
	TBranch        *b_coinc_head_bgo_x0;   //!
	TBranch        *b_coinc_head_bgo_y0;   //!
	TBranch        *b_coinc_head_bgo_z0;   //!
	TBranch        *b_coinc_head_bgo_t0;   //!
	TBranch        *b_coinc_head_tcal0;   //!
	TBranch        *b_coinc_head_tcalx;   //!
	TBranch        *b_coinc_head_tcal_rf;   //!
	TBranch        *b_coinc_tail_header_fEventId;   //!
	TBranch        *b_coinc_tail_header_fTriggerMask;   //!
	TBranch        *b_coinc_tail_header_fSerialNumber;   //!
	TBranch        *b_coinc_tail_header_fTimeStamp;   //!
	TBranch        *b_coinc_tail_header_fDataSize;   //!
	TBranch        *b_coinc_tail_io32_header;   //!
	TBranch        *b_coinc_tail_io32_trig_count;   //!
	TBranch        *b_coinc_tail_io32_tstamp;   //!
	TBranch        *b_coinc_tail_io32_start;   //!
	TBranch        *b_coinc_tail_io32_end;   //!
	TBranch        *b_coinc_tail_io32_latency;   //!
	TBranch        *b_coinc_tail_io32_read_time;   //!
	TBranch        *b_coinc_tail_io32_busy_time;   //!
	TBranch        *b_coinc_tail_io32_trigger_latch;   //!
	TBranch        *b_coinc_tail_io32_which_trigger;   //!
	TBranch        *b_coinc_tail_io32_tsc4_n_fifo;   //!
	TBranch        *b_coinc_tail_io32_tsc4_trig_time;   //!
	TBranch        *b_coinc_tail_v785;   //!
	TBranch        *b_coinc_tail_v1190_n_ch;   //!
	TBranch        *b_coinc_tail_v1190_count;   //!
	TBranch        *b_coinc_tail_v1190_word_count;   //!
	TBranch        *b_coinc_tail_v1190_trailer_word_count;   //!
	TBranch        *b_coinc_tail_v1190_event_id;   //!
	TBranch        *b_coinc_tail_v1190_bunch_id;   //!
	TBranch        *b_coinc_tail_v1190_status;   //!
	TBranch        *b_coinc_tail_v1190_type;   //!
	TBranch        *b_coinc_tail_v1190_extended_trigger;   //!
	TBranch        *b_coinc_tail_v1190_fifo0_measurement;   //!
	TBranch        *b_coinc_tail_v1190_fifo0_channel;   //!
	TBranch        *b_coinc_tail_v1190_fifo0_number;   //!
	TBranch        *b_coinc_tail_v1190_fifo1_measurement;   //!
	TBranch        *b_coinc_tail_v1190_fifo1_channel;   //!
	TBranch        *b_coinc_tail_v1190_fifo1_number;   //!
	TBranch        *b_coinc_tail_dsssd_ecal;   //!
	TBranch        *b_coinc_tail_dsssd_efront;   //!
	TBranch        *b_coinc_tail_dsssd_eback;   //!
	TBranch        *b_coinc_tail_dsssd_hit_front;   //!
	TBranch        *b_coinc_tail_dsssd_hit_back;   //!
	TBranch        *b_coinc_tail_dsssd_tfront;   //!
	TBranch        *b_coinc_tail_dsssd_tback;   //!
	TBranch        *b_coinc_tail_ic_anode;   //!
	TBranch        *b_coinc_tail_ic_tcal;   //!
	TBranch        *b_coinc_tail_ic_sum;   //!
	TBranch        *b_coinc_tail_nai_ecal;   //!
	TBranch        *b_coinc_tail_ge_ecal;   //!
	TBranch        *b_coinc_tail_mcp_anode;   //!
	TBranch        *b_coinc_tail_mcp_tcal;   //!
	TBranch        *b_coinc_tail_mcp_esum;   //!
	TBranch        *b_coinc_tail_mcp_tac;   //!
	TBranch        *b_coinc_tail_mcp_x;   //!
	TBranch        *b_coinc_tail_mcp_y;   //!
	TBranch        *b_coinc_tail_sb_ecal;   //!
	TBranch        *b_coinc_tail_tof_mcp;   //!
	TBranch        *b_coinc_tail_tof_mcp_dsssd;   //!
	TBranch        *b_coinc_tail_tof_mcp_ic;   //!
	TBranch        *b_coinc_tail_tcal_rf;   //!
	TBranch        *b_coinc_tail_tcal0;   //!
	TBranch        *b_coinc_tail_tcalx;   //!
	TBranch        *b_coinc_xtrig;   //!
	TBranch        *b_coinc_xtofh;   //!
	TBranch        *b_coinc_xtoft;   //!

	CoincSelector(TTree * /*tree*/ =0) : ASelector(0) { }
	virtual ~CoincSelector() { }
	virtual void    Init(TTree *tree);
	ClassDef(CoincSelector,0);
};

//! Selector class for scaler events.

/// The intended use of this class is for users to derive from it and then
/// implement the following methods:
///
///    - Begin():        called every time a loop on the tree starts,
///                      a convenient place to create your histograms.
///    - Process():      called for each event, in this function you decide what
///                      to read and fill your histograms.
///    - Terminate():    called at the end of the loop on the tree,
///                      a convenient place to draw/fit your histograms.
///
/// Optionally, derived classes can also implement Notify() and Init().
///
/// For an example on how to use this class, see the file examples/Selectors.cxx
///
class ScalerSelector : public ASelector {
public :
	// Declaration of leaf types
	//dragon::Scaler  *sch;
	UInt_t          count[17];
	UInt_t          sum[17];
	Double_t        rate[17];

	// List of branches
	TBranch        *b_sch_count;   //!
	TBranch        *b_sch_sum;   //!
	TBranch        *b_sch_rate;   //!

	ScalerSelector(TTree * /*tree*/ =0) : ASelector(0) { }
	virtual ~ScalerSelector() { }
	virtual void    Init(TTree *tree);

	ClassDef(ScalerSelector,0);
};


/// Class to recalibrate the DSSSD channels
class DsssdCalibrate : public ASelector {
protected:
	/// Output file
	TFile* fOut;
	/// Output tree (HI singles)
	TTree* fT3;
	/// Output tree (Coinc)
	TTree* fT5;
	/// Input tree/chain (Coinc)
	TTree* fChain5;

public:
	/// Output DSSSD (singles)
	dragon::Dsssd* fDsssd3;
	/// Output DSSSD (coinc)
	dragon::Dsssd* fDsssd5;

	/// Input ADC (singles)
	vme::V792 fAdc3[2];
	/// Input ADC (coinc)
	vme::V792 fAdc5[2];

	/// Input ADC branch (singles)
	TBranch* fAdcBranch3; //!
	/// Input ADC branch (coinc)
	TBranch* fAdcBranch5; //!

public:
	/// Construct from slope and offset array
	DsssdCalibrate(Double_t* slopes, Double_t* offsets);
	/// Construct from database class instance
	DsssdCalibrate(midas::Database* odb);
	/// Construct from database file
	DsssdCalibrate(const char* odbfile);
	/// Clean up
	~DsssdCalibrate();
	/// Initialization routines
	void Init(TTree*);
	///
	void Begin(TTree*);
	///
	Bool_t Process(Long64_t entry);
	/// Switch output to a new file
	Bool_t Notify();
	///
	void Terminate();

private:
	/// Constructor helper
	void InitMembers();
	/// Process() helper
	void ProcessEntry(TTree* tout, Dsssd* dsssd, vme::V792* adc);
};

} // namespace dragon


#endif
