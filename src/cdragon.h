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

	double trf.leading[5];
	double trf.trailing[5];

	double tcal0;
	double tcalx;
	
} dragon_head;
