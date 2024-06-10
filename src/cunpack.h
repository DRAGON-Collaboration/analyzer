#ifndef CUNPACK_H
#define CUNPACK_H
#include <vector>
#include <string>

const int READ_EVENT = 0;
const int READ_EOF   = 0xffff;
const int READ_DB0   = 2;
const int READ_DB1   = 3;

namespace pybind11 {
class list;
}
extern int setup(pybind11::list);
extern pybind11::list read_event();
extern int flush_queue();
extern void close_file();

typedef struct {

	unsigned int midas_fTimeStamp;
	unsigned int io32_busy_time;
	double io32_trig_time;

	double bgo_ecal[30];
	double bgo_tcal[30];
	double bgo_sum;
	int    bgo_hit0;
	double bgo_x0;
	double bgo_y0;
	double bgo_z0;
	double bgo_t0;

	double trf_leading[5];
	double trf_trailing[5];

	double tcal0;
	double tcalx;
	
} dragon_head;


#endif
