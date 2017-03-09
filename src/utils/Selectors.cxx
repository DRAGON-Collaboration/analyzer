///
/// \file Selectors.cxx
/// \brief Implements selector classes
///
#include "midas/Database.hxx"
#include "Selectors.hxx"


void dragon::ASelector::Begin(TTree*)
{
	//! BOR actions
	///
	/// The Begin() function is called at the start of the query.
	/// When running with PROOF Begin() is only called on the client.
	/// The tree argument is deprecated (on PROOF 0 is passed).
	///
	/// \note Derived classes should always implement this method.
	AbstractMethod("Begin");
}

Bool_t dragon::ASelector::Process(Long64_t)
{
	//! Event-by-event actions.
	///
	/// The Process() function is called for each entry in the tree (or possibly
	/// keyed object in the case of PROOF) to be processed. The entry argument
	/// specifies which entry in the currently loaded tree is to be processed.
	/// It can be passed to either dragon::HeadSelector::GetEntry() or TBranch::GetEntry()
	/// to read either all or the required parts of the data. When processing
	/// keyed objects with PROOF, the object is already loaded and is available
	/// via the fObject pointer.
	///
	/// This function should contain the "body" of the analysis. It can contain
	/// simple or elaborate selection criteria, run algorithms on the data
	/// of the event and typically fill histograms.
	///
	/// The processing can be stopped by calling Abort().
	///
	/// Use fStatus to set the return value of TTree::Process().
	///
	/// The return value is currently not used.
	///
	/// \note Derived classes should always implement this method.
	///
	/// \warning When a selector is used with a TChain:
	/// in the Process, ProcessCut, ProcessFill function, you must use
	/// the pointer to the current Tree to call `GetEntry(entry).`
	/// entry is always the local entry number in the current tree.
	/// Assuming that fChain is the pointer to the TChain being processed,
	/// use `fChain->GetTree()->GetEntry(entry);`
	///
	AbstractMethod("Process");
	return kTRUE;
}

void dragon::ASelector::Terminate()
{
	//! EOR actions
	///
	/// The Terminate() function is the last function to be called during
	/// a query. It always runs on the client, it can be used to present
	/// the results graphically or save the results to file.
	///
	/// \note Derived classes should always implement this method.
	AbstractMethod("Terminate");
}

Bool_t dragon::ASelector::Notify()
{
	//! File open actions.
	///
	/// The Notify() function is called when a new file is opened. This
	/// can be either for a new TTree in a TChain or when when a new TTree
	/// is started when using PROOF. It is normally not necessary to make changes
	/// to the generated code, but the routine can be extended by the
	/// user if needed. The return value is currently not used.
	return kTRUE;
}

void dragon::ASelector::Init(TTree*)
{
	//! Set branch addresses and branch pointers.
	///
	/// The Init() function is called when the selector needs to initialize
	/// a new tree or chain. Typically here the branch addresses and branch
	/// pointers of the tree will be set.
	/// It is normally not necessary to make changes to the generated
	/// code, but the routine can be extended by the user if needed.
	/// Init() will be called many times when running on PROOF
	/// (once per file to be processed).
	///
	/// For DRAGON, this method is implemented in HeadSelector, TailSelector, etc.
	/// If desired, users may choose to override it in their derived classes.
	AbstractMethod("Init");
}

void dragon::HeadSelector::Init(TTree *tree)
{
	/// See ASelector::Init()
	if (!tree) return;
	fChain = tree;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("header.fEventId", &header_fEventId, &b_gamma_header_fEventId);
	fChain->SetBranchAddress("header.fTriggerMask", &header_fTriggerMask, &b_gamma_header_fTriggerMask);
	fChain->SetBranchAddress("header.fSerialNumber", &header_fSerialNumber, &b_gamma_header_fSerialNumber);
	fChain->SetBranchAddress("header.fTimeStamp", &header_fTimeStamp, &b_gamma_header_fTimeStamp);
	fChain->SetBranchAddress("header.fDataSize", &header_fDataSize, &b_gamma_header_fDataSize);
	fChain->SetBranchAddress("io32.header", &io32_header, &b_gamma_io32_header);
	fChain->SetBranchAddress("io32.trig_count", &io32_trig_count, &b_gamma_io32_trig_count);
	fChain->SetBranchAddress("io32.tstamp", &io32_tstamp, &b_gamma_io32_tstamp);
	fChain->SetBranchAddress("io32.start", &io32_start, &b_gamma_io32_start);
	fChain->SetBranchAddress("io32.end", &io32_end, &b_gamma_io32_end);
	fChain->SetBranchAddress("io32.latency", &io32_latency, &b_gamma_io32_latency);
	fChain->SetBranchAddress("io32.read_time", &io32_read_time, &b_gamma_io32_read_time);
	fChain->SetBranchAddress("io32.busy_time", &io32_busy_time, &b_gamma_io32_busy_time);
	fChain->SetBranchAddress("io32.trigger_latch", &io32_trigger_latch, &b_gamma_io32_trigger_latch);
	fChain->SetBranchAddress("io32.which_trigger", &io32_which_trigger, &b_gamma_io32_which_trigger);
	fChain->SetBranchAddress("io32.tsc4.n_fifo[4]", io32_tsc4_n_fifo, &b_gamma_io32_tsc4_n_fifo);
	fChain->SetBranchAddress("io32.tsc4.trig_time", &io32_tsc4_trig_time, &b_gamma_io32_tsc4_trig_time);
	fChain->SetBranchAddress("v792.n_ch", &v792_n_ch, &b_gamma_v792_n_ch);
	fChain->SetBranchAddress("v792.count", &v792_count, &b_gamma_v792_count);
	fChain->SetBranchAddress("v792.overflow", &v792_overflow, &b_gamma_v792_overflow);
	fChain->SetBranchAddress("v792.underflow", &v792_underflow, &b_gamma_v792_underflow);
	fChain->SetBranchAddress("v792.data[32]", v792_data, &b_gamma_v792_data);
	fChain->SetBranchAddress("v1190.n_ch", &v1190_n_ch, &b_gamma_v1190_n_ch);
	fChain->SetBranchAddress("v1190.count", &v1190_count, &b_gamma_v1190_count);
	fChain->SetBranchAddress("v1190.word_count", &v1190_word_count, &b_gamma_v1190_word_count);
	fChain->SetBranchAddress("v1190.trailer_word_count", &v1190_trailer_word_count, &b_gamma_v1190_trailer_word_count);
	fChain->SetBranchAddress("v1190.event_id", &v1190_event_id, &b_gamma_v1190_event_id);
	fChain->SetBranchAddress("v1190.bunch_id", &v1190_bunch_id, &b_gamma_v1190_bunch_id);
	fChain->SetBranchAddress("v1190.status", &v1190_status, &b_gamma_v1190_status);
	fChain->SetBranchAddress("v1190.type", &v1190_type, &b_gamma_v1190_type);
	fChain->SetBranchAddress("v1190.extended_trigger", &v1190_extended_trigger, &b_gamma_v1190_extended_trigger);
	fChain->SetBranchAddress("v1190.fifo0.measurement", &v1190_fifo0_measurement, &b_gamma_v1190_fifo0_measurement);
	fChain->SetBranchAddress("v1190.fifo0.channel", &v1190_fifo0_channel, &b_gamma_v1190_fifo0_channel);
	fChain->SetBranchAddress("v1190.fifo0.number", &v1190_fifo0_number, &b_gamma_v1190_fifo0_number);
	fChain->SetBranchAddress("v1190.fifo1.measurement", &v1190_fifo1_measurement, &b_gamma_v1190_fifo1_measurement);
	fChain->SetBranchAddress("v1190.fifo1.channel", &v1190_fifo1_channel, &b_gamma_v1190_fifo1_channel);
	fChain->SetBranchAddress("v1190.fifo1.number", &v1190_fifo1_number, &b_gamma_v1190_fifo1_number);
	fChain->SetBranchAddress("bgo.ecal[30]", bgo_ecal, &b_gamma_bgo_ecal);
	fChain->SetBranchAddress("bgo.tcal[30]", bgo_tcal, &b_gamma_bgo_tcal);
	fChain->SetBranchAddress("bgo.esort[30]", bgo_esort, &b_gamma_bgo_esort);
	fChain->SetBranchAddress("bgo.sum", &bgo_sum, &b_gamma_bgo_sum);
	fChain->SetBranchAddress("bgo.hit0", &bgo_hit0, &b_gamma_bgo_hit0);
	fChain->SetBranchAddress("bgo.x0", &bgo_x0, &b_gamma_bgo_x0);
	fChain->SetBranchAddress("bgo.y0", &bgo_y0, &b_gamma_bgo_y0);
	fChain->SetBranchAddress("bgo.z0", &bgo_z0, &b_gamma_bgo_z0);
	fChain->SetBranchAddress("bgo.t0", &bgo_t0, &b_gamma_bgo_t0);
	fChain->SetBranchAddress("tcal0", &tcal0, &b_gamma_tcal0);
	fChain->SetBranchAddress("tcalx", &tcalx, &b_gamma_tcalx);
	fChain->SetBranchAddress("tcal_rf", &tcal_rf, &b_gamma_tcal_rf);
}

void dragon::TailSelector::Init(TTree *tree)
{
	/// See ASelector::Init()
	if (!tree) return;
	fChain = tree;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("header.fEventId", &header_fEventId, &b_hi_header_fEventId);
	fChain->SetBranchAddress("header.fTriggerMask", &header_fTriggerMask, &b_hi_header_fTriggerMask);
	fChain->SetBranchAddress("header.fSerialNumber", &header_fSerialNumber, &b_hi_header_fSerialNumber);
	fChain->SetBranchAddress("header.fTimeStamp", &header_fTimeStamp, &b_hi_header_fTimeStamp);
	fChain->SetBranchAddress("header.fDataSize", &header_fDataSize, &b_hi_header_fDataSize);
	fChain->SetBranchAddress("io32.header", &io32_header, &b_hi_io32_header);
	fChain->SetBranchAddress("io32.trig_count", &io32_trig_count, &b_hi_io32_trig_count);
	fChain->SetBranchAddress("io32.tstamp", &io32_tstamp, &b_hi_io32_tstamp);
	fChain->SetBranchAddress("io32.start", &io32_start, &b_hi_io32_start);
	fChain->SetBranchAddress("io32.end", &io32_end, &b_hi_io32_end);
	fChain->SetBranchAddress("io32.latency", &io32_latency, &b_hi_io32_latency);
	fChain->SetBranchAddress("io32.read_time", &io32_read_time, &b_hi_io32_read_time);
	fChain->SetBranchAddress("io32.busy_time", &io32_busy_time, &b_hi_io32_busy_time);
	fChain->SetBranchAddress("io32.trigger_latch", &io32_trigger_latch, &b_hi_io32_trigger_latch);
	fChain->SetBranchAddress("io32.which_trigger", &io32_which_trigger, &b_hi_io32_which_trigger);
	fChain->SetBranchAddress("io32.tsc4.n_fifo[4]", io32_tsc4_n_fifo, &b_hi_io32_tsc4_n_fifo);
	fChain->SetBranchAddress("io32.tsc4.trig_time", &io32_tsc4_trig_time, &b_hi_io32_tsc4_trig_time);
	fChain->SetBranchAddress("v785[2]", v785, &b_hi_v785);
	fChain->SetBranchAddress("v1190.n_ch", &v1190_n_ch, &b_hi_v1190_n_ch);
	fChain->SetBranchAddress("v1190.count", &v1190_count, &b_hi_v1190_count);
	fChain->SetBranchAddress("v1190.word_count", &v1190_word_count, &b_hi_v1190_word_count);
	fChain->SetBranchAddress("v1190.trailer_word_count", &v1190_trailer_word_count, &b_hi_v1190_trailer_word_count);
	fChain->SetBranchAddress("v1190.event_id", &v1190_event_id, &b_hi_v1190_event_id);
	fChain->SetBranchAddress("v1190.bunch_id", &v1190_bunch_id, &b_hi_v1190_bunch_id);
	fChain->SetBranchAddress("v1190.status", &v1190_status, &b_hi_v1190_status);
	fChain->SetBranchAddress("v1190.type", &v1190_type, &b_hi_v1190_type);
	fChain->SetBranchAddress("v1190.extended_trigger", &v1190_extended_trigger, &b_hi_v1190_extended_trigger);
	fChain->SetBranchAddress("v1190.fifo0.measurement", &v1190_fifo0_measurement, &b_hi_v1190_fifo0_measurement);
	fChain->SetBranchAddress("v1190.fifo0.channel", &v1190_fifo0_channel, &b_hi_v1190_fifo0_channel);
	fChain->SetBranchAddress("v1190.fifo0.number", &v1190_fifo0_number, &b_hi_v1190_fifo0_number);
	fChain->SetBranchAddress("v1190.fifo1.measurement", &v1190_fifo1_measurement, &b_hi_v1190_fifo1_measurement);
	fChain->SetBranchAddress("v1190.fifo1.channel", &v1190_fifo1_channel, &b_hi_v1190_fifo1_channel);
	fChain->SetBranchAddress("v1190.fifo1.number", &v1190_fifo1_number, &b_hi_v1190_fifo1_number);
	fChain->SetBranchAddress("dsssd.ecal[32]", dsssd_ecal, &b_hi_dsssd_ecal);
	fChain->SetBranchAddress("dsssd.efront", &dsssd_efront, &b_hi_dsssd_efront);
	fChain->SetBranchAddress("dsssd.eback", &dsssd_eback, &b_hi_dsssd_eback);
	fChain->SetBranchAddress("dsssd.hit_front", &dsssd_hit_front, &b_hi_dsssd_hit_front);
	fChain->SetBranchAddress("dsssd.hit_back", &dsssd_hit_back, &b_hi_dsssd_hit_back);
	fChain->SetBranchAddress("dsssd.tfront", &dsssd_tfront, &b_hi_dsssd_tfront);
	fChain->SetBranchAddress("dsssd.tback", &dsssd_tback, &b_hi_dsssd_tback);
	fChain->SetBranchAddress("ic.anode[5]", ic_anode, &b_hi_ic_anode);
	fChain->SetBranchAddress("ic.tcal[4]", ic_tcal, &b_hi_ic_tcal);
	fChain->SetBranchAddress("ic.sum", &ic_sum, &b_hi_ic_sum);
	fChain->SetBranchAddress("nai.ecal[2]", nai_ecal, &b_hi_nai_ecal);
	fChain->SetBranchAddress("ge.ecal", &ge_ecal, &b_hi_ge_ecal);
	fChain->SetBranchAddress("mcp.anode[4]", mcp_anode, &b_hi_mcp_anode);
	fChain->SetBranchAddress("mcp.tcal[2]", mcp_tcal, &b_hi_mcp_tcal);
	fChain->SetBranchAddress("mcp.esum", &mcp_esum, &b_hi_mcp_esum);
	fChain->SetBranchAddress("mcp.tac", &mcp_tac, &b_hi_mcp_tac);
	fChain->SetBranchAddress("mcp.x", &mcp_x, &b_hi_mcp_x);
	fChain->SetBranchAddress("mcp.y", &mcp_y, &b_hi_mcp_y);
	fChain->SetBranchAddress("sb.ecal[2]", sb_ecal, &b_hi_sb_ecal);
	fChain->SetBranchAddress("tof.mcp", &tof_mcp, &b_hi_tof_mcp);
	fChain->SetBranchAddress("tof.mcp_dsssd", &tof_mcp_dsssd, &b_hi_tof_mcp_dsssd);
	fChain->SetBranchAddress("tof.mcp_ic", &tof_mcp_ic, &b_hi_tof_mcp_ic);
	fChain->SetBranchAddress("tcal_rf", &tcal_rf, &b_hi_tcal_rf);
	fChain->SetBranchAddress("tcal0", &tcal0, &b_hi_tcal0);
	fChain->SetBranchAddress("tcalx", &tcalx, &b_hi_tcalx);
}

void dragon::CoincSelector::Init(TTree *tree)
{
	/// See ASelector::Init()
	if (!tree) return;
	fChain = tree;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("head.header.fEventId", &head_header_fEventId, &b_coinc_head_header_fEventId);
	fChain->SetBranchAddress("head.header.fTriggerMask", &head_header_fTriggerMask, &b_coinc_head_header_fTriggerMask);
	fChain->SetBranchAddress("head.header.fSerialNumber", &head_header_fSerialNumber, &b_coinc_head_header_fSerialNumber);
	fChain->SetBranchAddress("head.header.fTimeStamp", &head_header_fTimeStamp, &b_coinc_head_header_fTimeStamp);
	fChain->SetBranchAddress("head.header.fDataSize", &head_header_fDataSize, &b_coinc_head_header_fDataSize);
	fChain->SetBranchAddress("head.io32.header", &head_io32_header, &b_coinc_head_io32_header);
	fChain->SetBranchAddress("head.io32.trig_count", &head_io32_trig_count, &b_coinc_head_io32_trig_count);
	fChain->SetBranchAddress("head.io32.tstamp", &head_io32_tstamp, &b_coinc_head_io32_tstamp);
	fChain->SetBranchAddress("head.io32.start", &head_io32_start, &b_coinc_head_io32_start);
	fChain->SetBranchAddress("head.io32.end", &head_io32_end, &b_coinc_head_io32_end);
	fChain->SetBranchAddress("head.io32.latency", &head_io32_latency, &b_coinc_head_io32_latency);
	fChain->SetBranchAddress("head.io32.read_time", &head_io32_read_time, &b_coinc_head_io32_read_time);
	fChain->SetBranchAddress("head.io32.busy_time", &head_io32_busy_time, &b_coinc_head_io32_busy_time);
	fChain->SetBranchAddress("head.io32.trigger_latch", &head_io32_trigger_latch, &b_coinc_head_io32_trigger_latch);
	fChain->SetBranchAddress("head.io32.which_trigger", &head_io32_which_trigger, &b_coinc_head_io32_which_trigger);
	fChain->SetBranchAddress("head.io32.tsc4.n_fifo[4]", head_io32_tsc4_n_fifo, &b_coinc_head_io32_tsc4_n_fifo);
	fChain->SetBranchAddress("head.io32.tsc4.trig_time", &head_io32_tsc4_trig_time, &b_coinc_head_io32_tsc4_trig_time);
	fChain->SetBranchAddress("head.v792.n_ch", &head_v792_n_ch, &b_coinc_head_v792_n_ch);
	fChain->SetBranchAddress("head.v792.count", &head_v792_count, &b_coinc_head_v792_count);
	fChain->SetBranchAddress("head.v792.overflow", &head_v792_overflow, &b_coinc_head_v792_overflow);
	fChain->SetBranchAddress("head.v792.underflow", &head_v792_underflow, &b_coinc_head_v792_underflow);
	fChain->SetBranchAddress("head.v792.data[32]", head_v792_data, &b_coinc_head_v792_data);
	fChain->SetBranchAddress("head.v1190.n_ch", &head_v1190_n_ch, &b_coinc_head_v1190_n_ch);
	fChain->SetBranchAddress("head.v1190.count", &head_v1190_count, &b_coinc_head_v1190_count);
	fChain->SetBranchAddress("head.v1190.word_count", &head_v1190_word_count, &b_coinc_head_v1190_word_count);
	fChain->SetBranchAddress("head.v1190.trailer_word_count", &head_v1190_trailer_word_count, &b_coinc_head_v1190_trailer_word_count);
	fChain->SetBranchAddress("head.v1190.event_id", &head_v1190_event_id, &b_coinc_head_v1190_event_id);
	fChain->SetBranchAddress("head.v1190.bunch_id", &head_v1190_bunch_id, &b_coinc_head_v1190_bunch_id);
	fChain->SetBranchAddress("head.v1190.status", &head_v1190_status, &b_coinc_head_v1190_status);
	fChain->SetBranchAddress("head.v1190.type", &head_v1190_type, &b_coinc_head_v1190_type);
	fChain->SetBranchAddress("head.v1190.extended_trigger", &head_v1190_extended_trigger, &b_coinc_head_v1190_extended_trigger);
	fChain->SetBranchAddress("head.v1190.fifo0.measurement", &head_v1190_fifo0_measurement, &b_coinc_head_v1190_fifo0_measurement);
	fChain->SetBranchAddress("head.v1190.fifo0.channel", &head_v1190_fifo0_channel, &b_coinc_head_v1190_fifo0_channel);
	fChain->SetBranchAddress("head.v1190.fifo0.number", &head_v1190_fifo0_number, &b_coinc_head_v1190_fifo0_number);
	fChain->SetBranchAddress("head.v1190.fifo1.measurement", &head_v1190_fifo1_measurement, &b_coinc_head_v1190_fifo1_measurement);
	fChain->SetBranchAddress("head.v1190.fifo1.channel", &head_v1190_fifo1_channel, &b_coinc_head_v1190_fifo1_channel);
	fChain->SetBranchAddress("head.v1190.fifo1.number", &head_v1190_fifo1_number, &b_coinc_head_v1190_fifo1_number);
	fChain->SetBranchAddress("head.bgo.ecal[30]", head_bgo_ecal, &b_coinc_head_bgo_ecal);
	fChain->SetBranchAddress("head.bgo.tcal[30]", head_bgo_tcal, &b_coinc_head_bgo_tcal);
	fChain->SetBranchAddress("head.bgo.esort[30]", head_bgo_esort, &b_coinc_head_bgo_esort);
	fChain->SetBranchAddress("head.bgo.sum", &head_bgo_sum, &b_coinc_head_bgo_sum);
	fChain->SetBranchAddress("head.bgo.hit0", &head_bgo_hit0, &b_coinc_head_bgo_hit0);
	fChain->SetBranchAddress("head.bgo.x0", &head_bgo_x0, &b_coinc_head_bgo_x0);
	fChain->SetBranchAddress("head.bgo.y0", &head_bgo_y0, &b_coinc_head_bgo_y0);
	fChain->SetBranchAddress("head.bgo.z0", &head_bgo_z0, &b_coinc_head_bgo_z0);
	fChain->SetBranchAddress("head.bgo.t0", &head_bgo_t0, &b_coinc_head_bgo_t0);
	fChain->SetBranchAddress("head.tcal0", &head_tcal0, &b_coinc_head_tcal0);
	fChain->SetBranchAddress("head.tcalx", &head_tcalx, &b_coinc_head_tcalx);
	fChain->SetBranchAddress("head.tcal_rf", &head_tcal_rf, &b_coinc_head_tcal_rf);
	fChain->SetBranchAddress("tail.header.fEventId", &tail_header_fEventId, &b_coinc_tail_header_fEventId);
	fChain->SetBranchAddress("tail.header.fTriggerMask", &tail_header_fTriggerMask, &b_coinc_tail_header_fTriggerMask);
	fChain->SetBranchAddress("tail.header.fSerialNumber", &tail_header_fSerialNumber, &b_coinc_tail_header_fSerialNumber);
	fChain->SetBranchAddress("tail.header.fTimeStamp", &tail_header_fTimeStamp, &b_coinc_tail_header_fTimeStamp);
	fChain->SetBranchAddress("tail.header.fDataSize", &tail_header_fDataSize, &b_coinc_tail_header_fDataSize);
	fChain->SetBranchAddress("tail.io32.header", &tail_io32_header, &b_coinc_tail_io32_header);
	fChain->SetBranchAddress("tail.io32.trig_count", &tail_io32_trig_count, &b_coinc_tail_io32_trig_count);
	fChain->SetBranchAddress("tail.io32.tstamp", &tail_io32_tstamp, &b_coinc_tail_io32_tstamp);
	fChain->SetBranchAddress("tail.io32.start", &tail_io32_start, &b_coinc_tail_io32_start);
	fChain->SetBranchAddress("tail.io32.end", &tail_io32_end, &b_coinc_tail_io32_end);
	fChain->SetBranchAddress("tail.io32.latency", &tail_io32_latency, &b_coinc_tail_io32_latency);
	fChain->SetBranchAddress("tail.io32.read_time", &tail_io32_read_time, &b_coinc_tail_io32_read_time);
	fChain->SetBranchAddress("tail.io32.busy_time", &tail_io32_busy_time, &b_coinc_tail_io32_busy_time);
	fChain->SetBranchAddress("tail.io32.trigger_latch", &tail_io32_trigger_latch, &b_coinc_tail_io32_trigger_latch);
	fChain->SetBranchAddress("tail.io32.which_trigger", &tail_io32_which_trigger, &b_coinc_tail_io32_which_trigger);
	fChain->SetBranchAddress("tail.io32.tsc4.n_fifo[4]", tail_io32_tsc4_n_fifo, &b_coinc_tail_io32_tsc4_n_fifo);
	fChain->SetBranchAddress("tail.io32.tsc4.trig_time", &tail_io32_tsc4_trig_time, &b_coinc_tail_io32_tsc4_trig_time);
	fChain->SetBranchAddress("tail.v785[2]", tail_v785, &b_coinc_tail_v785);
	fChain->SetBranchAddress("tail.v1190.n_ch", &tail_v1190_n_ch, &b_coinc_tail_v1190_n_ch);
	fChain->SetBranchAddress("tail.v1190.count", &tail_v1190_count, &b_coinc_tail_v1190_count);
	fChain->SetBranchAddress("tail.v1190.word_count", &tail_v1190_word_count, &b_coinc_tail_v1190_word_count);
	fChain->SetBranchAddress("tail.v1190.trailer_word_count", &tail_v1190_trailer_word_count, &b_coinc_tail_v1190_trailer_word_count);
	fChain->SetBranchAddress("tail.v1190.event_id", &tail_v1190_event_id, &b_coinc_tail_v1190_event_id);
	fChain->SetBranchAddress("tail.v1190.bunch_id", &tail_v1190_bunch_id, &b_coinc_tail_v1190_bunch_id);
	fChain->SetBranchAddress("tail.v1190.status", &tail_v1190_status, &b_coinc_tail_v1190_status);
	fChain->SetBranchAddress("tail.v1190.type", &tail_v1190_type, &b_coinc_tail_v1190_type);
	fChain->SetBranchAddress("tail.v1190.extended_trigger", &tail_v1190_extended_trigger, &b_coinc_tail_v1190_extended_trigger);
	fChain->SetBranchAddress("tail.v1190.fifo0.measurement", &tail_v1190_fifo0_measurement, &b_coinc_tail_v1190_fifo0_measurement);
	fChain->SetBranchAddress("tail.v1190.fifo0.channel", &tail_v1190_fifo0_channel, &b_coinc_tail_v1190_fifo0_channel);
	fChain->SetBranchAddress("tail.v1190.fifo0.number", &tail_v1190_fifo0_number, &b_coinc_tail_v1190_fifo0_number);
	fChain->SetBranchAddress("tail.v1190.fifo1.measurement", &tail_v1190_fifo1_measurement, &b_coinc_tail_v1190_fifo1_measurement);
	fChain->SetBranchAddress("tail.v1190.fifo1.channel", &tail_v1190_fifo1_channel, &b_coinc_tail_v1190_fifo1_channel);
	fChain->SetBranchAddress("tail.v1190.fifo1.number", &tail_v1190_fifo1_number, &b_coinc_tail_v1190_fifo1_number);
	fChain->SetBranchAddress("tail.dsssd.ecal[32]", tail_dsssd_ecal, &b_coinc_tail_dsssd_ecal);
	fChain->SetBranchAddress("tail.dsssd.efront", &tail_dsssd_efront, &b_coinc_tail_dsssd_efront);
	fChain->SetBranchAddress("tail.dsssd.eback", &tail_dsssd_eback, &b_coinc_tail_dsssd_eback);
	fChain->SetBranchAddress("tail.dsssd.hit_front", &tail_dsssd_hit_front, &b_coinc_tail_dsssd_hit_front);
	fChain->SetBranchAddress("tail.dsssd.hit_back", &tail_dsssd_hit_back, &b_coinc_tail_dsssd_hit_back);
	fChain->SetBranchAddress("tail.dsssd.tfront", &tail_dsssd_tfront, &b_coinc_tail_dsssd_tfront);
	fChain->SetBranchAddress("tail.dsssd.tback", &tail_dsssd_tback, &b_coinc_tail_dsssd_tback);
	fChain->SetBranchAddress("tail.ic.anode[5]", tail_ic_anode, &b_coinc_tail_ic_anode);
	fChain->SetBranchAddress("tail.ic.tcal[4]", tail_ic_tcal, &b_coinc_tail_ic_tcal);
	fChain->SetBranchAddress("tail.ic.sum", &tail_ic_sum, &b_coinc_tail_ic_sum);
	fChain->SetBranchAddress("tail.nai.ecal[2]", tail_nai_ecal, &b_coinc_tail_nai_ecal);
	fChain->SetBranchAddress("tail.ge.ecal", &tail_ge_ecal, &b_coinc_tail_ge_ecal);
	fChain->SetBranchAddress("tail.mcp.anode[4]", tail_mcp_anode, &b_coinc_tail_mcp_anode);
	fChain->SetBranchAddress("tail.mcp.tcal[2]", tail_mcp_tcal, &b_coinc_tail_mcp_tcal);
	fChain->SetBranchAddress("tail.mcp.esum", &tail_mcp_esum, &b_coinc_tail_mcp_esum);
	fChain->SetBranchAddress("tail.mcp.tac", &tail_mcp_tac, &b_coinc_tail_mcp_tac);
	fChain->SetBranchAddress("tail.mcp.x", &tail_mcp_x, &b_coinc_tail_mcp_x);
	fChain->SetBranchAddress("tail.mcp.y", &tail_mcp_y, &b_coinc_tail_mcp_y);
	fChain->SetBranchAddress("tail.sb.ecal[2]", tail_sb_ecal, &b_coinc_tail_sb_ecal);
	fChain->SetBranchAddress("tail.tof.mcp", &tail_tof_mcp, &b_coinc_tail_tof_mcp);
	fChain->SetBranchAddress("tail.tof.mcp_dsssd", &tail_tof_mcp_dsssd, &b_coinc_tail_tof_mcp_dsssd);
	fChain->SetBranchAddress("tail.tof.mcp_ic", &tail_tof_mcp_ic, &b_coinc_tail_tof_mcp_ic);
	fChain->SetBranchAddress("tail.tcal_rf", &tail_tcal_rf, &b_coinc_tail_tcal_rf);
	fChain->SetBranchAddress("tail.tcal0", &tail_tcal0, &b_coinc_tail_tcal0);
	fChain->SetBranchAddress("tail.tcalx", &tail_tcalx, &b_coinc_tail_tcalx);
	fChain->SetBranchAddress("xtrig", &xtrig, &b_coinc_xtrig);
	fChain->SetBranchAddress("xtofh", &xtofh, &b_coinc_xtofh);
	fChain->SetBranchAddress("xtoft", &xtoft, &b_coinc_xtoft);
}

void dragon::ScalerSelector::Init(TTree *tree)
{
	/// See ASelector::Init()
	if (!tree) return;
	fChain = tree;
	fChain->SetMakeClass(1);

	fChain->SetBranchAddress("count[17]", count, &b_sch_count);
	fChain->SetBranchAddress("sum[17]", sum, &b_sch_sum);
	fChain->SetBranchAddress("rate[17]", rate, &b_sch_rate);
}




// =========== Class dragon::DsssdCalibrate ============ //

dragon::DsssdCalibrate::DsssdCalibrate(Double_t* slopes, Double_t* offsets)
{
	///
	/// \param slopes  Array of 32 calibration slopes
	/// \param offsets Array of 32 calibration offsets
	///
	InitMembers();
	for(Int_t i=0; i< Dsssd::MAX_CHANNELS; ++i) {
		fDsssd3->variables.adc.slope[i]  = slopes[i];
		fDsssd3->variables.adc.offset[i] = offsets[i];

		fDsssd5->variables.adc.slope[i]  = slopes[i];
		fDsssd5->variables.adc.offset[i] = offsets[i];
	}
}

dragon::DsssdCalibrate::DsssdCalibrate(midas::Database* odb)
{
	///
	/// \param odb Pointer to a midas::Database instance containing the desired calibration
	///  slopes and offsets
	///
	InitMembers();
	bool success;
	success = fDsssd3->variables.set(odb);
	success = fDsssd5->variables.set(odb);
	if(!success) {
		delete fDsssd3;
		delete fDsssd5;
		fDsssd3 = 0;
		fDsssd5 = 0;
	}
}

dragon::DsssdCalibrate::DsssdCalibrate(const char* odbfile)
{
	///
	/// \param odbfile Path of an XML file specifying the calibration slopes and offsets.
	///  See the file examples/dsssd_variables for the needed XML format (or use a copy of
	///  one of the XML files generated by MIDAS).
	///
	InitMembers();
	midas::Database odb(odbfile);
	bool success = !(odb.IsZombie());
	if(success) success = fDsssd3->variables.set(&odb);
	if(success) success = fDsssd5->variables.set(&odb);
	if(!success) {
		delete fDsssd3;
		delete fDsssd5;
		fDsssd3 = 0;
		fDsssd5 = 0;
	}
}

void dragon::DsssdCalibrate::InitMembers()
{
	fOut    = 0;
	fT3     = 0;
	fT5     = 0;
	fChain5 = 0;
	fDsssd3 = new dragon::Dsssd();
	fDsssd5 = new dragon::Dsssd();
}

void dragon::DsssdCalibrate::Init(TTree *tree)
{
	/// Set branch addresses, etc.
	///
	if(!strcmp(tree->GetName(), "t3")) {
		fChain = tree;
		fChain->SetMakeClass(1);
		fChain->SetBranchAddress("v785[2]", fAdc3, &fAdcBranch3);
	} else if(!strcmp(tree->GetName(), "t5")) {
		fChain5 = tree;
		fChain5->SetMakeClass(1);
		fChain5->SetBranchAddress("tail.v785[2]", fAdc5, &fAdcBranch5);
	} else {
		Abort(Form("Invalid Tree: \"%s\"", tree->GetName()));
	}
}

///
void dragon::DsssdCalibrate::Begin(TTree*)
{
	/// Empty
	///
	;
}


void dragon::DsssdCalibrate::ProcessEntry(TTree* tout, Dsssd* dsssd, vme::V792* adc)
{
	vme::V1190 tdc; // dummy

	dsssd->read_data(adc, tdc);
	dsssd->calculate();
	tout->Fill();
}

Bool_t dragon::DsssdCalibrate::Process(Long64_t entry)
{
	/// Get entry in the current tree, calculate calibrated DSSSD data,
	/// fill output tree. Do this for both singles and coincidences.
	///
	if(fChain == 0  || fChain5 == 0 ||
		 fT3    == 0  || fT5     == 0 ||
		 fDsssd3 == 0 || fDsssd5 == 0)
		Abort("fChain == 0  || fChain5 == 0 ||"
					"fT3    == 0  || fT5     == 0 ||"
					"fDsssd3 == 0 || fDsssd5 == 0");

	fAdcBranch3->GetEntry(entry);
	ProcessEntry(fT3, fDsssd3, fAdc3);

	if(entry < fChain5->GetEntries()) {
		fAdcBranch5->GetEntry(entry);
		ProcessEntry(fT5, fDsssd5, fAdc5);
	}

	return kTRUE;
}

Bool_t dragon::DsssdCalibrate::Notify()
{
	/// Switch output to a new file
	///
	if(!fDsssd3 || !fDsssd5)
		Abort("!fDsssd3 || !fDsssd5");

	if(fT3 && fT5 && fOut) {
		fT3->AutoSave();
		fT5->AutoSave();
		fT3->ResetBranchAddresses();
		fT5->ResetBranchAddresses();
		fOut->Close();
	}

	TFile* fcurr = fChain->GetCurrentFile();
	if(fcurr) {
		TString fname = fcurr->GetName();
		fname.ReplaceAll(".root", "_dsssd_recal.root");
		fOut = new TFile(fname, "recreate");

		fChain5 = (TTree*)fcurr->Get("t5");
		Init(fChain5);

		fT3 = new TTree("t3_dsssdcal", "DSSSD re-calibration (singles)");
		fT3->Branch("dsssd", "dragon::Dsssd", &fDsssd3);

		fT5 = new TTree("t5_dsssdcal", "DSSSD re-calibration (coinc)");
		fT5->Branch("dsssd", "dragon::Dsssd", &fDsssd5);

		utils::Info("DsssdCalibrate")
			<< "Switching to output file \"" << fname << "\"";
	}

	return kTRUE;
}

void dragon::DsssdCalibrate::Terminate()
{
	/// Close open files, save trees, etc.
	///
	if(fOut && fT3 && fT5) {
		fT3->AutoSave();
		fT3->ResetBranchAddresses();
		fT5->AutoSave();
		fT5->ResetBranchAddresses();
		fOut->Close();
		fOut = 0;
		fT3  = 0;
		fT5  = 0;
		std::cout << "Done!\n";
	}
}

dragon::DsssdCalibrate::~DsssdCalibrate()
{
	/// Close any open files and delete DSSSD instances
	///
	Terminate();
	if(fDsssd3) {
		delete fDsssd3;
		fDsssd3 = 0;
	}
	if(fDsssd5) {
		delete fDsssd5;
		fDsssd5 = 0;
	}
}
