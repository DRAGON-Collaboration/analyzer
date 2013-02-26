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
public:
	enum Etype { ONLINE, OFFLINE, NONE };
private:
	/// Return code for online event requests.
	INT fRequestId;

	/// MIDAS shared memory buffer handle
	HNDLE fBufferHandle;

  /// Storage buffer for online events
	char* fBuffer;

	/// Size of the storage buffer
	size_t fBufferSize;

	/// Flag for truncated MIDAS events
	Bool_t fIsTruncated;
	
	/// Offline MIDAS file.
	TMidasFile fFile;

	/// Type code (online or offline)
	Int_t fType;

public:
	/// Sets fIsTruncated to false, and allocates the internal buffer
	MidasBuffer(size_t size = 102400);

	/// Frees fBuffer
	virtual ~MidasBuffer();

	/// Opens an offline MIDAS file
	virtual Bool_t OpenFile(const char* file_name, char** other = 0, int nother = 0);

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
	virtual void CloseFile();

private:
	/// Disallow copy
	MidasBuffer(const MidasBuffer&) {  }

	/// Disallow assign
	MidasBuffer& operator= (const MidasBuffer&) { return *this; }

	/// Read variables from the ODB
	void ReadOdb();
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
