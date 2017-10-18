/******************************************//*!
 * \file definitions.h
 * \brief Defines constants used by multiple MIDAS clients
 */
#ifndef DRAGON_DEFINITIONS_H
#define DRAGON_DEFINITIONS_H


/*************************************************
 *  Define event ID codes for the dragon system. *
 *************************************************/

#define DRAGON_HEAD_EVENT          1 /*!< Dragon head event ID */
#define DRAGON_HEAD_SCALER         2 /*!< Dragon head scaler ID */
#define DRAGON_TAIL_EVENT          3 /*!< Dragon tail event ID */
#define DRAGON_TAIL_SCALER         4 /*!< Dragon tail scaler ID */
#define DRAGON_COINC_EVENT         5 /*!< Dragon coincidence event ID */
#define DRAGON_TSTAMP_DIAGNOSTICS  6 /*!< Timestamp diagnostics event ID (analysis only) */
#define DRAGON_RUN_PARAMETERS      7 /*!< Global run parameters event ID (analysis only) */
#define DRAGON_AUX_SCALER          8 /*!< Aux scaler event ID */
#define DRAGON_EPICS_EVENT        20 /*!< EPICS event ID */
#define DRAGON_EPICS_SCALER       21 /*!< EPICS virtual PV ID */

#define DRAGON_SCALER_READ_PERIOD 1000  /*!< Scaler readout period in milliseconds */

#define DRAGON_TSC_FREQ 20.                        /*!< TSC clock frequency in MHz */

#define FE_HEAD 1 /*!< Head frontend type code */
#define FE_TAIL 3 /*!< TAIL frontend type code */

#define MIDAS_BOR  0x8000 /*!< MIDAS begin-of-run event ID*/
#define MIDAS_EOR  0x8001 /*!< MIDAS end-of-run event ID*/


#endif
