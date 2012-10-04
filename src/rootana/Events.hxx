/// \file Events.hxx
/// \author G. Christian
/// \brief Defines DRAGON event handlers for rootana
#ifndef DRAGON_ROOTANA_EVENTS_HXX
#define DRAGON_ROOTANA_EVENTS_HXX
#include <stdint.h>
#include <map>
#include <list>
#include "dragon/Coinc.hxx"

class TDirectory;
namespace midas { class Event; }

namespace rootana {

class HistBase;

/// Class to process incoming midas events
class EventHandler {
private:
	/// Histogram container
	std::map<uint16_t, std::list<HistBase*> > fHistos;
private:
	/// Empty (private, use singleton instance)
	EventHandler() { }
public:
	/// Singleton instance
	static EventHandler* Instance();
	/// Empty
	~EventHandler() { }
	/// Tells how to handle an event (singles)
	void Process(const midas::Event& event);
	/// Tells how to handle an event (coincidence)
	void Process(const midas::CoincEvent& event);
	/// Adds a histogram
	void AddHisto(rootana::HistBase* hist, uint16_t eventId, TDirectory* file, const char* dirPath);
	/// Handles end-of-run
	void EndRun();
	/// Handles begin-of-run
	void BeginRun();

	size_t S() { return fHistos[1].size(); }
};

} // namespace rootana

#endif
