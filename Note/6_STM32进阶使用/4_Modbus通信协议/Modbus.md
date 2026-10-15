# STM32 进阶 4_Modbus协议

Modbus 官网： www.modbus.org

## 1. Modbus 协议简介

Modbus 是工业界常用的一种通讯协议。

Modbus 协议于 1979 年由 Modicon 公司（现被 Schneider 公司收购）设计开发，一经面世因其简单开放的通信方式逐渐成为工业系统中流行的标准，是全球第一个真正用于工业现场的总线协议。后来，为了更好地普及和推动 Modbus 基于以太网（TCP/IP）的分布式应用，施耐德公司将 Modbus 协议的所有权移交给 IDA（Interface for Distributed Automation，分布式自动化接口）组织，并成立了 Modbu-IDA 国际组织，负责推广 Modbus 标准以及对 Modbus 产品进行认证。

![NULL](./assets/picture_1.jpg)

Modbus 协议允许在各种网络体系结构内进行简单通信。

![NULL](./assets/picture_2.jpg)

> - 每种设备（PLC、HMI、控制面板、驱动程序、动作控制、输入/输出设备）都能使用 Modbus 协议来启动远程操作。
> - 在基于串行链路和以太 TCP/IP 网络的 Modbus 上可以进行相同通信。
> - 一些网关允许在几种使用 Modbus 协议的总线或网络之间进行通信。

## 2. Modbus 协议内容

### Modbus 通信帧格式

Modbus 协议定义了一个与基础通信层无关的简单协议数据单元（PDU）。特定总线或网络上的 Modbus 协议映射能够在应用数据单元（ADU）上引入一些附加域。

![NULL](./assets/picture_4.jpg)

> - **地址域：**子节点地址，主节点通过将子节点的地址放到报文的地址域对子节点寻址。当子节点返回应答时， 它将自己的地址放到应答报文的地址域以让主节点知道哪个子节点在回答。
> - **功能码：**指明服务器要执行的动作。
> - **数据域：**功能码后面的有表示含有请求和响应参数的数据。
> - **错误检验：**是对报文内容执行冗余校验的计算结果。根据不同的传输模式使用不同的计算方法。

### Modbus 数据模型

一个 Modbus 设备内可以有存在寄存器地址的数据块：

| 数据块     | 数据类型        | 访问权限 | 内容                         |
| ---------- | --------------- | -------- | ---------------------------- |
| 离散量输入 | bool            | 只读     | I/O 系统提供这种类型数据     |
| 线圈       | bool            | 读写     | 通过应用程序改变这种类型数据 |
| 输入寄存器 | 无符号2字节整型 | 只读     | I/O 系统提供这种类型数据     |
| 保持寄存器 | 无符号2字节整型 | 读写     | 通过应用程序改变这种类型数据 |

> - 线圈可读可写，还是布尔类型的，它可以表示控制现场的开关，1表示开关的打开，0表示开关的关闭。控制开关的开闭可以这么表示，同样的读取回来的值也可以表示开关的开闭状态。
> - 离散量输入也是布尔型的，但是只能读取，不支持写入。所以它可以表示控制现场中的信号灯的状态，主设备通过查询这些bit的值从而获知信号灯的状态。
> - 输入寄存器是只读的，占有两个字节的数据量。所以可以用于存储一些系统的信息，方便主设备随时查询，从而获知从设备的状态。
> - 保持寄存器可读可写，也是两个字节的数据量。所以可以接收来自主设备的控制数据，也可以向主设备返回从设备的控制数据。

这些数据块的寄存器地址编号范围为 0 - 65535：

| 寄存器地址前缀 | 编号范围（1-65536） | 编号范围（1-9999） | 数据区块   |
| -------------- | ------------------- | ------------------ | ---------- |
| 0              | 000001-065536       | 00001-09999        | 线圈       |
| 1              | 100001-165536       | 10001-19999        | 离散量输入 |
| 3              | 300001-365536       | 30001-39999        | 输入寄存器 |
| 4              | 400001-465536       | 40001-49999        | 保持寄存器 |

### Modbus 功能码

Modbus的功能码分为三类：公共功能码、用户定义功能码、保留功能码。

![NULL](./assets/picture_20.jpg)

> - 公共功能码：公共功能码是已经被定义好的功能码。
> - 用户定义功能码：用户定义功能码有两个范围，65 - 72和 100 - 110。
> - 保留功能码：可以用于某些公司或者应用使用的功能码，这部分功能码对公共使用是无效的。

- 公共功能码

  ![NULL](./assets/picture_21.jpg)

  - (0x01)读线圈

    在一个远程设备中，使用该功能码读取线圈的 1 至 2000 连续状态。

    ![NULL](./assets/picture_22.jpg)

    ![NULL](./assets/picture_23.jpg)

  - (0x02)读离散量输入

    在一个远程设备中，使用该功能码读取离散量输入的 1 至 2000 连续状态。
    
    ![NULL](./assets/picture_24.jpg)
    
  - (0x03)读保持寄存器
  
    在一个远程设备中，使用该功能码读取保持寄存器连续块的内容。
    
    ![NULL](./assets/picture_25.jpg)
    
    ![NULL](./assets/picture_26.jpg)
    
  - (0x04)读输入寄存器

    在一个远程设备中，使用该功能码读取 1 至大约 125 的连续输入寄存器。
  
    ![NULL](./assets/picture_27.jpg)
  
    ![NULL](./assets/picture_28.jpg)
  
  - (0x05)写单个线圈
  
    在一个远程设备上，使用该功能码写单个输出为 ON 或 OFF。
  
    ![NULL](./assets/picture_29.jpg)
  
  - (0x06)写单个寄存器
  
    在一个远程设备中，使用该功能码写单个保持寄存器。
  
    ![NULL](./assets/picture_30.jpg)
  
    ![NULL](./assets/picture_31.jpg)

  - (0x0F) 写多个线圈
  
    在一个远程设备中，使用该功能码强制线圈序列中的每个线圈为 ON 或 OFF。
  
    ![NULL](./assets/picture_32.jpg)
  
  - (0x10) 写多个寄存器
  
    在一个远程设备中，使用该功能码写连续寄存器块。
  
    ![NULL](./assets/picture_33.jpg)
  
    
  

## 3. Modbus 串行链路协议内容

### Modbus 通信模式

Modbus 是一种串行链路上的主从协议，在通信线路上只能有一个主机存在，不会有多主机存在的情况。虽然主机只有一个，但是从机是可以有多个的。

Modbus 的通信过程都是由主机发起的，从机在接收到主机的请求后再进行响应，从机不会主动进行数据的发送。并且从机之间也不会互相发送数据。

- Modbus 地址

  Modbus中的每个从机为了能在通信时被主机识别，都会有一个地址，也称为从节点地址。从节点地址对从设备而言必须是唯一的，在总线上也只能有唯一的一个设备地址，即从机设备的地址在总线上也不能冲突。

  从机是有唯一的设备地址的，而主机本身是没有地址的。

  Modbus支持的寻址空间为256个：

  | 0        | 1-247      | 248-255 |
  | -------- | ---------- | ------- |
  | 广播地址 | 子节点地址 | 保留    |

  > 0是广播地址，所有的子节点都必须要能够识别广播地址。

- 单播模式和广播模式

  1. **单播模式：**主机要访问某个子节点的时候，是通过子节点的地址进行的一对一的访问方式。子节点在收到主节点发过来的请求以后，根据请求的类型完成相应处理后，子节点会向主节点返回一个报文。

     ![NULL](./assets/picture_5.jpg)

     在单播模式下，一个 Modbus 的事务它包含了两个报文，一个是由主机主动发出的请求报文，另一个是由从机返回的应答报文。

  2. **广播模式：**广播模式是主机用于向总线上的所有的从机发送请求的指令。主机的广播指令一般都是用于写命令的，对于来自于主机的广播指令，从机是不需要进行应答的。

     ![NULL](./assets/picture_6.jpg)

     广播模式是单向的，主机发送出广播指令，而从机不再对广播指令进行应答，数据流只在一个方向上流动。

     总线上所有的从机都必须要能够识别主机的广播指令，并且地址0是用于广播的，不能作为从机的地址。

- 主从机的状态特征

  - 主机状态：

    ![NULL](./assets/picture_7.jpg)

    > 1. 初始上电后，主机处于空闲状态，主机在空闲状态时可以发送请求，发送请求后，主机离开空闲状态，进入忙状态，此时不能再发送其他的请求。
    > 2. 当单播请求发送到一个从机上，主机将进入等待应答状态， 同时一个临界超时定时计数器会启动。这个超时称为响应超时。 它避免主节点永远处于等待应答状态。响应超时的时间依赖于实际的应用场景。
    > 3. 当收到一个应答时，主节点在处理数据之前检验应答。在某些情况下，检验的结果可能为错误。如收到来自非期望的子节点的应答，或接收的帧错误，响应超时继续计时；当检测到帧错时，可以执行一个重试。
    > 4. 响应超时但没有收到应答时，则产生一个错误。那么主节点会重新进入空闲状态，并发出一个重试请求。重试的最大次数取决于主节点的设置。
    > 5. 当广播请求发送到串行总线上，没有响应从子节点返回。然而主节点需要进行延迟以便使子节点在发送新的请求处理完当前请求。该延迟被称作转换延迟。因此，主节点会在返回能够发送另一个请求的空闲状态之前，到等待转换延迟状态。
    > 6. 在单播方式，响应超时必须设置到足够的长度以使任何子节点都能处理完请求并返回响应。而广播转换延迟必须有足够的长度以使任何子节点都能只处理完请求而可以接收新的请求。因此，转换延迟应该比响应超时要短。

  - 从机状态：

    ![NULL](./assets/picture_8.jpg)

    > 1. 初始上电后，从机处于空闲状态；
    > 2. 当收到一个请求时，子节点在处理请求中要求的动作前要先检验报文包。当检测到错误时，必须向主节点发送应答告知错误。
    > 3. 当要求的动作完成后，单播报文要求必须按格式应答主节点。如果子节点在接收到的帧中检测到错误， 则没有响应返回到主节点。
    > 4. 任何子节点均应该定义并管理 Modbus 诊断计数器以提供诊断信息。通过使用 Modbus 诊断功能码，可以得到这些计数值。

  - 通信时序：

    ![NULL](./assets/picture_9.jpg)

### Modbus 通信帧格式

Modbus 串行链路定义两种通信模式：**RTU 模式和 ASCII 模式**；定义了报文域的位内容在线路上串行的传送，确定了信息如何打包为报文和解码。

Modbus 串行链路上所有设备的传输模式必须相同。**所有设备必须实现 RTU 模式**，ASCII 模式是备选项。

#### RTU 模式

当设备使用 RTU (Remote Terminal Unit) 模式在 Modbus 串行链路通信， 报文中每个8位字节含有两个4位十六进制字符。这种模式的主要优点是较高的数据密度，在相同的波特率下比 ASCII 模式有更高的吞吐率。每个报文必须以连续的字符流传送。

- RTU 帧格式和字节定义

    **编码系统：**8位二进制报文中每个8位字节含有两个4位十六进制字符(0–9， A–F)；

    **字节定义：**1位起始位；8位数据位，首先发送最低有效位；1位作为奇偶校验位；1位停止位。

    > 偶校验是要求的，其它模式(奇校验，无校验)也可以使用。为了保证与其它产品的最大兼容性，同时支持无校验模式是建议的。默认校验模式模式必须为偶校验。
    >
    > ![NULL](./assets/picture_10.jpg)
    >
    > ![NULL](./assets/picture_11.jpg)

    **校验方式：**循环冗余校验（CRC）。

    **帧格式：**

    ![NULL](./assets/picture_12.jpg)
    
- RTU Modbus 报文

    由发送设备将 Modbus 报文构造为带有已知起始和结束标记的帧。这使设备可以在报文的开始接收新帧，并且知道何时报文结束。不完整的报文必须能够被检测到而错误标志必须作为结果被设置。

    ![NULL](./assets/picture_13.jpg)

    整个报文帧必须以连续的字符流发送。如果两个字符之间的空闲间隔大于1.5个字符时间，则报文帧被认为不完整应该被接收节点丢弃。

    ![NULL](./assets/picture_14.jpg)

    > RTU 接收驱动程序的实现，由于1.5字符和3.5字符的定时，隐含着大量的对中断的管理。在高通信速率下，这导致 CPU 负担加重。因此，在通信速率等于或低于 19200 Bps 时，这两个定时必须严格遵守；对于波特率大于 19200 Bps 的情形，应该使用 2 个定时的固定值：建议的字符间超时时间为 750µs，帧间的超时时间为 1.750ms。

    ![NULL](./assets/picture_15.jpg)
    
    > 1. 从初始态到空闲态转换需要 3.5 字节定时超时，用以保证帧间延迟。
    > 2. 空闲态是没有发送和接收报文要处理的正常状态。
    > 3. 在 RTU 模式，当没有活动的传输的时间间隔达 3.5 字节长时，通信链路被认为在空闲态。
    > 4. 当链路空闲时，在链路上检测到的任何传输的字符被识别为 帧起始。链路变为活动状态。然后，当链路上没有字符传输的时间间个达到 3.5 字节后，被识别为帧结束。
    > 5. 检测到帧结束后，完成 CRC 计算和检验。然后，分析地址域以确定帧是否发往此设备，如果不是，则丢弃此帧。为了减少接收处理时间，地址域可以在一接到就分析，而不需要等到整个帧结束。这样，CRC 计算只需要在帧寻址到该节点 (包括广播帧) 时进行。

#### ASCII 模式

当 Modbus 串行链路的设备被配置为使用 ASCII 模式通信时，报文中的每个 8 位子节以两个 ASCII 字符发送。当通信链路或者设备无法符合 RTU 模式的定时管理时使用该模式。

- ASCII 帧格式和字节定义

    **编码系统：**十六进制，ASCII 字符 0-9，A-F。报文中每个 ASCII 字符含有 1 个十六进制字符。
    
    **字节定义：**1位起始位；7位数据位，首先发送最低有效位；1位作为奇偶校验位；1位停止位。
    
    > 偶校验是要求的，其它模式(奇校验，无校验)也可以使用。为了保证与其它产品的最大兼容性，同时支持无校验模式是建议的。默认校验模式模式必须为偶校验。
    >
    > ![NULL](./assets/picture_16.jpg)
    >
    > ![NULL](./assets/picture_17.jpg)
    
    **校验方式：**纵向冗余校验(LRC)
    
- ASCII Modbus 报文

    ![NULL](./assets/picture_18.jpg)

    > ASCII报文帧中用冒号(`:`)(ASCII的十六进制为`0x3A`)作为起始；用回车换行(`CR` `LF`)(ASCII的十六进制为`0x0D` `0x0A`)作为结束。

    ![NULL](./assets/picture_19.jpg)

## 4. FreeModbus 从机移植

FreeModbus 是一个开源的 Modbus 通信协议栈实现。它允许开发者在各种平台上轻松地实现 Modbus 通信功能，包括串口和以太网。FreeModbus 提供了用于从设备和主站通信的功能，支持 Modbus RTU 和 Modbus TCP 协议。

**FreeModbus 官方只开源了 Modbus 从机源码。**

[FreeMODBUS Downloads - Embedded Experts](https://www.embedded-experts.at/en/freemodbus-downloads/)

FreeModbus 文件夹如下：

![NULL](./assets/picture_34.jpg)

> `demo`: 接口文件的模板和了一些示例代码
>
> `doc`: FreeModBus的说明文档
>
> `modbus`: ModBus 源码
>
> `tools`: 测试 ModBus 工具，一般使用 Modbus Poll 工具调试 ModBus。

modbus 文件夹如下：

![NULL](./assets/picture_35.jpg)

> `ascii`: ModBus ASCII 源文件
>
> `functions`: ModBus 源码函数文件
>
> `include`: 相关的头文件
>
> `rtu`: ModBus RTU 文件
>
> `tcp`: ModBus TCP 文件

### FreeModbus 裸机移植

[参考资料](https://blog.csdn.net/qq_40305944/article/details/107447042)

移植 `demo/BARE/port` 和 `modbus` 中的 `functions`，`include` ，`rtu` 和 `mb.c`。

<font color=LightGreen>1. 修改`modbus\include\mbconfig.h`文件</font> 

```c
/* ----------------------- Defines ------------------------------------------*/
/*! \defgroup modbus_cfg Modbus Configuration
 *
 * Most modules in the protocol stack are completly optional and can be
 * excluded. This is specially important if target resources are very small
 * and program memory space should be saved.<br>
 *
 * All of these settings are available in the file <code>mbconfig.h</code>
 */
/*! \addtogroup modbus_cfg
 *  @{
 */

// 选择 ASCII/RTU/TCP 协议
/*! \brief If Modbus ASCII support is enabled. */
#define MB_ASCII_ENABLED                        (  0 )

/*! \brief If Modbus RTU support is enabled. */
#define MB_RTU_ENABLED                          (  1 )

/*! \brief If Modbus TCP support is enabled. */
#define MB_TCP_ENABLED                          (  0 )
```

```c
/*! \brief Number of bytes which should be allocated for the <em>Report Slave ID
 *    </em>command.
 *
 * This number limits the maximum size of the additional segment in the
 * report slave id function. See eMBSetSlaveID(  ) for more information on
 * how to set this value. It is only used if MB_FUNC_OTHER_REP_SLAVEID_ENABLED
 * is set to <code>1</code>.
 */
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           ( 32 )

// 选择启用的功能码
/*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       (  0 )

/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED              (  0 )

/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED            (  1 )

/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED           (  1 )

/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  (  1 )

/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED              (  0 )

/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED              (  0 )

/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    (  0 )

/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    (  0 )

/*! \brief If the <em>Read/Write Multiple Registers</em> function should be enabled. */
#define MB_FUNC_READWRITE_HOLDING_ENABLED       (  0 )
```

<font color=LightGreen>2. 串口硬件移植`portserial.c`</font>

 ```c
 void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
 {
     /* If xRXEnable enable serial receive interrupts. If xTxENable enable
      * transmitter empty interrupts.
      */
     // 启用接收中断
     if (xRxEnable) {
         __HAL_UART_ENABLE_IT(&UART_PORT, UART_IT_RXNE);
         RS485_Set(0);
     } else {
         __HAL_UART_DISABLE_IT(&UART_PORT, UART_IT_RXNE);
         RS485_Set(1);
     }
     // 启用发送完成中断
     if (xTxEnable) {
         RS485_Set(1);
         __HAL_UART_ENABLE_IT(&UART_PORT, UART_IT_TC);
     } else {
         RS485_Set(0);
         __HAL_UART_DISABLE_IT(&UART_PORT, UART_IT_TC);
     }
 }
 ```

> 如果使用 `UART_IT_TXE` 中断，不会出现发送问题。但是使用 `UART_IT_TC` 中断时，由于为发送完成中断，需要修改 `eMBRTUSend()` 函数：
>
> ```c
> eMBErrorCode
> eMBRTUSend(UCHAR ucSlaveAddress, const UCHAR *pucFrame, USHORT usLength)
> {
>     eMBErrorCode eStatus = MB_ENOERR;
>     USHORT usCRC16;
> 
>     ENTER_CRITICAL_SECTION();
> 
>     /* Check if the receiver is still in idle state. If not we where to
>      * slow with processing the received frame and the master sent another
>      * frame on the network. We have to abort sending the frame.
>      */
>     if (eRcvState == STATE_RX_IDLE) {
>         /* First byte before the Modbus-PDU is the slave address. */
>         pucSndBufferCur  = (UCHAR *)pucFrame - 1;
>         usSndBufferCount = 1;
> 
>         /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
>         pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
>         usSndBufferCount += usLength;
> 
>         /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
>         usCRC16                      = usMBCRC16((UCHAR *)pucSndBufferCur, usSndBufferCount);
>         ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 & 0xFF);
>         ucRTUBuf[usSndBufferCount++] = (UCHAR)(usCRC16 >> 8);
> 
>         /* Activate the transmitter. */
>         eSndState = STATE_TX_XMIT;
>         /**** User begin ****/
>         // 发送一次触发发送完成中断
>         xMBPortSerialPutByte((CHAR)*pucSndBufferCur);
>         pucSndBufferCur++; /* next byte in sendbuffer. */
>         usSndBufferCount--;
>         /**** User end ****/
>         vMBPortSerialEnable(FALSE, TRUE);
>     } else {
>         eStatus = MB_EIO;
>     }
>     EXIT_CRITICAL_SECTION();
>     return eStatus;
> }
> ```

```c
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    // 发送一个字节
    RS485_Set(1);
    UART_PORT.Instance->DR = ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    // 接收一个字节
    RS485_Set(0);
    *pucByte = UART_PORT.Instance->DR;
    return TRUE;
}

/**
 * @brief   串口中断服务函数
 */
void USART2_IRQHandler(void)
{
    // 发送中断
    if (__HAL_UART_GET_FLAG(&UART_PORT, UART_FLAG_TC) != RESET) {
        __HAL_UART_CLEAR_FLAG(&UART_PORT, UART_FLAG_TC);
        prvvUARTTxReadyISR();
    }
    // 接收中断
    if (__HAL_UART_GET_FLAG(&UART_PORT, UART_FLAG_RXNE) != RESET) {
        __HAL_UART_CLEAR_FLAG(&UART_PORT, UART_FLAG_RXNE);
        prvvUARTRxISR();
    }
}
```

<font color=LightGreen>3. 定时器硬件移植`porttimer.c`</font>

```c
BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    // 配置 50us 的定时器
    TIM_PORT.Instance->ARR = usTim1Timerout50us - 1;
    return TRUE;
}

inline void
vMBPortTimersEnable()
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    __HAL_TIM_SET_COUNTER(&TIM_PORT, 0);
    HAL_TIM_Base_Start_IT(&TIM_PORT);
}

inline void
vMBPortTimersDisable()
{
    /* Disable any pending timers. */
    HAL_TIM_Base_Stop_IT(&TIM_PORT);
    __HAL_TIM_SET_COUNTER(&TIM_PORT, 0);
}

void TIM2_IRQHandler(void)
{
    if(__HAL_TIM_GET_FLAG(&TIM_PORT,TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&TIM_PORT,TIM_FLAG_UPDATE);
        prvvTIMERExpiredISR();
    }
}
```

> 定时器中断优先级低于串口中断优先级。

<font color=LightGreen>4. 功能码实现</font>

```c
// 输入寄存器
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
	
    usAddress -= 1;
    
    if ((usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS)) {
        iRegIndex = (int)(usAddress - usRegInputStart);
        while (usNRegs > 0) {
            *pucRegBuffer++ =
                (unsigned char)(usRegInputBuf[iRegIndex] >> 8);
            *pucRegBuffer++ =
                (unsigned char)(usRegInputBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

// 保持寄存器
eMBErrorCode
eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;
    usAddress -= 1;
    if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS)) {
        iRegIndex = (int)(usAddress - usRegHoldingStart);
        if (eMode == MB_REG_READ) {
            while (usNRegs > 0) {
                *pucRegBuffer++ =
                    (unsigned char)(usRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ =
                    (unsigned char)(usRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
        } else {
            while (usNRegs > 0) {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++;
                usRegHoldingBuf[iRegIndex] <<= 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// 线圈
eMBErrorCode
eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iNCoils          = (int)usNCoils;
    unsigned short usBitOffset;

    usAddress -= 1;
    if ((usAddress >= REG_COILS_START) && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE)) {
        usBitOffset = (unsigned short)(usAddress - REG_COILS_START);
        switch (eMode) {
            case MB_REG_READ:
                while (iNCoils > 0) {
                    *pucRegBuffer++ = xMBUtilGetBits(ucRegCoilsBuf, usBitOffset, (unsigned char)(iNCoils > 8 ? 8 : iNCoils));
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
            case MB_REG_WRITE:
                while (iNCoils > 0) {
                    xMBUtilSetBits(ucRegCoilsBuf, usBitOffset, (unsigned char)(iNCoils > 8 ? 8 : iNCoils), *pucRegBuffer++);
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// 离散量输入
eMBErrorCode
eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode eStatus = MB_ENOERR;
    short iNDiscrete     = (short)usNDiscrete;
    USHORT usBitOffset;

    usAddress -= 1;
    if ((usAddress >= REG_DISCRETE_START) && (usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE)) {
        usBitOffset = (USHORT)(usAddress - REG_DISCRETE_START);
        while (iNDiscrete > 0) {
            *pucRegBuffer++ =
                xMBUtilGetBits(usRegDiscreteBuf, usBitOffset, (UCHAR)(iNDiscrete > 8 ? 8 : iNDiscrete));
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
```

> 输入的地址减一主要是避免 FreeModbus 源码中的地址自增。

<font color=LightGreen>5. 主程序</font>

```c
// Modbus 初始化：RTU模式，从机地址 0x01，端口0，波特率9600，无校验    
eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE);
// Modbus 使能
eMBEnable();
// Modbus 轮询，此函数更新从机所有寄存器的状态
eMBPoll();
```

### FreeModbus FreeRTOS 移植

移植过程和裸机移植相似，不同点如下：

- 临界段代码（`port.h`）

  ```c
  #define ENTER_CRITICAL_SECTION( )   taskENTER_CRITICAL()
  #define EXIT_CRITICAL_SECTION( )    taskEXIT_CRITICAL()
  ```

- (可选)使用队列实现 FreeModbus 内的事件(`portevent.c`)

  ```c
  /* ----------------------- Variables ----------------------------------------*/
  static osMessageQId xQueueHdl; // 队列句柄
  
  /* ----------------------- Start implementation -----------------------------*/
  // 判断是否在中断内
  BOOL bMBPortIsWithinException(void) {
      return (BOOL)xPortIsInsideInterrupt();
  }
  
  BOOL xMBPortEventInit(void)
  {
      BOOL bStatus = FALSE;
      xQueueHdl    = osMessageQueueNew(1, sizeof(eMBEventType), NULL);
      if (xQueueHdl != NULL) {
          bStatus = TRUE;
      }
      return bStatus;
  }
  
  void vMBPortEventClose(void)
  {
      if (xQueueHdl != NULL) {
          osMessageQueueDelete(xQueueHdl);
          xQueueHdl = NULL;
      }
  }
  
  BOOL xMBPortEventPost(eMBEventType eEvent)
  {
      BOOL bStatus = TRUE;
      if (bMBPortIsWithinException()) {
          BaseType_t xHigherPriorityTaskWoken = pdFALSE;
          if (xQueueSendFromISR((QueueHandle_t)xQueueHdl, (const void *)&eEvent, &xHigherPriorityTaskWoken) == pdPASS) {
              portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
          } else {
              bStatus = FALSE;
          }
      } else {
          osStatus_t status = osMessageQueuePut(xQueueHdl, &eEvent, 0, 0);
          if (status != osOK) {
              bStatus = FALSE;
          }
      }
      return bStatus;
  }
  
  BOOL xMBPortEventGet(eMBEventType *peEvent)
  {
      BOOL xEventHappened = FALSE;
      uint32_t timeout = osKernelGetTickCount() + 50;
      osStatus_t status   = osMessageQueueGet(xQueueHdl, peEvent, NULL, timeout);
      if (status == osOK) {
          xEventHappened = TRUE;
      }
  
      return xEventHappened;
  }
  ```


## 5. FreeModbus 主机移植

FreeModbus 官方未提供主机源码，主机源码由 armink 后续开发得到。

[FreeModbus 主机仓库](https://github.com/armink/FreeModbus_Slave-Master-RTT-STM32)

该仓库使用 RT-Thread 移植，这里使用 HAL 库进行主机移植。

HAL 库移植参考：[Alidong/HAL_FreeRTOS_Modbus](https://github.com/Alidong/HAL_FreeRTOS_Modbus)

注意事项：

1. CubeMX 生成

   ![NULL](./assets/picture_36.jpg)

   Register Callack 的 UART 和 USART 均勾选 Enable。

2. `IS_IRQ()` 修改为以下代码：

   ```c
   BOOL IS_IRQ(void)
   {
       return (BOOL)xPortIsInsideInterrupt();
   }
   ```

剩下部分保留即可，该代码采用 FIFO 发送，效率更高。
