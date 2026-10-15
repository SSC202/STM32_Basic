/**
 * \addtogroup LAN9252_Device LAN9252_Device
 * @{
 */

/**
\file LAN9252_Device.h
\brief LAN9252_Device function prototypes and defines

\version 1.0.0.11
 */

 
 #ifndef _LAN9252__DEVICE_H_
#define _LAN9252__DEVICE_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "ecatappl.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#endif //_LAN9252__DEVICE_H_

//include custom application object dictionary 
#include "LAN9252_DeviceObjects.h"


#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif


PROTO void APPL_Application(void);
#if EXPLICIT_DEVICE_ID
PROTO UINT16 APPL_GetDeviceID(void);
#endif

PROTO void   APPL_AckErrorInd(UINT16 stateTrans);
PROTO UINT16 APPL_StartMailboxHandler(void);
PROTO UINT16 APPL_StopMailboxHandler(void);
PROTO UINT16 APPL_StartInputHandler(UINT16 *pIntMask);
PROTO UINT16 APPL_StopInputHandler(void);
PROTO UINT16 APPL_StartOutputHandler(void);
PROTO UINT16 APPL_StopOutputHandler(void);

PROTO UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize);
PROTO void APPL_InputMapping(UINT16* pData);
PROTO void APPL_OutputMapping(UINT16* pData);

#undef PROTO
/** @}*/

