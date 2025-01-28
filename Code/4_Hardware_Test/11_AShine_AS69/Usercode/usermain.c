#include "usermain.h"

mavlink_joystick_air_led_t mav_led_msg;
mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
char direct_choose_title[20] = "Hello";
char direct_choose_msg[20]   = "World";

void StartDefaultTask(void *argument)
{
    RetargetInit(&huart6);
    AS69_RemoteControl_Init();
    for (;;) {
        JoystickSwitchLED(255, 0, 0, 0.5, 100, &mav_led_msg);
        osDelay(2);
    }
}