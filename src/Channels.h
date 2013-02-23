/*!
 * \file Channels.h
 * \author G. Christian
 * \brief Sets \e default channel mappings
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

#define BGO_ADC0         0  /**< Bgo q: Ch 0..30 */
#define MCP_ADC0         0  /**< Mcp anode: Ch 0..3 */
#define MCP_TAC_ADC0     4  /**< Mcp TAC: Ch 4 */
#define SB_ADC0          5  /**< Surface barriers: Ch 5..6 */
#define IC_ADC0          7  /**< Ion-chamber anodes: Ch 7..10 */
#define NAI_ADC0         11 /**< NaI signals: Ch 11..12 */
#define GE_ADC0          13 /**< HPGe signal: Ch 13 */
#define DSSSD_ADC0       0  /**< DSSSD: Ch 0..31 [own module] */

#define DEFAULT_HI_MODULE  0  /**< Default ADC for heavy-ion detectors */
#define DSSSD_MODULE       1  /**< DSSSD gets it's own ADC */

#define BGO_TDC0         0  /**< Bgo times: Ch 0..30 */
#define MCP_TDC0         0  /**< Mcp times: Ch 0..1 */
#define DSSSD_TDC0       2  /**< DSSSD time: Ch 2 */
#define IC_TDC0          3  /**< Ion-chamber time: Ch 3 */

#define HEAD_CROSS_TDC    31
#define TAIL_CROSS_TDC    4


#endif
