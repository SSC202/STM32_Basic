/**
* \addtogroup LAN9252_Device LAN9252_Device
* @{
*/

/**
\file LAN9252_DeviceObjects
\author ET9300Utilities.ApplicationHandler (Version 1.3.6.0) | EthercatSSC@beckhoff.com

\brief LAN9252_Device specific objects<br>
\brief NOTE : This file will be overwritten if a new object dictionary is generated!<br>
*/

#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
#define PROTO
#else
#define PROTO extern
#endif
/******************************************************************************
*                    Object 0x1600 : Led process data mapping
******************************************************************************/
/**
* \addtogroup 0x1600 0x1600 | Led process data mapping
* @{
* \brief Object 0x1600 (Led process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Reference to 0x7000.1<br>
* SubIndex 2 - Reference to 0x7000.2<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1600[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x7000.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex2 - Reference to 0x7000.2 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1600[] = "Led process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x7000.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x7000.2 */
} OBJ_STRUCT_PACKED_END
TOBJ1600;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1600 LedProcessDataMapping0x1600
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={2,0x70000110,0x70000210}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1A00 : Switch process data mapping
******************************************************************************/
/**
* \addtogroup 0x1A00 0x1A00 | Switch process data mapping
* @{
* \brief Object 0x1A00 (Switch process data mapping) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - Reference to 0x6000.1<br>
* SubIndex 2 - Reference to 0x6000.2<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1A00[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }, /* Subindex1 - Reference to 0x6000.1 */
{ DEFTYPE_UNSIGNED32 , 0x20 , ACCESS_READ }}; /* Subindex2 - Reference to 0x6000.2 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x1A00[] = "Switch process data mapping\000"
"SubIndex 001\000"
"SubIndex 002\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT32 SI1; /* Subindex1 - Reference to 0x6000.1 */
UINT32 SI2; /* Subindex2 - Reference to 0x6000.2 */
} OBJ_STRUCT_PACKED_END
TOBJ1A00;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1A00 SwitchProcessDataMapping0x1A00
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={2,0x60000110,0x60000210}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C12 : SyncManager 2 assignment
******************************************************************************/
/**
* \addtogroup 0x1C12 0x1C12 | SyncManager 2 assignment
* @{
* \brief Object 0x1C12 (SyncManager 2 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C12[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C12[] = "SyncManager 2 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C12;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C12 sRxPDOassign
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={1,{0x1600}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x1C13 : SyncManager 3 assignment
******************************************************************************/
/**
* \addtogroup 0x1C13 0x1C13 | SyncManager 3 assignment
* @{
* \brief Object 0x1C13 (SyncManager 3 assignment) definition
*/
#ifdef _OBJD_
/**
* \brief Entry descriptions<br>
* 
* Subindex 0<br>
* Subindex 1 - n (the same entry description is used)<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x1C13[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READ }};

/**
* \brief Object name definition<br>
* For Subindex 1 to n the syntax 'Subindex XXX' is used
*/
OBJCONST UCHAR OBJMEM aName0x1C13[] = "SyncManager 3 assignment\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16   u16SubIndex0;  /**< \brief Subindex 0 */
UINT16 aEntries[1];  /**< \brief Subindex 1 - 1 */
} OBJ_STRUCT_PACKED_END
TOBJ1C13;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ1C13 sTxPDOassign
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={1,{0x1A00}}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x6000 : Switch
******************************************************************************/
/**
* \addtogroup 0x6000 0x6000 | Switch
* @{
* \brief Object 0x6000 (Switch) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - s1<br>
* SubIndex 2 - s2<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x6000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE | OBJACCESS_TXPDOMAPPING }, /* Subindex1 - s1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE | OBJACCESS_TXPDOMAPPING }}; /* Subindex2 - s2 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x6000[] = "Switch\000"
"s1\000"
"s2\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 S1; /* Subindex1 - s1 */
UINT16 S2; /* Subindex2 - s2 */
} OBJ_STRUCT_PACKED_END
TOBJ6000;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ6000 Switch0x6000
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={2,0,0}
#endif
;
/** @}*/



/******************************************************************************
*                    Object 0x7000 : Led
******************************************************************************/
/**
* \addtogroup 0x7000 0x7000 | Led
* @{
* \brief Object 0x7000 (Led) definition
*/
#ifdef _OBJD_
/**
* \brief Object entry descriptions<br>
* <br>
* SubIndex 0<br>
* SubIndex 1 - d1<br>
* SubIndex 2 - d2<br>
*/
OBJCONST TSDOINFOENTRYDESC    OBJMEM asEntryDesc0x7000[] = {
{ DEFTYPE_UNSIGNED8 , 0x8 , ACCESS_READ },
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING }, /* Subindex1 - d1 */
{ DEFTYPE_UNSIGNED16 , 0x10 , ACCESS_READWRITE | OBJACCESS_RXPDOMAPPING }}; /* Subindex2 - d2 */

/**
* \brief Object/Entry names
*/
OBJCONST UCHAR OBJMEM aName0x7000[] = "Led\000"
"d1\000"
"d2\000\377";
#endif //#ifdef _OBJD_

#ifndef _LAN9252__DEVICE_OBJECTS_H_
/**
* \brief Object structure
*/
typedef struct OBJ_STRUCT_PACKED_START {
UINT16 u16SubIndex0;
UINT16 D1; /* Subindex1 - d1 */
UINT16 D2; /* Subindex2 - d2 */
} OBJ_STRUCT_PACKED_END
TOBJ7000;
#endif //#ifndef _LAN9252__DEVICE_OBJECTS_H_

/**
* \brief Object variable
*/
PROTO TOBJ7000 Led0x7000
#if defined(_LAN9252__DEVICE_) && (_LAN9252__DEVICE_ == 1)
={2,0,0}
#endif
;
/** @}*/







#ifdef _OBJD_
TOBJECT    OBJMEM ApplicationObjDic[] = {
/* Object 0x1600 */
{NULL , NULL ,  0x1600 , {DEFTYPE_PDOMAPPING , 2 | (OBJCODE_REC << 8)} , asEntryDesc0x1600 , aName0x1600 , &LedProcessDataMapping0x1600, NULL , NULL , 0x0000 },
/* Object 0x1A00 */
{NULL , NULL ,  0x1A00 , {DEFTYPE_PDOMAPPING , 2 | (OBJCODE_REC << 8)} , asEntryDesc0x1A00 , aName0x1A00 , &SwitchProcessDataMapping0x1A00, NULL , NULL , 0x0000 },
/* Object 0x1C12 */
{NULL , NULL ,  0x1C12 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C12 , aName0x1C12 , &sRxPDOassign, NULL , NULL , 0x0000 },
/* Object 0x1C13 */
{NULL , NULL ,  0x1C13 , {DEFTYPE_UNSIGNED16 , 1 | (OBJCODE_ARR << 8)} , asEntryDesc0x1C13 , aName0x1C13 , &sTxPDOassign, NULL , NULL , 0x0000 },
/* Object 0x6000 */
{NULL , NULL ,  0x6000 , {DEFTYPE_RECORD , 2 | (OBJCODE_REC << 8)} , asEntryDesc0x6000 , aName0x6000 , &Switch0x6000, NULL , NULL , 0x0000 },
/* Object 0x7000 */
{NULL , NULL ,  0x7000 , {DEFTYPE_RECORD , 2 | (OBJCODE_REC << 8)} , asEntryDesc0x7000 , aName0x7000 , &Led0x7000, NULL , NULL , 0x0000 },
{NULL,NULL, 0xFFFF, {0, 0}, NULL, NULL, NULL, NULL}};
#endif    //#ifdef _OBJD_
#undef PROTO

/** @}*/
#define _LAN9252__DEVICE_OBJECTS_H_
