/**
  ******************************************************************************
  * @file    Dev_Inf.c
  * @author  MCD Tools Team
  * @date    October-2017
  * @brief   This file defines the structure containing informations about the 
  *          external flash memory MX25LM51245G used by ST-LINK Utility in 
  *          programming/erasing the device.
  ******************************************************************************
  */

#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
   "MX25LM51245G_STM32L4R9I-DISCO", 	 				        /* Device Name + EVAL Borad name*/
   NOR_FLASH,                   					        /* Device Type*/
   0x90000000,                						        /* Device Start Address*/
   0x2000000,                 						        /* Device Size in Bytes (32MBytes)*/
   0x10000,                    						        /* Programming Page Size 256Bytes*/
   0xFF,                       						        /* Initial Content of Erased Memory*/
   /*Specify Size and Address of Sectors (view example below)*/
   0x00000200, 0x00010000,     				 		        /* Sector Num : 512 ,Sector Size: 64KBytes*/
   0x00000000, 0x00000000,      
}; 


