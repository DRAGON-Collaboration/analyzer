/*!
 * \file Defaults.hxx
 * \author G. Christian
 * \brief Sets \e default channel mappings and other variables.
 * \details
 * \code
 *** Sketch of channel mappings ***

 ** HEAVY-ION **

 ADC0
 0..3 = MCP_ANODE[]
 4    = MCP_TAC
 5..6 = SB
 7..10 = IC_ANODE[]
 11..12 = NaI[]
 13 = Ge

 ADC1
 0..31 = DSSSD_E[]


 TDC
 0..1 = MCP
 2 = DSSSD
 3 = IC
 4 = CROSS TRIGGER

 ** GAMMA **
ADC
0..30 = bgo[]

TDC
0..30 = bgo[]
31 = CROSS_TRIGGER

 \endcode
*/
#ifndef DRAGON_CHANNELS_H
#define DRAGON_CHANNELS_H

#define HEAD_IO32_BANK  "VTRH" /**< Head IO32 bank name */
#define HEAD_TSC_BANK   "TSCH" /**< Head TSC bank name  */
#define HEAD_ADC_BANK   "ADC0" /**< Head ADC bank name  */
#define HEAD_TDC_BANK   "TDC0" /**< Head TDC bank name  */

#define TAIL_IO32_BANK  "VTRT" /**< Tail IO32 bank name */
#define TAIL_TSC_BANK   "TSCT" /**< Tail TSC bank name  */
#define TAIL_ADC_BANK_0 "TLQ0" /**< Tail ADC0 bank name  */
#define TAIL_ADC_BANK_1 "TLQ1" /**< Tail ADC1 bank name  */
#define TAIL_TDC_BANK   "TLT0" /**< Tail TDC bank name  */

#define BGO_ADC0         0  /**< Bgo q */
#define DSSSD_ADC0       0  /**< DSSSD (module 1) */
#define SB_ADC0          16 /**< Surface barriers */
#define MCP_ADC0         18 /**< Mcp anode */
#define MCP_TAC_ADC0     22 /**< Mcp TAC */
#define GE_ADC0          23 /**< HPGe signal */
#define IC_TAC_ADC0      24 /**< IC TAC */
#define IC_ADC0          25 /**< Ion-chamber anodes */
#define NAI_ADC0         30 /**< NaI signals */

#define DEFAULT_HI_MODULE  0  /**< Default ADC for heavy-ion detectors */
#define DSSSD_MODULE       1  /**< DSSSD gets it's own ADC */

#define BGO_TDC0         0  /**< Bgo times: Ch 0..30 */
#define IC_TDC0          0  /**< Ion-chamber time: Ch 3 */
#define DSSSD_TDC0       4  /**< DSSSD time: Ch 2 */
#define MCP_TDC0         6  /**< Mcp times: Ch 0..1 */

#define HEAD_RF_TDC       32 /**< RF timing */
#define HEAD_OR_TDC       33 /**< OR of all triggers */
#define HEAD_CROSS_TDC    34 /**< Crossover TDC */
#define TAIL_RF_TDC       8  /**< RF timing */
#define TAIL_OR_TDC       9	 /**< OR of all triggers */
#define TAIL_CROSS_TDC    10 /**< Crossover TDC */


#define DRAGON_DEFAULT_COINC_WINDOW        10 /** Coincidence window in usec */
#define DRAGON_DEFAULT_COINC_BUFFER_TIME   4  /** Coincidence buffering time in sec */

/** Bgo coordinates */
#define BGO_COORDS		\
	{										\
		{ 4, -4.8,-15.3},	\
		{ 0,-10.1,-12.2},	\
		{ 4,  5.0,-12.2},	\
		{ 0,  9.9, -9.2},	\
		{ 0,  8.0, -3.1},	\
		{ 0,  8.0,  3.1},	\
		{ 0,  9.9,  9.2},	\
		{ 0,-10.1, 12.2},	\
		{ 0,  5.0, 12.2},	\
		{ 0, -4.8, 15.3},	\
		{-4, -2.6, -9.2},	\
		{ 4, -2.6, -9.2},	\
		{-4, -7.9, -6.1},	\
		{ 4, -7.9, -6.1},	\
		{-4,  2.7, -6.1},	\
		{ 4,  2.7, -6.1},	\
		{-4, -2.6, -3.1},	\
		{ 4, -2.6, -3.1},	\
		{-4, -7.9,    0},	\
		{ 4, -7.9,    0},	\
		{-4,  2.7,    0},	\
		{ 4,  2.7,    0},	\
		{-4, -2.6,  3.1},	\
		{ 4, -2.6,  3.1},	\
		{-4, -7.9,  6.1},	\
		{ 4, -7.9,  6.1},	\
		{-4,  2.7,  6.1},	\
		{ 4,  2.7,  6.1},	\
		{-4, -2.6,  9.2},	\
		{ 4, -2.6,  9.2}	\
	}

#endif
