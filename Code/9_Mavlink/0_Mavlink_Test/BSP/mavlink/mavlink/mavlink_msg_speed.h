#pragma once
// MESSAGE SPEED PACKING

#define MAVLINK_MSG_ID_SPEED 1


typedef struct __mavlink_speed_t {
 double vx; /*<  x 方向速度*/
 double vy; /*<  y 方向速度*/
 double rot; /*<  z 方向角速度*/
} mavlink_speed_t;

#define MAVLINK_MSG_ID_SPEED_LEN 24
#define MAVLINK_MSG_ID_SPEED_MIN_LEN 24
#define MAVLINK_MSG_ID_1_LEN 24
#define MAVLINK_MSG_ID_1_MIN_LEN 24

#define MAVLINK_MSG_ID_SPEED_CRC 100
#define MAVLINK_MSG_ID_1_CRC 100



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_SPEED { \
    1, \
    "SPEED", \
    3, \
    {  { "vx", NULL, MAVLINK_TYPE_DOUBLE, 0, 0, offsetof(mavlink_speed_t, vx) }, \
         { "vy", NULL, MAVLINK_TYPE_DOUBLE, 0, 8, offsetof(mavlink_speed_t, vy) }, \
         { "rot", NULL, MAVLINK_TYPE_DOUBLE, 0, 16, offsetof(mavlink_speed_t, rot) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_SPEED { \
    "SPEED", \
    3, \
    {  { "vx", NULL, MAVLINK_TYPE_DOUBLE, 0, 0, offsetof(mavlink_speed_t, vx) }, \
         { "vy", NULL, MAVLINK_TYPE_DOUBLE, 0, 8, offsetof(mavlink_speed_t, vy) }, \
         { "rot", NULL, MAVLINK_TYPE_DOUBLE, 0, 16, offsetof(mavlink_speed_t, rot) }, \
         } \
}
#endif

/**
 * @brief Pack a speed message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param vx  x 方向速度
 * @param vy  y 方向速度
 * @param rot  z 方向角速度
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_speed_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               double vx, double vy, double rot)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SPEED_LEN];
    _mav_put_double(buf, 0, vx);
    _mav_put_double(buf, 8, vy);
    _mav_put_double(buf, 16, rot);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SPEED_LEN);
#else
    mavlink_speed_t packet;
    packet.vx = vx;
    packet.vy = vy;
    packet.rot = rot;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SPEED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SPEED;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
}

/**
 * @brief Pack a speed message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vx  x 方向速度
 * @param vy  y 方向速度
 * @param rot  z 方向角速度
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_speed_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   double vx,double vy,double rot)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SPEED_LEN];
    _mav_put_double(buf, 0, vx);
    _mav_put_double(buf, 8, vy);
    _mav_put_double(buf, 16, rot);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SPEED_LEN);
#else
    mavlink_speed_t packet;
    packet.vx = vx;
    packet.vy = vy;
    packet.rot = rot;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SPEED_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_SPEED;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
}

/**
 * @brief Encode a speed struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param speed C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_speed_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_speed_t* speed)
{
    return mavlink_msg_speed_pack(system_id, component_id, msg, speed->vx, speed->vy, speed->rot);
}

/**
 * @brief Encode a speed struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param speed C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_speed_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_speed_t* speed)
{
    return mavlink_msg_speed_pack_chan(system_id, component_id, chan, msg, speed->vx, speed->vy, speed->rot);
}

/**
 * @brief Send a speed message
 * @param chan MAVLink channel to send the message
 *
 * @param vx  x 方向速度
 * @param vy  y 方向速度
 * @param rot  z 方向角速度
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_speed_send(mavlink_channel_t chan, double vx, double vy, double rot)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_SPEED_LEN];
    _mav_put_double(buf, 0, vx);
    _mav_put_double(buf, 8, vy);
    _mav_put_double(buf, 16, rot);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SPEED, buf, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
#else
    mavlink_speed_t packet;
    packet.vx = vx;
    packet.vy = vy;
    packet.rot = rot;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SPEED, (const char *)&packet, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
#endif
}

/**
 * @brief Send a speed message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_speed_send_struct(mavlink_channel_t chan, const mavlink_speed_t* speed)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_speed_send(chan, speed->vx, speed->vy, speed->rot);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SPEED, (const char *)speed, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
#endif
}

#if MAVLINK_MSG_ID_SPEED_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_speed_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  double vx, double vy, double rot)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_double(buf, 0, vx);
    _mav_put_double(buf, 8, vy);
    _mav_put_double(buf, 16, rot);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SPEED, buf, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
#else
    mavlink_speed_t *packet = (mavlink_speed_t *)msgbuf;
    packet->vx = vx;
    packet->vy = vy;
    packet->rot = rot;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SPEED, (const char *)packet, MAVLINK_MSG_ID_SPEED_MIN_LEN, MAVLINK_MSG_ID_SPEED_LEN, MAVLINK_MSG_ID_SPEED_CRC);
#endif
}
#endif

#endif

// MESSAGE SPEED UNPACKING


/**
 * @brief Get field vx from speed message
 *
 * @return  x 方向速度
 */
static inline double mavlink_msg_speed_get_vx(const mavlink_message_t* msg)
{
    return _MAV_RETURN_double(msg,  0);
}

/**
 * @brief Get field vy from speed message
 *
 * @return  y 方向速度
 */
static inline double mavlink_msg_speed_get_vy(const mavlink_message_t* msg)
{
    return _MAV_RETURN_double(msg,  8);
}

/**
 * @brief Get field rot from speed message
 *
 * @return  z 方向角速度
 */
static inline double mavlink_msg_speed_get_rot(const mavlink_message_t* msg)
{
    return _MAV_RETURN_double(msg,  16);
}

/**
 * @brief Decode a speed message into a struct
 *
 * @param msg The message to decode
 * @param speed C-struct to decode the message contents into
 */
static inline void mavlink_msg_speed_decode(const mavlink_message_t* msg, mavlink_speed_t* speed)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    speed->vx = mavlink_msg_speed_get_vx(msg);
    speed->vy = mavlink_msg_speed_get_vy(msg);
    speed->rot = mavlink_msg_speed_get_rot(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_SPEED_LEN? msg->len : MAVLINK_MSG_ID_SPEED_LEN;
        memset(speed, 0, MAVLINK_MSG_ID_SPEED_LEN);
    memcpy(speed, _MAV_PAYLOAD(msg), len);
#endif
}
