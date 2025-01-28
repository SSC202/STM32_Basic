#pragma once
// MESSAGE JOYSTICK_AIR_DASHBOARD_SET_MSG PACKING

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG 212


typedef struct __mavlink_joystick_air_dashboard_set_msg_t {
 uint8_t id; /*<  */
 char message[20]; /*<  */
} mavlink_joystick_air_dashboard_set_msg_t;

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN 21
#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN 21
#define MAVLINK_MSG_ID_212_LEN 21
#define MAVLINK_MSG_ID_212_MIN_LEN 21

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC 22
#define MAVLINK_MSG_ID_212_CRC 22

#define MAVLINK_MSG_JOYSTICK_AIR_DASHBOARD_SET_MSG_FIELD_MESSAGE_LEN 20

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_SET_MSG { \
    212, \
    "JOYSTICK_AIR_DASHBOARD_SET_MSG", \
    2, \
    {  { "id", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_joystick_air_dashboard_set_msg_t, id) }, \
         { "message", NULL, MAVLINK_TYPE_CHAR, 20, 1, offsetof(mavlink_joystick_air_dashboard_set_msg_t, message) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_SET_MSG { \
    "JOYSTICK_AIR_DASHBOARD_SET_MSG", \
    2, \
    {  { "id", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_joystick_air_dashboard_set_msg_t, id) }, \
         { "message", NULL, MAVLINK_TYPE_CHAR, 20, 1, offsetof(mavlink_joystick_air_dashboard_set_msg_t, message) }, \
         } \
}
#endif

/**
 * @brief Pack a joystick_air_dashboard_set_msg message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param id  
 * @param message  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t id, const char *message)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN];
    _mav_put_uint8_t(buf, 0, id);
    _mav_put_char_array(buf, 1, message, 20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#else
    mavlink_joystick_air_dashboard_set_msg_t packet;
    packet.id = id;
    mav_array_memcpy(packet.message, message, sizeof(char)*20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
}

/**
 * @brief Pack a joystick_air_dashboard_set_msg message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param id  
 * @param message  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint8_t id, const char *message)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN];
    _mav_put_uint8_t(buf, 0, id);
    _mav_put_char_array(buf, 1, message, 20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#else
    mavlink_joystick_air_dashboard_set_msg_t packet;
    packet.id = id;
    mav_array_memcpy(packet.message, message, sizeof(char)*20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#endif
}

/**
 * @brief Pack a joystick_air_dashboard_set_msg message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param id  
 * @param message  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t id,const char *message)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN];
    _mav_put_uint8_t(buf, 0, id);
    _mav_put_char_array(buf, 1, message, 20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#else
    mavlink_joystick_air_dashboard_set_msg_t packet;
    packet.id = id;
    mav_array_memcpy(packet.message, message, sizeof(char)*20);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
}

/**
 * @brief Encode a joystick_air_dashboard_set_msg struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_set_msg C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_set_msg_t* joystick_air_dashboard_set_msg)
{
    return mavlink_msg_joystick_air_dashboard_set_msg_pack(system_id, component_id, msg, joystick_air_dashboard_set_msg->id, joystick_air_dashboard_set_msg->message);
}

/**
 * @brief Encode a joystick_air_dashboard_set_msg struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_set_msg C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_set_msg_t* joystick_air_dashboard_set_msg)
{
    return mavlink_msg_joystick_air_dashboard_set_msg_pack_chan(system_id, component_id, chan, msg, joystick_air_dashboard_set_msg->id, joystick_air_dashboard_set_msg->message);
}

/**
 * @brief Encode a joystick_air_dashboard_set_msg struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_set_msg C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_set_msg_t* joystick_air_dashboard_set_msg)
{
    return mavlink_msg_joystick_air_dashboard_set_msg_pack_status(system_id, component_id, _status, msg,  joystick_air_dashboard_set_msg->id, joystick_air_dashboard_set_msg->message);
}

/**
 * @brief Send a joystick_air_dashboard_set_msg message
 * @param chan MAVLink channel to send the message
 *
 * @param id  
 * @param message  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_joystick_air_dashboard_set_msg_send(mavlink_channel_t chan, uint8_t id, const char *message)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN];
    _mav_put_uint8_t(buf, 0, id);
    _mav_put_char_array(buf, 1, message, 20);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#else
    mavlink_joystick_air_dashboard_set_msg_t packet;
    packet.id = id;
    mav_array_memcpy(packet.message, message, sizeof(char)*20);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG, (const char *)&packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#endif
}

/**
 * @brief Send a joystick_air_dashboard_set_msg message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_joystick_air_dashboard_set_msg_send_struct(mavlink_channel_t chan, const mavlink_joystick_air_dashboard_set_msg_t* joystick_air_dashboard_set_msg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_joystick_air_dashboard_set_msg_send(chan, joystick_air_dashboard_set_msg->id, joystick_air_dashboard_set_msg->message);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG, (const char *)joystick_air_dashboard_set_msg, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#endif
}

#if MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_joystick_air_dashboard_set_msg_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t id, const char *message)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, id);
    _mav_put_char_array(buf, 1, message, 20);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#else
    mavlink_joystick_air_dashboard_set_msg_t *packet = (mavlink_joystick_air_dashboard_set_msg_t *)msgbuf;
    packet->id = id;
    mav_array_memcpy(packet->message, message, sizeof(char)*20);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG, (const char *)packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_CRC);
#endif
}
#endif

#endif

// MESSAGE JOYSTICK_AIR_DASHBOARD_SET_MSG UNPACKING


/**
 * @brief Get field id from joystick_air_dashboard_set_msg message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_joystick_air_dashboard_set_msg_get_id(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field message from joystick_air_dashboard_set_msg message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_set_msg_get_message(const mavlink_message_t* msg, char *message)
{
    return _MAV_RETURN_char_array(msg, message, 20,  1);
}

/**
 * @brief Decode a joystick_air_dashboard_set_msg message into a struct
 *
 * @param msg The message to decode
 * @param joystick_air_dashboard_set_msg C-struct to decode the message contents into
 */
static inline void mavlink_msg_joystick_air_dashboard_set_msg_decode(const mavlink_message_t* msg, mavlink_joystick_air_dashboard_set_msg_t* joystick_air_dashboard_set_msg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    joystick_air_dashboard_set_msg->id = mavlink_msg_joystick_air_dashboard_set_msg_get_id(msg);
    mavlink_msg_joystick_air_dashboard_set_msg_get_message(msg, joystick_air_dashboard_set_msg->message);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN? msg->len : MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN;
        memset(joystick_air_dashboard_set_msg, 0, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG_LEN);
    memcpy(joystick_air_dashboard_set_msg, _MAV_PAYLOAD(msg), len);
#endif
}
