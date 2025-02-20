#include "DJI.h"
#include "wtr_can.h"

#define LAST 0
#define NOW  1

DJI_t hDJI[8]; // DJI电机全局数组

/**
 * @brief	DJI电机初始化函数
 * @note    使用前应对全局变量hDJI进行类型初始化
 */
void DJI_Init()
{
    for (int i = 0; i < 8; i++) {
        // 速度环PID
        hDJI[i].speedPID.KP        = 12;
        hDJI[i].speedPID.KI        = 0.2;
        hDJI[i].speedPID.KD        = 5;
        hDJI[i].speedPID.outputMax = 8000;

        // 位置环PID
        hDJI[i].posPID.KP        = 80.0f;
        hDJI[i].posPID.KI        = 1.0f;
        hDJI[i].posPID.KD        = 0.0f;
        hDJI[i].posPID.outputMax = 5000;
        // hDJI[i].posPID.outputMin = 1500;

        if (hDJI[i].motorType == M3508) {
            hDJI[i].reductionRate = 100.0f; // 2006减速比为36 3508减速比约为19
        } else if (hDJI[i].motorType == M2006) {
            hDJI[i].reductionRate = 36.0f;
        }

        hDJI[i].encoder_resolution = 8192.0f;
    }
}

/**
 * @brief	DJI电机数据更新函数
 * @param	motor	电机句柄
 * @param	fdbData	反馈值
 */
void DJI_Update(DJI_t *motor, uint8_t *fdbData)
{
    /* 反馈信息计算 */
    motor->FdbData.RotorAngle_0_360 = (fdbData[0] << 8 | fdbData[1]) * 360.0f / motor->encoder_resolution; /* unit:degree*/
    motor->FdbData.rpm              = (int16_t)(fdbData[2] << 8 | fdbData[3]);                             /* unit:rpm   */
    motor->FdbData.current          = (int16_t)(fdbData[4] << 8 | fdbData[5]);
    /* 计算数据处理 */
    /* 更新反馈速度/位置 */
    motor->Calculate.RotorAngle_0_360_Log[LAST] = motor->Calculate.RotorAngle_0_360_Log[NOW];
    motor->Calculate.RotorAngle_0_360_Log[NOW]  = motor->FdbData.RotorAngle_0_360;
    /* 电机圈数更新 */
    if (motor->Calculate.RotorAngle_0_360_Log[NOW] - motor->Calculate.RotorAngle_0_360_Log[LAST] > (180.0f))
        motor->Calculate.RotorRound--;
    else if (motor->Calculate.RotorAngle_0_360_Log[NOW] - motor->Calculate.RotorAngle_0_360_Log[LAST] < -(180.0f))
        motor->Calculate.RotorRound++;
    /* 电机输出轴角度 */
    motor->AxisData.AxisAngle_inDegree = motor->Calculate.RotorRound * 360.0f;
    motor->AxisData.AxisAngle_inDegree += motor->Calculate.RotorAngle_0_360_Log[NOW] - motor->Calculate.RotorAngle_0_360_OffSet;
    motor->AxisData.AxisAngle_inDegree /= motor->reductionRate;

    motor->AxisData.AxisVelocity    = motor->FdbData.rpm / motor->reductionRate;
    motor->Calculate.RotorAngle_all = motor->Calculate.RotorRound * 360 + motor->Calculate.RotorAngle_0_360_Log[NOW] - motor->Calculate.RotorAngle_0_360_OffSet;
}

/**
 * @brief	DJI储存偏差函数
 * @param	motor	电机句柄
 * @param	fdbData	反馈值
 */
void get_dji_offset(DJI_t *motor, uint8_t *fdbData)
{
    motor->FdbData.RotorAngle_0_360             = (fdbData[0] << 8 | fdbData[1]) * 360.0f / motor->encoder_resolution;
    motor->Calculate.RotorAngle_0_360_Log[LAST] = motor->FdbData.RotorAngle_0_360;
    motor->Calculate.RotorAngle_0_360_Log[NOW]  = motor->Calculate.RotorAngle_0_360_Log[LAST];

    motor->Calculate.RotorAngle_0_360_OffSet = motor->FdbData.RotorAngle_0_360;
}

/**
 * @brief	DJI电调数据解码函数
 * @param	Stdid	CAN数据帧ID
 * @param	fdbData	反馈值
 */
HAL_StatusTypeDef DJI_CanMsgDecode(uint32_t Stdid, uint8_t *fdbData)
{
    int i = Stdid - 0x201;
    if (i >= 0 && i < 8) {
        // 上电后进行反馈数据计数，计数到第50次时设置为当前偏差
        if (hDJI[i].FdbData.msg_cnt < 50) {
            get_dji_offset(&hDJI[i], fdbData);
            hDJI[i].FdbData.msg_cnt++;
        } else {
            DJI_Update(&hDJI[i], fdbData);
        }
        return HAL_OK;
    }
    return HAL_ERROR;
}

#if (STM32F427xx)
/**
 * @brief	DJI电机CAN发送函数，电机ID 1/2/3/4
 * @param	hcanx	CAN句柄
 * @param	cmx_iq	发送值，通常为最内环控制的输出值
 */
void CanTransmit_DJI_1234(CAN_HandleTypeDef *hcanx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
    CAN_TxHeaderTypeDef TxMessage;

    TxMessage.DLC   = 0x08;
    TxMessage.StdId = 0x200;
    TxMessage.IDE   = CAN_ID_STD;
    TxMessage.RTR   = CAN_RTR_DATA;

    uint8_t TxData[8];
    TxData[0] = (uint8_t)(cm1_iq >> 8);
    TxData[1] = (uint8_t)cm1_iq;
    TxData[2] = (uint8_t)(cm2_iq >> 8);
    TxData[3] = (uint8_t)cm2_iq;
    TxData[4] = (uint8_t)(cm3_iq >> 8);
    TxData[5] = (uint8_t)cm3_iq;
    TxData[6] = (uint8_t)(cm4_iq >> 8);
    TxData[7] = (uint8_t)cm4_iq;
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcanx) == 0) // 等待邮箱空闲
        ;
    if (HAL_CAN_AddTxMessage(hcanx, &TxMessage, TxData, &TxMailbox) != HAL_OK) {
        Error_Handler(); // 如果CAN信息发送失败则进入死循环
    }
}

/**
 * @brief	DJI电机CAN发送函数，电机ID 5/6/7/8
 * @param	hcanx	CAN句柄
 * @param	cmx_iq	发送值，通常为最内环控制的输出值
 */
void CanTransmit_DJI_5678(CAN_HandleTypeDef *hcanx, int16_t cm5_iq, int16_t cm6_iq, int16_t cm7_iq, int16_t cm8_iq)
{
    CAN_TxHeaderTypeDef TxMessage;

    TxMessage.DLC   = 0x08;
    TxMessage.StdId = 0x1FF;
    TxMessage.IDE   = CAN_ID_STD;
    TxMessage.RTR   = CAN_RTR_DATA;

    uint8_t TxData[8];
    TxData[0] = (uint8_t)(cm5_iq >> 8);
    TxData[1] = (uint8_t)cm5_iq;
    TxData[2] = (uint8_t)(cm6_iq >> 8);
    TxData[3] = (uint8_t)cm6_iq;
    TxData[4] = (uint8_t)(cm7_iq >> 8);
    TxData[5] = (uint8_t)cm7_iq;
    TxData[6] = (uint8_t)(cm8_iq >> 8);
    TxData[7] = (uint8_t)cm8_iq;

    while (HAL_CAN_GetTxMailboxesFreeLevel(hcanx) == 0)
        ;
    if (HAL_CAN_AddTxMessage(hcanx, &TxMessage, TxData, &TxMailbox) != HAL_OK) {
        Error_Handler(); // 如果CAN信息发送失败则进入死循环
    }
}

#endif
#if (STM32H723xx)
/**
 * @brief	DJI电机CAN发送函数，电机ID 1/2/3/4
 * @param	cmx_iq	发送值，通常为最内环控制的输出值
 */
void CanTransmit_DJI_1234(int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
    CAN_MSG umsg;

    umsg.id  = 0x200;
    umsg.len = 0x08;
    umsg.rtr = DATA_FRAME;

    umsg.buffer[0] = (uint8_t)(cm1_iq >> 8);
    umsg.buffer[1] = (uint8_t)cm1_iq;
    umsg.buffer[2] = (uint8_t)(cm2_iq >> 8);
    umsg.buffer[3] = (uint8_t)cm2_iq;
    umsg.buffer[4] = (uint8_t)(cm3_iq >> 8);
    umsg.buffer[5] = (uint8_t)cm3_iq;
    umsg.buffer[6] = (uint8_t)(cm4_iq >> 8);
    umsg.buffer[7] = (uint8_t)cm4_iq;

    fdcan1.FDCAN_Send_MSG(&umsg);
}

/**
 * @brief	DJI电机CAN发送函数，电机ID 5/6/7/8
 * @param	cmx_iq	发送值，通常为最内环控制的输出值
 */
void CanTransmit_DJI_5678(int16_t cm5_iq, int16_t cm6_iq, int16_t cm7_iq, int16_t cm8_iq)
{
    CAN_MSG umsg;

    umsg.len = 0x08;
    umsg.id  = 0x1FF;
    umsg.rtr = DATA_FRAME;

    umsg.buffer[0] = (uint8_t)(cm5_iq >> 8);
    umsg.buffer[1] = (uint8_t)cm5_iq;
    umsg.buffer[2] = (uint8_t)(cm6_iq >> 8);
    umsg.buffer[3] = (uint8_t)cm6_iq;
    umsg.buffer[4] = (uint8_t)(cm7_iq >> 8);
    umsg.buffer[5] = (uint8_t)cm7_iq;
    umsg.buffer[6] = (uint8_t)(cm8_iq >> 8);
    umsg.buffer[7] = (uint8_t)cm8_iq;

    fdcan1.FDCAN_Send_MSG(&umsg);
}
#endif