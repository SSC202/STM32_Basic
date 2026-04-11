/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

/** \file
 * \brief
 * Headerfile for oshw.c
 */

#ifndef _oshw_
#define _oshw_

#ifdef __cplusplus
extern "C"
{
#endif

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatmain.h"
#include "stm32f4xx.h"

typedef unsigned char bool;

#define htons(x) ((((x)&0xff)<<8)|(((x)&0xff00)>>8))
#define ntohs(x) htons(x)
#define htonl(x) ((((x)&0xff)<<24)| \
                 (((x)&0xff00)<<8) | \
                 (((x)&0xff0000)>>8) | \
                 (((x)&0xff000000)>>24))

#define ntohl(x) htonl(x)

uint16 oshw_htons(uint16 host);
uint16 oshw_ntohs(uint16 network);

ec_adaptert * oshw_find_adapters(void);
void oshw_free_adapters(ec_adaptert * adapter);

#ifdef __cplusplus
}
#endif

#endif
