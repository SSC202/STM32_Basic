#ifndef __ASHINING_AS69__
#define __ASHINING_AS69__

#include "wtr_mavlink.h"

#define AS69_UART_HANDLE huart8
#define USE_FREERTOS     1

#if (USE_FREERTOS == 1)
#include "cmsis_os.h"
#endif

/********************类型定义*************************/
typedef enum {
    Btn_LeftCrossUp     = 2,
    Btn_LeftCrossDown   = 1,
    Btn_LeftCrossLeft   = 13,
    Btn_LeftCrossRight  = 5,
    Btn_LeftCrossMid    = 9,
    Btn_RightCrossUp    = 4,
    Btn_RightCrossDown  = 3,
    Btn_RightCrossLeft  = 7,
    Btn_RightCrossRight = 15,
    Btn_RightCrossMid   = 11,
    Btn_Btn0            = 6,
    Btn_Btn1            = 10,
    Btn_Btn2            = 14,
    Btn_Btn3            = 8,
    Btn_Btn4            = 12,
    Btn_Btn5            = 16,
    Btn_KnobL           = 17,
    Btn_KnobR           = 18,
    Btn_JoystickL       = 19,
    Btn_JoystickR       = 20,
} KEYS;

typedef enum {
    Left_switch = 0,
    Right_switch
} SWITCHS;

void AS69_RemoteControl_Init();
void AS69_RemoteControl_SendMsg(mavlink_joystick_air_dashboard_set_msg_t *msg_joystick_air_msg);
void JoystickSwitchLED(float r, float g, float b, float lightness, uint16_t duration, mavlink_joystick_air_led_t *Msg_joystick_air_led);
void JoystickSwitchTitle(uint8_t id, char title[20], mavlink_joystick_air_dashboard_set_title_t *Msg_joystick_air_title);
void JoystickSwitchMsg(uint8_t id, char message[20], mavlink_joystick_air_dashboard_set_msg_t *Msg_joystick_air_msg);
void JoystickDelete(uint8_t id, mavlink_joystick_air_dashboard_del_t *Msg_joystick_air_delete);
bool ReadJoystickButtons(mavlink_joystick_air_t *msg_joystick_air_, KEYS index);
float ReadJoystickLeft_x(mavlink_joystick_air_t *msg_joystick_air_);
float ReadJoystickLeft_y(mavlink_joystick_air_t *msg_joystick_air_);
float ReadJoystickRight_x(mavlink_joystick_air_t *msg_joystick_air_);
float ReadJoystickRight_y(mavlink_joystick_air_t *msg_joystick_air_);
int16_t ReadJoystickKnobsLeft(mavlink_joystick_air_t *msg_joystick_air_);
int16_t ReadJoystickKnobsRight(mavlink_joystick_air_t *msg_joystick_air_);
bool ReadJoystickSwitchs(mavlink_joystick_air_t *msg_joystick_air_, SWITCHS index);

extern mavlink_joystick_air_t msg_joystick_air;
extern mavlink_joystick_air_led_t msg_joystick_air_led;
extern mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_point;
extern mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_state;
extern mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_posture;
extern mavlink_joystick_air_dashboard_set_title_t msg_joystick_air_title_knob_r;
extern mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_point;
extern mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_state;
extern mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_posture;
extern mavlink_joystick_air_dashboard_set_msg_t msg_joystick_air_msg_knob_r;
extern mavlink_joystick_air_dashboard_del_t msg_joystick_air_delete;

#endif