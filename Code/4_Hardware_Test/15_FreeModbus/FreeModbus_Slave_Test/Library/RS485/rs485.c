#include "rs485.h"

/**
 * @brief   RS485 模式控制
 * @param   en  RS485芯片模式  0,接收;1,发送.
 */
void RS485_Set(uint8_t en)
{
    PCF8574_Write_Bit(RS485_RE_IO, en);
}
