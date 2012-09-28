/// \file MidasBuffer.hxx
/// \author G. Christian
/// \brief Implementation of rb::BufferSource for DRAGON timestamped experiments
#ifndef DRAGON_RB_MIDASBUFFER_HXX
#define DRAGON_RB_MIDASBUFFER_HXX
#include "midas/TMidasFile.h"
#include "Buffer.hxx"

#ifdef MIDASSYS
#include <midas.h>
#else
typedef int32_t INT;
typedef INT HNDLE;
#endif 

/// Encloses all DRAGON-related rootbeer classes & functions.
namespace rootbeer {

class MidasBuffer: public rb::BufferSource
{
private:
	/// Return code for online event requests.
	INT fRequestId;

	/// MIDAS shared memory buffer handle
	HNDLE fBufferHandle;

  /// Temporary storage buffer for online events
	char fBuffer[100*1024];

	/// Flag for truncated MIDAS events
	Bool_t fIsTruncated;
	
	/// Offline MIDAS file.
	TMidasFile fFile;

public:
	/// Sets fIsTruncated to false
	MidasBuffer(): fIsTruncated(false) { }

	/// Closes file & disconnects from online stuff
	virtual ~MidasBuffer()
		{
			/*! \warning We shouldn't be callig virtual functions from a destructor.
			 *  \todo Fix virtual calls from destructor in ROOTBEER!!!! */
			// CloseFile();
			// DisconnectOnline();
		}

	/// Opens an offline MIDAS file
	virtual Bool_t OpenFile(const char* file_name, char** other = 0, int nother = 0)
		{ return fFile.Open(file_name); }

	/// Connects to an online MIDAS experiment
	virtual Bool_t ConnectOnline(const char* host, const char* other_arg = "", char** other_args = 0, int n_others = 0);

	/// Reads event buffers from an offline MIDAS file
	virtual Bool_t ReadBufferOffline();

	/// Reads event buffers from an online MIDAS experiment
	virtual Bool_t ReadBufferOnline();

	/// Specifies how to deal with various received buffer types
	virtual Bool_t UnpackBuffer();

	/// Disconnects from an online MIDAS experiment
	virtual void DisconnectOnline();

	/// Closes on offline MIDAS file
	virtual void CloseFile() { return fFile.Close(); }
};

} // namespace rootbeer


extern "C" {

/// Run stop transition handler
INT rootbeer_run_stop(INT runnum, char* err);
/// Run start transition handler
INT rootbeer_run_start(INT runnum, char* err);
/// Run pause transition handler
INT rootbeer_run_pause(INT runnum, char* err);
/// Run resume transition handler
INT rootbeer_run_resume(INT runnum, char* err);

}


#endif
