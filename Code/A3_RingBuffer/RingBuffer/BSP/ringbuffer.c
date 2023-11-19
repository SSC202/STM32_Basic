#include "ringbuffer.h"
#include "assert.h"

/**************函数定义****************/

/**
 * @brief   环形缓冲区状态读取函数
 * @param   pbuf    环形缓冲区指针
 * @retval  环形缓冲区状态
 */
static Ring_Buffer_State Ring_Buffer_State_Get(Ring_Buffer_t pbuf)
{
    if (pbuf->read_pos == pbuf->write_pos) // 读写指针为相同位置，此时环形缓冲区仅能为空和满两种状态，此时将使用镜像位
    {
        if (pbuf->read_mirror == pbuf->write_mirror) {
            return RINGBUFFER_EMPTY; // 如果读写指针镜像位相同，则缓冲区为空，反之则满
        } else {
            return RINGBUFFER_FULL;
        }
    } else {
        return RINGBUFFER_HALFFULL;
    }
}

/**
 * @brief   环形缓冲区占用长度读取函数
 * @param   pbuf    环形缓冲区指针
 * @retval  环形缓冲区占用长度
 */
static int16_t Ring_Buffer_Full_Get(Ring_Buffer_t pbuf)
{
    switch (Ring_Buffer_State_Get(pbuf)) {
        case RINGBUFFER_EMPTY:
            return 0;
        case RINGBUFFER_FULL:
            return pbuf->size;
        case RINGBUFFER_HALFFULL:
            if (pbuf->read_mirror == pbuf->write_mirror) {
                return (pbuf->write_pos - pbuf->read_pos); // 如果镜像位相同，说明写指针在读指针之后
            } else {
                return (pbuf->size - (pbuf->read_pos - pbuf->write_pos)); // 如果镜像位不同，说明读指针在写指针之后
            }
        default:
            return -1;
    }
}

#define Ring_Buffer_Empty_Get(pbuf) ((pbuf)->size - Ring_Buffer_Full_Get(pbuf))

/**
 *  @brief  环形缓冲区读取单字节函数
 */
int32_t Ring_Buffer_Read_OneByte(Ring_Buffer_t pbuf, uint8_t *ch)
{
    assert(pbuf != NULL);
    // 首先判定是否为空
    if (!Ring_Buffer_Full_Get(pbuf)) {
        return 0; // 空缓冲区，无法读取
    }
    *ch = pbuf->buf[pbuf->read_pos];

    if (pbuf->read_pos == pbuf->size - 1) // 读取完毕，读指针前进一位
    {
        pbuf->read_mirror = ~pbuf->read_mirror;
        pbuf->read_pos    = 0;
    } else {
        pbuf->read_pos++;
    }
    return 1;
}

/**
 *  @brief  环形缓冲区写入单字节函数
 */
int32_t Ring_Buffer_Write_OneByte(Ring_Buffer_t pbuf, const uint8_t ch)
{
    assert(pbuf != NULL);
    // 首先判定是否为满
    if (!Ring_Buffer_Empty_Get(pbuf)) {
        return 0; // 满缓冲区，无法写入
    }
    pbuf->buf[pbuf->write_pos] = ch;

    if (pbuf->write_pos == pbuf->size - 1) // 读取完毕，写指针前进一位
    {
        pbuf->write_mirror = ~pbuf->write_mirror;
        pbuf->write_pos    = 0;
    } else {
        pbuf->write_pos++;
    }
    return 1;
}

/**
 *  @brief  环形缓冲区读单字节函数
 */
int32_t Ring_Buffer_Peek_OneByte(Ring_Buffer_t pbuf, uint8_t *ch)
{
    assert(pbuf != NULL);
    // 首先判定是否为空
    if (Ring_Buffer_State_Get(pbuf) == RINGBUFFER_EMPTY) {
        return 0; // 空缓冲区，无法读取
    }
    *ch = pbuf->buf[pbuf->read_pos];

    return 1;
}