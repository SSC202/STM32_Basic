#pragma once
// MESSAGE GYRO PACKING

#define MAVLINK_MSG_ID_GYRO 2


typedef struct __mavlink_gyro_t {
 int16_t gx; /*<  x 方向角速度*/
 int16_t gy; /*<  y 方向角速度*/
 int16_t gz; /*<  z 方向角速度*/
 int16_t ax; /*<  x 方向加速度*/
 int16_t ay; /*<  y 方向加速度*/
 int16_t az; /*<  z 方向加速度*/
} mavlink_gyro_t;

#define MAVLINK_MSG_ID_GYRO_LEN 12
#define MAVLINK_MSG_ID_GYRO_MIN_LEN 12
#define MAVLINK_MSG_ID_2_LEN 12
#define MAVLINK_MSG_ID_2_MIN_LEN 12

#define MAVLINK_MSG_ID_GYRO_CRC 241
#define MAVLINK_MSG_ID_2_CRC 241



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_GYRO { \
    2, \
    "GYRO", \
    6, \
    {  { "gx", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_gyro_t, gx) }, \
         { "gy", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_gyro_t, gy) }, \
         { "gz", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_gyro_t, gz) }, \
         { "ax", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_gyro_t, ax) }, \
         { "ay", NULL, MAVLINK_TYPE_INT16_T, 0, 8, offsetof(mavlink_gyro_t, ay) }, \
         { "az", NULL, MAVLINK_TYPE_INT16_T, 0, 10, offsetof(mavlink_gyro_t, az) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_GYRO { \
    "GYRO", \
    6, \
    {  { "gx", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_gyro_t, gx) }, \
         { "gy", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_gyro_t, gy) }, \
         { "gz", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_gyro_t, gz) }, \
         { "ax", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_gyro_t, ax) }, \
         { "ay", NULL, MAVLINK_TYPE_INT16_T, 0, 8, offsetof(mavlink_gyro_t, ay) }, \
         { "az", NULL, MAVLINK_TYPE_INT16_T, 0, 10, offsetof(mavlink_gyro_t, az) }, \
         } \
}
#endif

/**
 * @brief Pack a gyro message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param gx  x 方向角速度
 * @param gy  y 方向角速度
 * @param gz  z 方向角速度
 * @param ax  x 方向加速度
 * @param ay  y 方向加速度
 * @param az  z 方向加速度
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gyro_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GYRO_LEN];
    _mav_put_int16_t(buf, 0, gx);
    _mav_put_int16_t(buf, 2, gy);
    _mav_put_int16_t(buf, 4, gz);
    _mav_put_int16_t(buf, 6, ax);
    _mav_put_int16_t(buf, 8, ay);
    _mav_put_int16_t(buf, 10, az);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GYRO_LEN);
#else
    mavlink_gyro_t packet;
    packet.gx = gx;
    packet.gy = gy;
    packet.gz = gz;
    packet.ax = ax;
    packet.ay = ay;
    packet.az = az;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GYRO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GYRO;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
}

/**
 * @brief Pack a gyro message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param gx  x 方向角速度
 * @param gy  y 方向角速度
 * @param gz  z 方向角速度
 * @param ax  x 方向加速度
 * @param ay  y 方向加速度
 * @param az  z 方向加速度
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gyro_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   int16_t gx,int16_t gy,int16_t gz,int16_t ax,int16_t ay,int16_t az)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GYRO_LEN];
    _mav_put_int16_t(buf, 0, gx);
    _mav_put_int16_t(buf, 2, gy);
    _mav_put_int16_t(buf, 4, gz);
    _mav_put_int16_t(buf, 6, ax);
    _mav_put_int16_t(buf, 8, ay);
    _mav_put_int16_t(buf, 10, az);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GYRO_LEN);
#else
    mavlink_gyro_t packet;
    packet.gx = gx;
    packet.gy = gy;
    packet.gz = gz;
    packet.ax = ax;
    packet.ay = ay;
    packet.az = az;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GYRO_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_GYRO;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
}

/**
 * @brief Encode a gyro struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param gyro C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gyro_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_gyro_t* gyro)
{
    return mavlink_msg_gyro_pack(system_id, component_id, msg, gyro->gx, gyro->gy, gyro->gz, gyro->ax, gyro->ay, gyro->az);
}

/**
 * @brief Encode a gyro struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param gyro C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gyro_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_gyro_t* gyro)
{
    return mavlink_msg_gyro_pack_chan(system_id, component_id, chan, msg, gyro->gx, gyro->gy, gyro->gz, gyro->ax, gyro->ay, gyro->az);
}

/**
 * @brief Send a gyro message
 * @param chan MAVLink channel to send the message
 *
 * @param gx  x 方向角速度
 * @param gy  y 方向角速度
 * @param gz  z 方向角速度
 * @param ax  x 方向加速度
 * @param ay  y 方向加速度
 * @param az  z 方向加速度
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_gyro_send(mavlink_channel_t chan, int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_GYRO_LEN];
    _mav_put_int16_t(buf, 0, gx);
    _mav_put_int16_t(buf, 2, gy);
    _mav_put_int16_t(buf, 4, gz);
    _mav_put_int16_t(buf, 6, ax);
    _mav_put_int16_t(buf, 8, ay);
    _mav_put_int16_t(buf, 10, az);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GYRO, buf, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
#else
    mavlink_gyro_t packet;
    packet.gx = gx;
    packet.gy = gy;
    packet.gz = gz;
    packet.ax = ax;
    packet.ay = ay;
    packet.az = az;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GYRO, (const char *)&packet, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
#endif
}

/**
 * @brief Send a gyro message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_gyro_send_struct(mavlink_channel_t chan, const mavlink_gyro_t* gyro)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_gyro_send(chan, gyro->gx, gyro->gy, gyro->gz, gyro->ax, gyro->ay, gyro->az);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GYRO, (const char *)gyro, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
#endif
}

#if MAVLINK_MSG_ID_GYRO_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_gyro_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int16_t gx, int16_t gy, int16_t gz, int16_t ax, int16_t ay, int16_t az)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_int16_t(buf, 0, gx);
    _mav_put_int16_t(buf, 2, gy);
    _mav_put_int16_t(buf, 4, gz);
    _mav_put_int16_t(buf, 6, ax);
    _mav_put_int16_t(buf, 8, ay);
    _mav_put_int16_t(buf, 10, az);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GYRO, buf, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
#else
    mavlink_gyro_t *packet = (mavlink_gyro_t *)msgbuf;
    packet->gx = gx;
    packet->gy = gy;
    packet->gz = gz;
    packet->ax = ax;
    packet->ay = ay;
    packet->az = az;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GYRO, (const char *)packet, MAVLINK_MSG_ID_GYRO_MIN_LEN, MAVLINK_MSG_ID_GYRO_LEN, MAVLINK_MSG_ID_GYRO_CRC);
#endif
}
#endif

#endif

// MESSAGE GYRO UNPACKING


/**
 * @brief Get field gx from gyro message
 *
 * @return  x 方向角速度
 */
static inline int16_t mavlink_msg_gyro_get_gx(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  0);
}

/**
 * @brief Get field gy from gyro message
 *
 * @return  y 方向角速度
 */
static inline int16_t mavlink_msg_gyro_get_gy(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  2);
}

/**
 * @brief Get field gz from gyro message
 *
 * @return  z 方向角速度
 */
static inline int16_t mavlink_msg_gyro_get_gz(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Get field ax from gyro message
 *
 * @return  x 方向加速度
 */
static inline int16_t mavlink_msg_gyro_get_ax(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  6);
}

/**
 * @brief Get field ay from gyro message
 *
 * @return  y 方向加速度
 */
static inline int16_t mavlink_msg_gyro_get_ay(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  8);
}

/**
 * @brief Get field az from gyro message
 *
 * @return  z 方向加速度
 */
static inline int16_t mavlink_msg_gyro_get_az(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  10);
}

/**
 * @brief Decode a gyro message into a struct
 *
 * @param msg The message to decode
 * @param gyro C-struct to decode the message contents into
 */
static inline void mavlink_msg_gyro_decode(const mavlink_message_t* msg, mavlink_gyro_t* gyro)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    gyro->gx = mavlink_msg_gyro_get_gx(msg);
    gyro->gy = mavlink_msg_gyro_get_gy(msg);
    gyro->gz = mavlink_msg_gyro_get_gz(msg);
    gyro->ax = mavlink_msg_gyro_get_ax(msg);
    gyro->ay = mavlink_msg_gyro_get_ay(msg);
    gyro->az = mavlink_msg_gyro_get_az(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_GYRO_LEN? msg->len : MAVLINK_MSG_ID_GYRO_LEN;
        memset(gyro, 0, MAVLINK_MSG_ID_GYRO_LEN);
    memcpy(gyro, _MAV_PAYLOAD(msg), len);
#endif
}
