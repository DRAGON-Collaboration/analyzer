/******************************************//*!
 * \file definitions.h
 * \brief Defines constants used by multiple MIDAS clients
 */
#ifndef DRAGON_DEFINITIONS_H
#define DRAGON_DEFINITIONS_H


/*************************************************
 *  Define event ID codes for the dragon system. *
 *************************************************/

#define DRAGON_HEAD_EVENT      1 /*!< Dragon head event ID */
#define DRAGON_HEAD_SCALER     2 /*!< Dragon head scaler ID */
#define DRAGON_TAIL_EVENT      3 /*!< Dragon tail event ID */
#define DRAGON_TAIL_SCALER     4 /*!< Dragon tail scaler ID */
#define DRAGON_COINC_EVENT     5 /*!< Dragon coincidence event ID */
#define DRAGON_HEAD_PULSER     6 /*!< Pulser (ch 7) event ID */
#define DRAGON_CLOCK_SYNC      7 /*!< Periodic (longer) scaler readout for clock sync */

#define TRIGGER_HEAD           (1<<0)    /*!< Dragon head trigger mask */
#define TRIGGER_TAIL           (1<<1)    /*!< Dragon tail trigger mask */
#define TRIGGER_SYNC           (1<<2)    /*!< Synch event mask */

#define DRAGON_SCALER_READ_PERIOD 1000  /*!< Scaler readout period in milliseconds */
#define DRAGON_HEAD_PULSER_PERIOD 10000 /*!< Pulser (ch 7) period */
#define DRAGON_CLOCK_SYNC_PERIOD  15000 /*!< Synch redout period */

#define DRAGON_TSC_FREQ 20.                        /*!< TSC clock frequency in MHz */

#define HEAD_FRONTEND_NAME            "fe_head"    /*!< frontend_name for dragon head */
#define HEAD_EQUIPMENT_NAME           "HeadVME"    /*!< MIDAS equipment name for dragon head */
#define HEAD_SCALER_EQUIPMENT_NAME    "HeadScaler" /*!< MIDAS equipment name for dragon head scaler */

#define TAIL_FRONTEND_NAME            "fe_tail"    /*!< frontend_name for dragon tail */
#define TAIL_EQUIPMENT_NAME           "TailVME"    /*!< MIDAS equipment name for dragon tail */
#define TAIL_SCALER_EQUIPMENT_NAME    "TailScaler" /*!< MIDAS equipment name for dragon tail scaler */

#define HEAD_ADC_ADDR { 0x110000, 0 }              /*!< Addresses of head ADC modules; last entry always 0 */
#define HEAD_TDC_ADDR { 0xE00000, 0 }              /*!< Addresses of head TDC modules; last entry always 0 */
#define HEAD_CFD_ADDR { 0x800000, 0x810000, 0 }    /*!< Addresses of head CFD modules; last entry always 0 */
#define HEAD_IO32_ADDR 0x100000                    /*!< Address of head IO32 module */

#define TAIL_ADC_ADDR { 0x120000, 0x130000, 0 }    /*!< Addresses of tail ADC modules; last entry always 0 */
#define TAIL_TDC_ADDR { 0xE10000, 0 }              /*!< Addresses of tail TDC modules; last entry always 0 */
#define TAIL_CFD_ADDR                              /*!< Addresses of tail CFD modules; last entry always 0 */
#define TAIL_IO32_ADDR 0x100000                    /*!< Address of tail IO32 module */


#endif
