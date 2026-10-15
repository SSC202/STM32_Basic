#pragma once
// MESSAGE JOYSTICK_AIR PACKING

#define MAVLINK_MSG_ID_JOYSTICK_AIR 209


typedef struct __mavlink_joystick_air_t {
 uint32_t buttons; /*<   Buttons */
 float joystickL[2]; /*<   [0]: x pos [1]: y pos */
 float joystickR[2]; /*<   [0]: x pos [1]: y pos */
 int16_t knobs[2]; /*<   [0]: left knob. [1]: right knob */
 uint8_t switchs; /*<   bit0: left sw. bit1: right sw */
} mavlink_joystick_air_t;

#define MAVLINK_MSG_ID_JOYSTICK_AIR_LEN 25
#define MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN 25
#define MAVLINK_MSG_ID_209_LEN 25
#define MAVLINK_MSG_ID_209_MIN_LEN 25

#define MAVLINK_MSG_ID_JOYSTICK_AIR_CRC 219
#define MAVLINK_MSG_ID_209_CRC 219

#define MAVLINK_MSG_JOYSTICK_AIR_FIELD_JOYSTICKL_LEN 2
#define MAVLINK_MSG_JOYSTICK_AIR_FIELD_JOYSTICKR_LEN 2
#define MAVLINK_MSG_JOYSTICK_AIR_FIELD_KNOBS_LEN 2

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR { \
    209, \
    "JOYSTICK_AIR", \
    5, \
    {  { "buttons", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_joystick_air_t, buttons) }, \
         { "knobs", NULL, MAVLINK_TYPE_INT16_T, 2, 20, offsetof(mavlink_joystick_air_t, knobs) }, \
         { "joystickL", NULL, MAVLINK_TYPE_FLOAT, 2, 4, offsetof(mavlink_joystick_air_t, joystickL) }, \
         { "joystickR", NULL, MAVLINK_TYPE_FLOAT, 2, 12, offsetof(mavlink_joystick_air_t, joystickR) }, \
         { "switchs", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_joystick_air_t, switchs) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR { \
    "JOYSTICK_AIR", \
    5, \
    {  { "buttons", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_joystick_air_t, buttons) }, \
         { "knobs", NULL, MAVLINK_TYPE_INT16_T, 2, 20, offsetof(mavlink_joystick_air_t, knobs) }, \
         { "joystickL", NULL, MAVLINK_TYPE_FLOAT, 2, 4, offsetof(mavlink_joystick_air_t, joystickL) }, \
         { "joystickR", NULL, MAVLINK_TYPE_FLOAT, 2, 12, offsetof(mavlink_joystick_air_t, joystickR) }, \
         { "switchs", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_joystick_air_t, switchs) }, \
         } \
}
#endif

/**
 * @brief Pack a joystick_air message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param buttons   Buttons 
 * @param knobs   [0]: left knob. [1]: right knob 
 * @param joystickL   [0]: x pos [1]: y pos 
 * @param joystickR   [0]: x pos [1]: y pos 
 * @param switchs   bit0: left sw. bit1: right sw 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint32_t buttons, const int16_t *knobs, const float *joystickL, const float *joystickR, uint8_t switchs)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LEN];
    _mav_put_uint32_t(buf, 0, buttons);
    _mav_put_uint8_t(buf, 24, switchs);
    _mav_put_float_array(buf, 4, joystickL, 2);
    _mav_put_float_array(buf, 12, joystickR, 2);
    _mav_put_int16_t_array(buf, 20, knobs, 2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#else
    mavlink_joystick_air_t packet;
    packet.buttons = buttons;
    packet.switchs = switchs;
    mav_array_memcpy(packet.joystickL, joystickL, sizeof(float)*2);
    mav_array_memcpy(packet.joystickR, joystickR, sizeof(float)*2);
    mav_array_memcpy(packet.knobs, knobs, sizeof(int16_t)*2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
}

/**
 * @brief Pack a joystick_air message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param buttons   Buttons 
 * @param knobs   [0]: left knob. [1]: right knob 
 * @param joystickL   [0]: x pos [1]: y pos 
 * @param joystickR   [0]: x pos [1]: y pos 
 * @param switchs   bit0: left sw. bit1: right sw 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint32_t buttons, const int16_t *knobs, const float *joystickL, const float *joystickR, uint8_t switchs)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LEN];
    _mav_put_uint32_t(buf, 0, buttons);
    _mav_put_uint8_t(buf, 24, switchs);
    _mav_put_float_array(buf, 4, joystickL, 2);
    _mav_put_float_array(buf, 12, joystickR, 2);
    _mav_put_int16_t_array(buf, 20, knobs, 2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#else
    mavlink_joystick_air_t packet;
    packet.buttons = buttons;
    packet.switchs = switchs;
    mav_array_memcpy(packet.joystickL, joystickL, sizeof(float)*2);
    mav_array_memcpy(packet.joystickR, joystickR, sizeof(float)*2);
    mav_array_memcpy(packet.knobs, knobs, sizeof(int16_t)*2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#endif
}

/**
 * @brief Pack a joystick_air message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param buttons   Buttons 
 * @param knobs   [0]: left knob. [1]: right knob 
 * @param joystickL   [0]: x pos [1]: y pos 
 * @param joystickR   [0]: x pos [1]: y pos 
 * @param switchs   bit0: left sw. bit1: right sw 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint32_t buttons,const int16_t *knobs,const float *joystickL,const float *joystickR,uint8_t switchs)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LEN];
    _mav_put_uint32_t(buf, 0, buttons);
    _mav_put_uint8_t(buf, 24, switchs);
    _mav_put_float_array(buf, 4, joystickL, 2);
    _mav_put_float_array(buf, 12, joystickR, 2);
    _mav_put_int16_t_array(buf, 20, knobs, 2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#else
    mavlink_joystick_air_t packet;
    packet.buttons = buttons;
    packet.switchs = switchs;
    mav_array_memcpy(packet.joystickL, joystickL, sizeof(float)*2);
    mav_array_memcpy(packet.joystickR, joystickR, sizeof(float)*2);
    mav_array_memcpy(packet.knobs, knobs, sizeof(int16_t)*2);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
}

/**
 * @brief Encode a joystick_air struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_joystick_air_t* joystick_air)
{
    return mavlink_msg_joystick_air_pack(system_id, component_id, msg, joystick_air->buttons, joystick_air->knobs, joystick_air->joystickL, joystick_air->joystickR, joystick_air->switchs);
}

/**
 * @brief Encode a joystick_air struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_joystick_air_t* joystick_air)
{
    return mavlink_msg_joystick_air_pack_chan(system_id, component_id, chan, msg, joystick_air->buttons, joystick_air->knobs, joystick_air->joystickL, joystick_air->joystickR, joystick_air->switchs);
}

/**
 * @brief Encode a joystick_air struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_joystick_air_t* joystick_air)
{
    return mavlink_msg_joystick_air_pack_status(system_id, component_id, _status, msg,  joystick_air->buttons, joystick_air->knobs, joystick_air->joystickL, joystick_air->joystickR, joystick_air->switchs);
}

/**
 * @brief Send a joystick_air message
 * @param chan MAVLink channel to send the message
 *
 * @param buttons   Buttons 
 * @param knobs   [0]: left knob. [1]: right knob 
 * @param joystickL   [0]: x pos [1]: y pos 
 * @param joystickR   [0]: x pos [1]: y pos 
 * @param switchs   bit0: left sw. bit1: right sw 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_joystick_air_send(mavlink_channel_t chan, uint32_t buttons, const int16_t *knobs, const float *joystickL, const float *joystickR, uint8_t switchs)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LEN];
    _mav_put_uint32_t(buf, 0, buttons);
    _mav_put_uint8_t(buf, 24, switchs);
    _mav_put_float_array(buf, 4, joystickL, 2);
    _mav_put_float_array(buf, 12, joystickR, 2);
    _mav_put_int16_t_array(buf, 20, knobs, 2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#else
    mavlink_joystick_air_t packet;
    packet.buttons = buttons;
    packet.switchs = switchs;
    mav_array_memcpy(packet.joystickL, joystickL, sizeof(float)*2);
    mav_array_memcpy(packet.joystickR, joystickR, sizeof(float)*2);
    mav_array_memcpy(packet.knobs, knobs, sizeof(int16_t)*2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR, (const char *)&packet, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#endif
}

/**
 * @brief Send a joystick_air message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_joystick_air_send_struct(mavlink_channel_t chan, const mavlink_joystick_air_t* joystick_air)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_joystick_air_send(chan, joystick_air->buttons, joystick_air->knobs, joystick_air->joystickL, joystick_air->joystickR, joystick_air->switchs);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR, (const char *)joystick_air, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#endif
}

#if MAVLINK_MSG_ID_JOYSTICK_AIR_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_joystick_air_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t buttons, const int16_t *knobs, const float *joystickL, const float *joystickR, uint8_t switchs)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, buttons);
    _mav_put_uint8_t(buf, 24, switchs);
    _mav_put_float_array(buf, 4, joystickL, 2);
    _mav_put_float_array(buf, 12, joystickR, 2);
    _mav_put_int16_t_array(buf, 20, knobs, 2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#else
    mavlink_joystick_air_t *packet = (mavlink_joystick_air_t *)msgbuf;
    packet->buttons = buttons;
    packet->switchs = switchs;
    mav_array_memcpy(packet->joystickL, joystickL, sizeof(float)*2);
    mav_array_memcpy(packet->joystickR, joystickR, sizeof(float)*2);
    mav_array_memcpy(packet->knobs, knobs, sizeof(int16_t)*2);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR, (const char *)packet, MAVLINK_MSG_ID_JOYSTICK_AIR_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_CRC);
#endif
}
#endif

#endif

// MESSAGE JOYSTICK_AIR UNPACKING


/**
 * @brief Get field buttons from joystick_air message
 *
 * @return   Buttons 
 */
static inline uint32_t mavlink_msg_joystick_air_get_buttons(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field knobs from joystick_air message
 *
 * @return   [0]: left knob. [1]: right knob 
 */
static inline uint16_t mavlink_msg_joystick_air_get_knobs(const mavlink_message_t* msg, int16_t *knobs)
{
    return _MAV_RETURN_int16_t_array(msg, knobs, 2,  20);
}

/**
 * @brief Get field joystickL from joystick_air message
 *
 * @return   [0]: x pos [1]: y pos 
 */
static inline uint16_t mavlink_msg_joystick_air_get_joystickL(const mavlink_message_t* msg, float *joystickL)
{
    return _MAV_RETURN_float_array(msg, joystickL, 2,  4);
}

/**
 * @brief Get field joystickR from joystick_air message
 *
 * @return   [0]: x pos [1]: y pos 
 */
static inline uint16_t mavlink_msg_joystick_air_get_joystickR(const mavlink_message_t* msg, float *joystickR)
{
    return _MAV_RETURN_float_array(msg, joystickR, 2,  12);
}

/**
 * @brief Get field switchs from joystick_air message
 *
 * @return   bit0: left sw. bit1: right sw 
 */
static inline uint8_t mavlink_msg_joystick_air_get_switchs(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  24);
}

/**
 * @brief Decode a joystick_air message into a struct
 *
 * @param msg The message to decode
 * @param joystick_air C-struct to decode the message contents into
 */
static inline void mavlink_msg_joystick_air_decode(const mavlink_message_t* msg, mavlink_joystick_air_t* joystick_air)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    joystick_air->buttons = mavlink_msg_joystick_air_get_buttons(msg);
    mavlink_msg_joystick_air_get_joystickL(msg, joystick_air->joystickL);
    mavlink_msg_joystick_air_get_joystickR(msg, joystick_air->joystickR);
    mavlink_msg_joystick_air_get_knobs(msg, joystick_air->knobs);
    joystick_air->switchs = mavlink_msg_joystick_air_get_switchs(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_JOYSTICK_AIR_LEN? msg->len : MAVLINK_MSG_ID_JOYSTICK_AIR_LEN;
        memset(joystick_air, 0, MAVLINK_MSG_ID_JOYSTICK_AIR_LEN);
    memcpy(joystick_air, _MAV_PAYLOAD(msg), len);
#endif
}
