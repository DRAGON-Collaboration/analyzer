//! \file Incrvoid.hxx
//! \brief Defines a function to increment a void pointer based
//! on the data type it points to.
#include "utils/Error.hxx"

namespace {

void increment_void(void*& begin, int midas_data_type)
{
	int data_sizes[] = {
		0, // From midas.h																									
		1, // TID_BYTE      1   /**< unsigned byte         0       255    */
		1, // TID_SBYTE     2   /**< signed byte         -128      127    */
		1, // TID_CHAR      3   /**< single character      0       255    */
		2, // TID_WORD      4   /**< two bytes             0      65535   */
		2, // TID_SHORT     5   /**< signed word        -32768    32767   */
		4, // TID_DWORD     6   /**< four bytes            0      2^32-1  */
		4, // TID_INT       7   /**< signed dword        -2^31    2^31-1  */
		4, // TID_BOOL      8   /**< four bytes bool       0        1     */
		4, // TID_FLOAT     9   /**< 4 Byte float format                  */
		8	 // TID_DOUBLE   10   /**< 8 Byte float format                  */
	};

	if (midas_data_type < 1 || midas_data_type > 10) {
		err::Throw("increment_void") << "Unknown midas data type: " << midas_data_type << "\n";
	}

  begin = reinterpret_cast<char*>(begin) + data_sizes[midas_data_type];
}

}
