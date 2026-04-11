#ifndef __ETHERCATUSER_H
#define __ETHERCATUSER_H

#include "ethercat.h"
#include "osal.h"

PACKED_BEGIN
typedef struct PACKED
{
	uint16 d1;
	uint16 d2;
}PDO_Outputs;
PACKED_END

PACKED_BEGIN
typedef struct PACKED
{
	uint16 s1;
	uint16 s2;
}PDO_Inputs;
PACKED_END

void ecat_init(void);
void ecat_loop(void);

#endif
