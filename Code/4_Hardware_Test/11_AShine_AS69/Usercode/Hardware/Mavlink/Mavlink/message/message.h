/** @file
 *  @brief MAVLink comm protocol generated from message.xml
 *  @see http://mavlink.org
 */
#pragma once
#ifndef MAVLINK_MESSAGE_H
#define MAVLINK_MESSAGE_H

#ifndef MAVLINK_H
    #error Wrong include order: MAVLINK_MESSAGE.H MUST NOT BE DIRECTLY USED. Include mavlink.h from the same directory instead or set ALL AND EVERY defines from MAVLINK.H manually accordingly, including the #define MAVLINK_H call.
#endif

#define MAVLINK_MESSAGE_XML_HASH -1877380854367932155

#ifdef __cplusplus
extern "C" {
#endif

// MESSAGE LENGTHS AND CRCS

#ifndef MAVLINK_MESSAGE_LENGTHS
#define MAVLINK_MESSAGE_LENGTHS {}
#endif

#ifndef MAVLINK_MESSAGE_CRCS
#define MAVLINK_MESSAGE_CRCS {{209, 219, 25, 25, 0, 0, 0}, {210, 142, 18, 18, 0, 0, 0}, {211, 56, 21, 21, 0, 0, 0}, {212, 22, 21, 21, 0, 0, 0}, {213, 36, 1, 1, 0, 0, 0}}
#endif

#include "../protocol.h"

#define MAVLINK_ENABLED_MESSAGE

// ENUM DEFINITIONS



// MAVLINK VERSION

#ifndef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

#if (MAVLINK_VERSION == 0)
#undef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

// MESSAGE DEFINITIONS
#include "./mavlink_msg_joystick_air.h"
#include "./mavlink_msg_joystick_air_led.h"
#include "./mavlink_msg_joystick_air_dashboard_set_title.h"
#include "./mavlink_msg_joystick_air_dashboard_set_msg.h"
#include "./mavlink_msg_joystick_air_dashboard_del.h"

// base include



#if MAVLINK_MESSAGE_XML_HASH == MAVLINK_PRIMARY_XML_HASH
# define MAVLINK_MESSAGE_INFO {MAVLINK_MESSAGE_INFO_JOYSTICK_AIR, MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_LED, MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_SET_TITLE, MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_SET_MSG, MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_DEL}
# define MAVLINK_MESSAGE_NAMES {{ "JOYSTICK_AIR", 209 }, { "JOYSTICK_AIR_DASHBOARD_DEL", 213 }, { "JOYSTICK_AIR_DASHBOARD_SET_MSG", 212 }, { "JOYSTICK_AIR_DASHBOARD_SET_TITLE", 211 }, { "JOYSTICK_AIR_LED", 210 }}
# if MAVLINK_COMMAND_24BIT
#  include "../mavlink_get_info.h"
# endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MAVLINK_MESSAGE_H
