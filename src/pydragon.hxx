#ifndef PYDRAGON_HEADER_FILE_123
#define PYDRAGON_HEADER_FILE_123

const int READ_EVENT = 0;
const int READ_EOF   = 0xffff;
const int READ_DB0   = 2;
const int READ_DB1   = 3;

namespace pybind11 {
class list;
}
extern int setup(pybind11::list);
extern pybind11::list read_event();
extern pybind11::list flush_queue();
extern void close_file();


#endif
