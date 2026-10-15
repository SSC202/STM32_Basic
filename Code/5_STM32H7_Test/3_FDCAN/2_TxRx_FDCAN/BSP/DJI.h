#ifndef DJI_H
#define DJI_H

#ifdef __cplusplus
extern "C" {
#endif
#if (STM32F427xx)
#include "can.h"
#endif
#if (STM32H723xx)
#include "fdcan.h"
#endif

/***************************结构体定义**********************************/
typedef struct
{
    float KP;        // PID参数P
    float KI;        // PID参数I
    float KD;        // PID参数D
    float fdb;       // PID反馈值
    float ref;       // PID目标值
    float cur_error; // 当前误差
    float error[2];  // 前两次误差
    float output;    // 输出值
    float outputMax; // 最大输出值的绝对值
    float outputMin; // 最小输出值的绝对值用于防抖
} PID_t;

typedef enum {
    M3508 = 0,
    M2006
} DJI_e;

typedef struct
{
    DJI_e motorType;

    struct
    {
        int msg_cnt;            // 收到消息数
        float RotorAngle_0_360; // 电机转子角度 单位 度° 范围0~360
        float rpm;              // 电机转子速度，单位 rpm
        float current;          // 电机转矩电流
    } FdbData;

    struct
    {
        float RotorAngle_all;          // 电机总角度 单位 度° 范围-inf~inf
        float RotorAngle_0_360_Log[2]; // NOW->电机当前位置0~360 LAST->电机上一次更新位置0~360
        float RotorAngle_0_360_OffSet; // 电机机械角度偏移量 单位 度°
        int RotorRound;                // 电机转的圈数
    } Calculate;
    struct
    {
        float AxisAngle_inDegree; // 电机轴输出角度 单位 度°
        float AxisVelocity;       // 电机轴输出速度 单位 rpm
    } AxisData;

    float reductionRate;      // 减速比
    float encoder_resolution; // 编码器分辨率

    PID_t speedPID;
    PID_t posPID;
} DJI_t;

/***************************用户变量定义**********************************/
extern DJI_t hDJI[8];
/***************************用户函数定义**********************************/
#if (STM32F427xx)
void CanTransmit_DJI_1234(CAN_HandleTypeDef *hcanx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
void CanTransmit_DJI_5678(CAN_HandleTypeDef *hcanx, int16_t cm5_iq, int16_t cm6_iq, int16_t cm7_iq, int16_t cm8_iq);
#endif
#if (STM32H723xx)
void CanTransmit_DJI_1234(int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq);
void CanTransmit_DJI_5678(int16_t cm5_iq, int16_t cm6_iq, int16_t cm7_iq, int16_t cm8_iq);
#endif
void DJI_Init(void);
HAL_StatusTypeDef DJI_CanMsgDecode(uint32_t Stdid, uint8_t *fdbData);

#ifdef __cplusplus
}
#endif

#endif
