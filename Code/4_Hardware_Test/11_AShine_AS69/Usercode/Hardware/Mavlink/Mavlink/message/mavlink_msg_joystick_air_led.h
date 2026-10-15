#pragma once
// MESSAGE JOYSTICK_AIR_LED PACKING

#define MAVLINK_MSG_ID_JOYSTICK_AIR_LED 210


typedef struct __mavlink_joystick_air_led_t {
 float r; /*<   range: [0, 1] */
 float g; /*<   range: [0, 1] */
 float b; /*<   range: [0, 1] */
 float lightness; /*<   Gain of lightness. Must be positive values */
 uint16_t duration; /*<   Led will be on for this milliseconds */
} mavlink_joystick_air_led_t;

#define MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN 18
#define MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN 18
#define MAVLINK_MSG_ID_210_LEN 18
#define MAVLINK_MSG_ID_210_MIN_LEN 18

#define MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC 142
#define MAVLINK_MSG_ID_210_CRC 142



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_LED { \
    210, \
    "JOYSTICK_AIR_LED", \
    5, \
    {  { "r", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_joystick_air_led_t, r) }, \
         { "g", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_joystick_air_led_t, g) }, \
         { "b", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_joystick_air_led_t, b) }, \
         { "lightness", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_joystick_air_led_t, lightness) }, \
         { "duration", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_joystick_air_led_t, duration) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_LED { \
    "JOYSTICK_AIR_LED", \
    5, \
    {  { "r", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_joystick_air_led_t, r) }, \
         { "g", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_joystick_air_led_t, g) }, \
         { "b", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_joystick_air_led_t, b) }, \
         { "lightness", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_joystick_air_led_t, lightness) }, \
         { "duration", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_joystick_air_led_t, duration) }, \
         } \
}
#endif

/**
 * @brief Pack a joystick_air_led message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param r   range: [0, 1] 
 * @param g   range: [0, 1] 
 * @param b   range: [0, 1] 
 * @param lightness   Gain of lightness. Must be positive values 
 * @param duration   Led will be on for this milliseconds 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_led_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               float r, float g, float b, float lightness, uint16_t duration)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN];
    _mav_put_float(buf, 0, r);
    _mav_put_float(buf, 4, g);
    _mav_put_float(buf, 8, b);
    _mav_put_float(buf, 12, lightness);
    _mav_put_uint16_t(buf, 16, duration);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#else
    mavlink_joystick_air_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.lightness = lightness;
    packet.duration = duration;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_LED;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
}

/**
 * @brief Pack a joystick_air_led message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param r   range: [0, 1] 
 * @param g   range: [0, 1] 
 * @param b   range: [0, 1] 
 * @param lightness   Gain of lightness. Must be positive values 
 * @param duration   Led will be on for this milliseconds 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_led_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               float r, float g, float b, float lightness, uint16_t duration)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN];
    _mav_put_float(buf, 0, r);
    _mav_put_float(buf, 4, g);
    _mav_put_float(buf, 8, b);
    _mav_put_float(buf, 12, lightness);
    _mav_put_uint16_t(buf, 16, duration);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#else
    mavlink_joystick_air_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.lightness = lightness;
    packet.duration = duration;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_LED;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#endif
}

/**
 * @brief Pack a joystick_air_led message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param r   range: [0, 1] 
 * @param g   range: [0, 1] 
 * @param b   range: [0, 1] 
 * @param lightness   Gain of lightness. Must be positive values 
 * @param duration   Led will be on for this milliseconds 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_led_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   float r,float g,float b,float lightness,uint16_t duration)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN];
    _mav_put_float(buf, 0, r);
    _mav_put_float(buf, 4, g);
    _mav_put_float(buf, 8, b);
    _mav_put_float(buf, 12, lightness);
    _mav_put_uint16_t(buf, 16, duration);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#else
    mavlink_joystick_air_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.lightness = lightness;
    packet.duration = duration;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_LED;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
}

/**
 * @brief Encode a joystick_air_led struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_led_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_joystick_air_led_t* joystick_air_led)
{
    return mavlink_msg_joystick_air_led_pack(system_id, component_id, msg, joystick_air_led->r, joystick_air_led->g, joystick_air_led->b, joystick_air_led->lightness, joystick_air_led->duration);
}

/**
 * @brief Encode a joystick_air_led struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_led_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_joystick_air_led_t* joystick_air_led)
{
    return mavlink_msg_joystick_air_led_pack_chan(system_id, component_id, chan, msg, joystick_air_led->r, joystick_air_led->g, joystick_air_led->b, joystick_air_led->lightness, joystick_air_led->duration);
}

/**
 * @brief Encode a joystick_air_led struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_led C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_led_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_joystick_air_led_t* joystick_air_led)
{
    return mavlink_msg_joystick_air_led_pack_status(system_id, component_id, _status, msg,  joystick_air_led->r, joystick_air_led->g, joystick_air_led->b, joystick_air_led->lightness, joystick_air_led->duration);
}

/**
 * @brief Send a joystick_air_led message
 * @param chan MAVLink channel to send the message
 *
 * @param r   range: [0, 1] 
 * @param g   range: [0, 1] 
 * @param b   range: [0, 1] 
 * @param lightness   Gain of lightness. Must be positive values 
 * @param duration   Led will be on for this milliseconds 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_joystick_air_led_send(mavlink_channel_t chan, float r, float g, float b, float lightness, uint16_t duration)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN];
    _mav_put_float(buf, 0, r);
    _mav_put_float(buf, 4, g);
    _mav_put_float(buf, 8, b);
    _mav_put_float(buf, 12, lightness);
    _mav_put_uint16_t(buf, 16, duration);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#else
    mavlink_joystick_air_led_t packet;
    packet.r = r;
    packet.g = g;
    packet.b = b;
    packet.lightness = lightness;
    packet.duration = duration;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED, (const char *)&packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#endif
}

/**
 * @brief Send a joystick_air_led message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_joystick_air_led_send_struct(mavlink_channel_t chan, const mavlink_joystick_air_led_t* joystick_air_led)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_joystick_air_led_send(chan, joystick_air_led->r, joystick_air_led->g, joystick_air_led->b, joystick_air_led->lightness, joystick_air_led->duration);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED, (const char *)joystick_air_led, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#endif
}

#if MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_joystick_air_led_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  float r, float g, float b, float lightness, uint16_t duration)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_float(buf, 0, r);
    _mav_put_float(buf, 4, g);
    _mav_put_float(buf, 8, b);
    _mav_put_float(buf, 12, lightness);
    _mav_put_uint16_t(buf, 16, duration);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#else
    mavlink_joystick_air_led_t *packet = (mavlink_joystick_air_led_t *)msgbuf;
    packet->r = r;
    packet->g = g;
    packet->b = b;
    packet->lightness = lightness;
    packet->duration = duration;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_LED, (const char *)packet, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_CRC);
#endif
}
#endif

#endif

// MESSAGE JOYSTICK_AIR_LED UNPACKING


/**
 * @brief Get field r from joystick_air_led message
 *
 * @return   range: [0, 1] 
 */
static inline float mavlink_msg_joystick_air_led_get_r(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field g from joystick_air_led message
 *
 * @return   range: [0, 1] 
 */
static inline float mavlink_msg_joystick_air_led_get_g(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field b from joystick_air_led message
 *
 * @return   range: [0, 1] 
 */
static inline float mavlink_msg_joystick_air_led_get_b(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field lightness from joystick_air_led message
 *
 * @return   Gain of lightness. Must be positive values 
 */
static inline float mavlink_msg_joystick_air_led_get_lightness(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field duration from joystick_air_led message
 *
 * @return   Led will be on for this milliseconds 
 */
static inline uint16_t mavlink_msg_joystick_air_led_get_duration(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  16);
}

/**
 * @brief Decode a joystick_air_led message into a struct
 *
 * @param msg The message to decode
 * @param joystick_air_led C-struct to decode the message contents into
 */
static inline void mavlink_msg_joystick_air_led_decode(const mavlink_message_t* msg, mavlink_joystick_air_led_t* joystick_air_led)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    joystick_air_led->r = mavlink_msg_joystick_air_led_get_r(msg);
    joystick_air_led->g = mavlink_msg_joystick_air_led_get_g(msg);
    joystick_air_led->b = mavlink_msg_joystick_air_led_get_b(msg);
    joystick_air_led->lightness = mavlink_msg_joystick_air_led_get_lightness(msg);
    joystick_air_led->duration = mavlink_msg_joystick_air_led_get_duration(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN? msg->len : MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN;
        memset(joystick_air_led, 0, MAVLINK_MSG_ID_JOYSTICK_AIR_LED_LEN);
    memcpy(joystick_air_led, _MAV_PAYLOAD(msg), len);
#endif
}
