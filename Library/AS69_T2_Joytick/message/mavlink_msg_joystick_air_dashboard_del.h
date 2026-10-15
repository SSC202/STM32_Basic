#pragma once
// MESSAGE JOYSTICK_AIR_DASHBOARD_DEL PACKING

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL 213


typedef struct __mavlink_joystick_air_dashboard_del_t {
 uint8_t id; /*<  */
} mavlink_joystick_air_dashboard_del_t;

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN 1
#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN 1
#define MAVLINK_MSG_ID_213_LEN 1
#define MAVLINK_MSG_ID_213_MIN_LEN 1

#define MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC 36
#define MAVLINK_MSG_ID_213_CRC 36



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_DEL { \
    213, \
    "JOYSTICK_AIR_DASHBOARD_DEL", \
    1, \
    {  { "id", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_joystick_air_dashboard_del_t, id) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_JOYSTICK_AIR_DASHBOARD_DEL { \
    "JOYSTICK_AIR_DASHBOARD_DEL", \
    1, \
    {  { "id", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_joystick_air_dashboard_del_t, id) }, \
         } \
}
#endif

/**
 * @brief Pack a joystick_air_dashboard_del message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param id  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN];
    _mav_put_uint8_t(buf, 0, id);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#else
    mavlink_joystick_air_dashboard_del_t packet;
    packet.id = id;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
}

/**
 * @brief Pack a joystick_air_dashboard_del message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param id  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint8_t id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN];
    _mav_put_uint8_t(buf, 0, id);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#else
    mavlink_joystick_air_dashboard_del_t packet;
    packet.id = id;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#endif
}

/**
 * @brief Pack a joystick_air_dashboard_del message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param id  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN];
    _mav_put_uint8_t(buf, 0, id);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#else
    mavlink_joystick_air_dashboard_del_t packet;
    packet.id = id;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
}

/**
 * @brief Encode a joystick_air_dashboard_del struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_del C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_del_t* joystick_air_dashboard_del)
{
    return mavlink_msg_joystick_air_dashboard_del_pack(system_id, component_id, msg, joystick_air_dashboard_del->id);
}

/**
 * @brief Encode a joystick_air_dashboard_del struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_del C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_del_t* joystick_air_dashboard_del)
{
    return mavlink_msg_joystick_air_dashboard_del_pack_chan(system_id, component_id, chan, msg, joystick_air_dashboard_del->id);
}

/**
 * @brief Encode a joystick_air_dashboard_del struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param joystick_air_dashboard_del C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_joystick_air_dashboard_del_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_joystick_air_dashboard_del_t* joystick_air_dashboard_del)
{
    return mavlink_msg_joystick_air_dashboard_del_pack_status(system_id, component_id, _status, msg,  joystick_air_dashboard_del->id);
}

/**
 * @brief Send a joystick_air_dashboard_del message
 * @param chan MAVLink channel to send the message
 *
 * @param id  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_joystick_air_dashboard_del_send(mavlink_channel_t chan, uint8_t id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN];
    _mav_put_uint8_t(buf, 0, id);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#else
    mavlink_joystick_air_dashboard_del_t packet;
    packet.id = id;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL, (const char *)&packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#endif
}

/**
 * @brief Send a joystick_air_dashboard_del message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_joystick_air_dashboard_del_send_struct(mavlink_channel_t chan, const mavlink_joystick_air_dashboard_del_t* joystick_air_dashboard_del)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_joystick_air_dashboard_del_send(chan, joystick_air_dashboard_del->id);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL, (const char *)joystick_air_dashboard_del, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#endif
}

#if MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_joystick_air_dashboard_del_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t id)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, id);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL, buf, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#else
    mavlink_joystick_air_dashboard_del_t *packet = (mavlink_joystick_air_dashboard_del_t *)msgbuf;
    packet->id = id;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL, (const char *)packet, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_MIN_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_CRC);
#endif
}
#endif

#endif

// MESSAGE JOYSTICK_AIR_DASHBOARD_DEL UNPACKING


/**
 * @brief Get field id from joystick_air_dashboard_del message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_joystick_air_dashboard_del_get_id(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Decode a joystick_air_dashboard_del message into a struct
 *
 * @param msg The message to decode
 * @param joystick_air_dashboard_del C-struct to decode the message contents into
 */
static inline void mavlink_msg_joystick_air_dashboard_del_decode(const mavlink_message_t* msg, mavlink_joystick_air_dashboard_del_t* joystick_air_dashboard_del)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    joystick_air_dashboard_del->id = mavlink_msg_joystick_air_dashboard_del_get_id(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN? msg->len : MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN;
        memset(joystick_air_dashboard_del, 0, MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL_LEN);
    memcpy(joystick_air_dashboard_del, _MAV_PAYLOAD(msg), len);
#endif
}
