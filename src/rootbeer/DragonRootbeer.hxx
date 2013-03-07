/// \file DragonRootbeer.hxx
/// \author G. Christian
/// \brief Fake header file to get Doxygen to generate documentation
/// of functions declared in ROOTBEER headers.


#ifdef FOR_DOXYGEN_ONLY


namespace rb {

//! ABC for defining how to obtain and unpack data buffers.
/*!
 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_buffer_source.html :
 *
 * By creating a class derived from this one, users can define
 * how to connect (disconnect) to (from) an offline or online data source, how to recieve incoming
 * data buffers, and how to unpack those buffers into user-defined classes.  All of the
 * non-static member functions are pure virtual and must be implemented in derived classes.  See the
 * documentation of individual functions for an explanation of what each should do.
 */
class BufferSource {
public:
  /// Required implementation of rb::BufferSource::New()
	BufferSource* New();
};

//! Class that runs the interactive ROOT application.
/*!
 * From http://trshare.triumf.ca/~gchristian/rootbeer/doc/html/classrb_1_1_rint.html :
 *
 * We can essentially use the normal <tt>TRint</tt>, except
 * we need to override the Terminate() method to stop threaded processes.
 */
class Rint {
public:
  /// Required implementation of rb::Rint::RegisterEvents()
	void RegisterEvents();
};

}


#endif
