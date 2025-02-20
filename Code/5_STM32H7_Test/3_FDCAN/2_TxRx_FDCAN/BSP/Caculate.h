#ifndef _CACULATE_H__
#define _CACULATE_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "DJI.h"
    void positionServo(float ref, DJI_t *motor);
    void speedServo(float ref, DJI_t *motor);
#ifdef __cplusplus
}
#endif

#endif
