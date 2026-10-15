/* @file chassis_communicate.c
 * @author OriTwil
 * @brief 遥控器库函数&给遥控器发dashboard
 * @date 2023-5-18
 *
 */
#include "ashining_as69.h"

mavlink_joystick_air_t msg_joystick_air;
mavlink_joystick_air_led_t msg_joystick_air_led;

mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_point;
mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_state;
mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_posture;
mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_knob_r;

mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_point;
mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_state;
mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_posture;
mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_knob_r;

mavlink_joystick_air_dashboard_del_t msg_joystick_air_delete;

char title_point[20]   = "point";
char title_state[20]   = "state";
char title_posture[20] = "posture";
char title_knob_r[20]  = "mic_adjust_v";
char msg_point[20]     = "no_msg";
char msg_state[20]     = "no_msg";
char msg_posture[20]   = "no_msg";
char msg_knob_r[20]    = "no_msg";

float last_control_vw = 0;
#define ID_Point   6
#define ID_State   8
#define ID_Posture 16
#define ID_Knob_R  3

/***********************************用户函数段****************************************/

/**
 * @brief   遥控器初始化
 * @param
 */
void AS69_RemoteControl_Init()
{
    wtrMavlink_BindChannel(&AS69_UART_HANDLE, MAVLINK_COMM_0); // MAVLINK遥控器
    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0);                 // 以mavlink接收遥控器
}

/**
 * @brief   遥控器发送消息函数
 * @param
 */
void AS69_RemoteControl_SendMsg(mavlink_joystick_air_dashboard_set_msg_t *msg_joystick_air_msg)
{
    mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_dashboard_set_msg_temp = *msg_joystick_air_msg;

    mavlink_msg_joystick_air_dashboard_set_msg_send_struct(MAVLINK_COMM_0, &msg_joystick_air_dashboard_set_msg_temp);

#if (USE_FREERTOS == 1)
    osDelay(25);
#else
    HAL_Delay(25);
#endif
}

/**
 * @brief   遥控器LED灯信息发送函数
 */
void JoystickSwitchLED(float r, float g, float b, float lightness, uint16_t duration, mavlink_joystick_air_led_t *msg_joystick_air_led)
{
    msg_joystick_air_led->r                           = r;
    msg_joystick_air_led->g                           = g;
    msg_joystick_air_led->b                           = b;
    msg_joystick_air_led->lightness                   = lightness;
    msg_joystick_air_led->duration                    = duration;
    mavlink_joystick_air_led_t msg_joystick_send_temp = *msg_joystick_air_led;

    mavlink_msg_joystick_air_led_send_struct(MAVLINK_COMM_0, &msg_joystick_send_temp);
#if (USE_FREERTOS == 1)
    osDelay(2);
#else
    HAL_Delay(2);
#endif
}

/**
 * @brief   遥控器Title信息发送函数
 */
void JoystickSwitchTitle(uint8_t id, char title[20], mavlink_joystick_air_dashboard_set_title_t *msg_joystick_air_title)
{

    msg_joystick_air_title->id = id;
    strncpy(msg_joystick_air_title->title, title, 20);
    mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_temp = *msg_joystick_air_title;

    mavlink_msg_joystick_air_dashboard_set_title_send_struct(MAVLINK_COMM_0, &msg_joystick_air_title_temp);
#if (USE_FREERTOS == 1)
    osDelay(25);
#else
    HAL_Delay(25);
#endif
}

/**
 * @brief   遥控器Msg信息发送函数
 */
void JoystickSwitchMsg(uint8_t id, char message[20], mavlink_joystick_air_dashboard_set_msg_t *msg_joystick_air_msg)
{
    msg_joystick_air_msg->id = id;
    strncpy(msg_joystick_air_msg->message, message, 20);
}

/**
 * @brief   遥控器Del信息发送函数
 */
void JoystickDelete(uint8_t id, mavlink_joystick_air_dashboard_del_t *msg_joystick_air_delete)
{
    msg_joystick_air_delete->id                                       = id;
    mavlink_joystick_air_dashboard_del_t Msg_joystick_air_delete_temp = *msg_joystick_air_delete;

    mavlink_msg_joystick_air_dashboard_del_send_struct(MAVLINK_COMM_0, &Msg_joystick_air_delete_temp);
}

/**
 * @brief   遥控器Button信息解码函数
 */
bool ReadJoystickButtons(mavlink_joystick_air_t *msg_joystick_air_, KEYS index)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return ((msg_joystick_air_temp.buttons >> (index - 1)) & 1);
}

/**
 * @brief   遥控器左侧横摇杆信息解码函数
 */
float ReadJoystickLeft_x(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickL[0];
}

/**
 * @brief   遥控器左侧纵摇杆信息解码函数
 */
float ReadJoystickLeft_y(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickL[1];
}

/**
 * @brief   遥控器右侧横摇杆信息解码函数
 */
float ReadJoystickRight_x(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickR[0];
}

/**
 * @brief   遥控器右侧纵摇杆信息解码函数
 */
float ReadJoystickRight_y(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.joystickR[1];
}

/**
 * @brief   遥控器左侧旋轮信息解码函数
 */
int16_t ReadJoystickKnobsLeft(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return msg_joystick_air_temp.knobs[0];
}

/**
 * @brief   遥控器右侧侧旋轮信息解码函数
 */
int16_t ReadJoystickKnobsRight(mavlink_joystick_air_t *msg_joystick_air_)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif

    return msg_joystick_air_temp.knobs[1];
}

/**
 * @brief   遥控器开关信息解码函数
 */
bool ReadJoystickSwitchs(mavlink_joystick_air_t *msg_joystick_air_, SWITCHS index)
{
#if (USE_FREERTOS == 1)
    taskENTER_CRITICAL(); 
#endif
    mavlink_joystick_air_t msg_joystick_air_temp = *msg_joystick_air_;
#if (USE_FREERTOS == 1)
    taskEXIT_CRITICAL();
#endif
    return ((msg_joystick_air_temp.switchs >> index) & 1);
}

