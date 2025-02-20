# STM32H7 11_FDCAN

## 1. STM32H7 FDCAN简介

### FDCAN 协议

![NULL](picture_1.jpg)

- FDCAN 新增了 EDL , BRS , ESI 位，均在控制段中。

> - EDL：表示是 CAN 报文还是 CAN-FD 报文。
> - BRS：表示位速率转换，该位隐性时，速率可变（即BSR到CRC使用转换速率传输），该位为显性时，以正常的CAN-FD总线速率传输（恒定速率）。CAN FD采用了两种位速率：从控制段中的BRS位到ACK场之前（含CRC分界符）为可变速率，其余部分为原CAN总线用的速率，即仲裁段和数据控制段使用标准的通信波特率，而数据传输段时就会切换到更高的通信波特率。两种速率各有一套位时间定义寄存器，它们除了采用不同的位时间单位tq外，位时间各段的分配比例也可不同。
> - ESI：指示节点处于错误活动模式还是错误被动模式。

- FDCAN 协议速率可变，仲裁比特率最高 1Mbps（与CAN相同），数据比特率最高8Mbps，CAN-FD的传输格式如下图所示。
- FDCAN 标准帧ID长度可扩展到 12bit。

- CRC 校验加强

> - 为了避免位填充对CRC的影响，CAN FD在CRC场中增加了stuff count记录填充位的个数对应8的模，并用Grey Code表示，还增加了奇偶校验位。FSB（fixed stuff-bit）固定为前一位的补码。
>
> > Stuff Count由以下两个元素组成：
> >
> > 1. 格雷码计算：CRC区域之前的填充位数除以8，得到的余数进行格雷码计算得到的值（Bit0-2）。
> >
> >2. 奇偶校验：通过格雷码计算后的值的奇偶校验。
>
> - 为了保证信息发送的质量，CAN FD的CRC计算不仅要包括数据段的位，还包括来自SOF的Stuff Count和填充位。通过比较CRC的计算结果，可以判断接收节点是否能够正常接收。此时 CRC 位增加到 21 位。
>
> - 位填充：与CAN一样，填充位插入到SOF和数据场的末尾之间。插入的填充位数值是经过格雷码计算转换后的值，并且用奇偶校验位保护（Stuff Count）。在CRC校验场中，填充位被放置在固定的位位置，这称为固定填充位（Fixed Stuff Bit FSB）。固定填充位的值是上一位的反码。
>

### FDCAN  Message RAM

- STM32H7 自带了 10K 的消息 RAM，消息 RAM 的配置用来实现以下功能：

> 1. 过滤器
> 2. 接收 FIFO
> 3. 接收 BUFF
> 4. 发送事件 FIFO
> 5. 发送 BUFF
> 6. TTCAN
>
> > 系统 **不会** 对 Message RAM 配置进行检查。Message RAM 在 FDCAN1 和 FDCAN2 模块之间共用,，用户在配置的时候需要注意，如果配置错误将会导致发生异常情况。

![NULL](picture_2.jpg)

|寄存器	|功能	|元素大小	|描述
-|-|-|-
|SIDFC.FLSSA	|最大支持128个元素	|最多占用 128个 word	|11 bit 标准帧滤波器设置
|XIDFC.FLESA	|最大支持64个元素	|最多占用 128 个 word	|29 bit 扩展帧滤波器设置
|RXF0C.F0SA	|最大支持64个元素	|最多占用 1152 个 word	|RX FIFO0 的设置
|RXF1C.F1SA	|最大支持64个元素	|最多占用 1152 个 word	|RX FIFO1 的设置
|RXBC.RBSA	|最大支持64个元素	|最多占用 1152 个 word	|RX BUFF 的设置
|TXEFC.EFSA	|最大支持32个元素	|最多占用 64 个 word	|TX EVENT FIFO 的设置
|TXBC.TBSA	|最大支持32个元素	|最多占用 576 个 word	|TX BUFF 的设置
|TMC.TMSA	|最大支持64个元素	|最多占用 128 个 word	|TT CAN 的设置

#### Message RAM 的配置

Message RAM 的大小是 10K Bytes,也就是 2560 个 word。如果都按照最大的配置去计算则会超出 Message RAM 的可配置范围。

Message RAM 配置结构体成员如下：

```c
  uint32_t MessageRAMOffset;             /*!< Specifies the message RAM start address.
                                              This parameter must be a number between 0 and 2560           */

  uint32_t StdFiltersNbr;                /*!< Specifies the number of standard Message ID filters.
                                              This parameter must be a number between 0 and 128            */

  uint32_t ExtFiltersNbr;                /*!< Specifies the number of extended Message ID filters.
                                              This parameter must be a number between 0 and 64             */

  uint32_t RxFifo0ElmtsNbr;              /*!< Specifies the number of Rx FIFO0 Elements.
                                              This parameter must be a number between 0 and 64             */

  uint32_t RxFifo0ElmtSize;              /*!< Specifies the Data Field Size in an Rx FIFO 0 element.
                                              This parameter can be a value of @ref FDCAN_data_field_size  */

  uint32_t RxFifo1ElmtsNbr;              /*!< Specifies the number of Rx FIFO 1 Elements.
                                              This parameter must be a number between 0 and 64             */

  uint32_t RxFifo1ElmtSize;              /*!< Specifies the Data Field Size in an Rx FIFO 1 element.
                                              This parameter can be a value of @ref FDCAN_data_field_size  */

  uint32_t RxBuffersNbr;                 /*!< Specifies the number of Dedicated Rx Buffer elements.
                                              This parameter must be a number between 0 and 64             */

  uint32_t RxBufferSize;                 /*!< Specifies the Data Field Size in an Rx Buffer element.
                                              This parameter can be a value of @ref FDCAN_data_field_size  */

  uint32_t TxEventsNbr;                  /*!< Specifies the number of Tx Event FIFO elements.
                                              This parameter must be a number between 0 and 32             */

  uint32_t TxBuffersNbr;                 /*!< Specifies the number of Dedicated Tx Buffers.
                                              This parameter must be a number between 0 and 32             */

  uint32_t TxFifoQueueElmtsNbr;          /*!< Specifies the number of Tx Buffers used for Tx FIFO/Queue.
                                              This parameter must be a number between 0 and 32             */

  uint32_t TxFifoQueueMode;              /*!< Tx FIFO/Queue Mode selection.
                                              This parameter can be a value of @ref FDCAN_txFifoQueue_Mode */

  uint32_t TxElmtSize;                   /*!< Specifies the Data Field Size in a Tx Element.
                                              This parameter can be a value of @ref FDCAN_data_field_size  */
```

Message RAM 配置	|STM32 FDCAN 结构体成员	|配置举例	|RAM 占用（word）
-|-|-|-
SIDFC.FLSSA（11 bit 标准帧滤波器配置）	|`StdFiltersNbr`	|1（设置 1 个标准帧滤波器）	|1
XIDFC.FLESA（29 bit 标准帧滤波器配置）	|`ExtFiltersNbr`	|1（设置 1 个扩展帧滤波器）|	2
RXF0C.F0SA（RX FIFO0 的设置）	|`RxFifo0ElmtsNbr * RxFifo0ElmtSize`（可设置的大小是 8 ，12，16，20，24，32，48，64）	|10（深度为 10 的 RX FIFO0）* 18（每帧长度是 64 个字节）	|180
RXF1C.F1SA（RX FIFO1 的设置）|	`RxFifo1ElmtsNbr * RxFifo1ElmtSize`（可设置的大小是 8 ，12，16，20，24，32，48，64）	|10（深度为 10 的 RX FIFO0）* 18（每帧长度是 64 个字节）	|180
RXBC.RBSA（RX BUFF 的设置）	|`RxBuffersNbr * RxBufferSize`(可设置的大小是 8 ，12，16，20，24，32，48，64）	|10（ 10 个 专用RX BUFF）* 18（每帧长度是 64 个字节）	|180
TXEFC.EFSA（TX EVENT FIFO 的设置）	|`TxEventsNbr`	|1	|2
TXBC.TBSA （TX BUFF 的设置）	|`TxBuffersNbr * TxElmtSize` + `TxFifoQueueElmtsNbr * TxElmtSize`	|5 * 18（每帧长度是 64 个字节）+ 5 * 18	|180
TMC.TMSA（TT CAN 的设置）	|无	|-	|0

（HAL库会对此配置进行严格检查）

#### FDCAN Filter

FDCAN 外设可以配置两套验收滤波器：一套用于标准标识符，一种是扩展标识符，用于存储或拒绝接收到的消息。可以将这些过滤器分配给 Rx FIFO 0/1 或专用的 Rx buffers。 FDCAN 执行接受过滤，它总是从过滤器 0 开始，然后遍历过滤器列表以找到匹配项元素。验收过滤在第一个匹配元素处停止，而随后的过滤元素不在针对此消息进行检索。因此，配置过滤器元素的顺序对过滤性能有很重要的影响。用户选择启用或禁用每个过滤器元素，并可以配置每个元素以接受或拒绝过滤。

每个过滤器元素可以配置为：

> - 范围过滤器（Range filter）：该过滤器匹配标识符在两个 ID 定义的范围内的所有消息。
>
> - 专用 ID 的过滤器（Filter for dedicated IDs）：可以将过滤器配置为匹配一个或两个特定的标识符。(列表模式)
>
> - 经典位屏蔽过滤器（Classic bit mask filter）：通过对接收到的标识符的位进行屏蔽来匹配标识符组。第一个 ID 配置为消息 ID 过滤器，第二个 ID 为过滤器屏蔽。过滤器屏蔽的每个零位屏蔽已配置的 ID过滤器的相应位位置。(掩码模式)

![NULL](picture_3.jpg)

#### FDCAN Rx

可以在 CAN 消息 RAM 中配置两个 Rx FIFO。 每个 Rx FIFO 部分最多可存储 64 个元素。每个元素存储在一个 Rx FIFO 元素中。

- Rx FIFO 的起始地址是第一个 Rx FIFO 元素的第一个字的地址。收到的元素通过匹配过滤的数据将根据匹配的过滤器元素存储在适当的 Rx FIFO 中。如果 Rx FIFO 已满，则可以根据两种不同模式来处理新到达的元素：

  > 阻塞模式：这是 Rx FIFO 的默认操作模式，没有其他元素写入 RxFIFO，直到至少一个元素已被读出。
>
  > 覆盖模式：Rx FIFO 中接受的新元素将覆盖 Rx FIFO 中最旧（最先接收的数据）的元素并且 FIFO 的 put 和 get 索引加 1。

- 要从 Rx FIFO 读取元素，CPU 必须执行以下步骤：

  > - 读取寄存器 FDCAN_RXF1S 以了解 Rx FIFO 的状态。
  >
  > - 计算 RAM 中最旧的元素的地址。
  > - 从计算出的地址中读取元素。CPU 从 Rx FIFO 读取一个元素或一系列元素后，它必须确认读取。确认后，FDCAN 可以将相应的Rx FIFO 缓冲区重新用于新元素。为了确认一个或多个元素，则 CPU 必须将从 Rx FIFO 读取的最后一个元素的缓冲区索引写入 FDCAN_RXF1A 寄存器。结果，FDCAN 更新了 FIFO 填充级别和 get 索引。

![NULL](picture_4.jpg)

- FDCAN 支持多达 64 个专用 Rx buffers。每个专用的 Rx buffers 可以存储一个元素。当将元素存储在专用 Rx 缓冲区中时，FDCAN 通过 FDCAN_IR 寄存器的 DRX 位设置中断标志以及新数据标志 FDCAN_NDAT1或FDCAN_NDAT2寄存器中的相应位。当FDCAN_NDAT1 / 2中的位置1时，相应的 Rx 缓冲区将被锁定（不会被新元素覆盖），并且相应的过滤器不匹配。 读取元素后，CPU 必须将相应的位复位 FDCAN_NDAT1 / 2，以解锁相应的 Rx 缓冲区。

#### FDCAN Tx

FDCAN 提供发送 event FIFO。 该 Tx event FIFO 的使用是可选的。 FDCAN 在 CAN 总线上传输了一个元素成功后，它可以将消息 ID 和时间戳存储在 Tx event FIFO 中元素。 Tx event FIFO 元素是一种数据结构，用于存储已传输消息。

为了将 Tx event 链接到 Tx event FIFO 元素，将来自已发送 Tx buffer 的消息标记复制到 Tx event FIFO 元素。

仅当 Tx buffer 元素中的 EFC 位（存储 Tx 事件）等于 1 时，事件才存储在 Tx event FIFO 中。当 Tx event FIFO 已满时，不会再有其他元素写入 Tx event FIFO，直到至少有一个元素被读出为止。读出后，Tx event FIFO 获取索引增加。如果在 Tx event FIFO 已满时发生 Tx event，则这事件被丢弃。**为避免 Tx event FIFO 溢出，可以使用 Tx event FIFO WaterMark**。

- 为了使外设传输元素，该元素在定义的存储空间内，并且传输开始。 传输的元素存储在 Tx buffer中，用户可以选择使用的机制：专用的 Tx buffer 或 Tx queue 或 Tx FIFO。FDCAN 最多支持 32 个元素。每个元素存储标识符，DLC，控制位（ESI，XTD，RTR，BRS，FDF），数据字段，位字段消息标记和事件 FIFO 控制位，仅一条消息。

- 在 RAM 上的分配按以下顺序进行：如果应用程序使用了专用的 Tx buffer，则它们在 Tx FIFO 和 Tx queue 之前分配。 用户只能在同一队列中选择 Tx queue 或 Tx FIFO 应用程序，FDCAN 不支持它们的组合。

![NULL](picture_5.jpg)

- Tx FIFO 机制如下：

![NULL](picture_6.jpg)

- Tx Queue 机制如下：

![NULL](picture_7.jpg)

### FDCAN 测试模式

在 FDCAN 的操作模式下，除了正常操作外，还有几种测试模式可用。 那里测试模式只能用于生产测试或自测以及校准单元。必须将 FDCAN_CCCR 中的 TEST 位设置为 1，以允许对 FDCAN 测试寄存器和存储器的写访问。测试模式和功能的配置。FDCAN 以下列模式之一工作：

> - 限制操作模式 Restricted-operation mode
> - 总线监控模式 Bus-monitoring mode
> - 外部环回模式 External loop-back mode
> - 内部环回模式 Internal loop-back mode

![NULL](picture_8.jpg)

## 2. STM32H7 FDCAN HAL库函数

### 结构体简介（对应CubeMX配置）

#### FDCAN 总线初始化结构体`FDCAN_InitTypeDef`

```c
typedef struct
{
 	uint32_t FrameFormat; 				     /*!< Specifies the FDCAN frame format.This parameter can be a value of @ref FDCAN_frame_format */
 	uint32_t Mode; 							/*!< Specifies the FDCAN mode.This parameter can be a value of @ref FDCAN_operating_mode */
 	FunctionalState AutoRetransmission; 	  /*!< Enable or disable the automatic retransmission mode.This parameter can be set to ENABLE or DISABLE */
    FunctionalState TransmitPause; 			  /*!< Enable or disable the Transmit Pause feature.this parameter can be set to ENABLE or DISABLE */
 	FunctionalState ProtocolException; 		  /*!< Enable or disable the Protocol Exception Handling.This parameter can be set to ENABLE or DISABLE */
 	uint32_t NominalPrescaler; 				 /*!< Specifies the value by which the oscillator frequency is divided for generating the nominal bit time quanta.This parameter must be a number between 1 and 512 */
 	uint32_t NominalSyncJumpWidth;            /*!< Specifies the maximum number of time quanta the FDCAN hardware is allowed to lengthen or shorten a bit to perform resynchronization.This parameter must be a number between 1 and 128 */
 	uint32_t NominalTimeSeg1; 				 /*!< Specifies the number of time quanta in Bit Segment 1.This parameter must be a number between 2 and 256 */
 	uint32_t NominalTimeSeg2; 				 /*!< Specifies the number of time quanta in Bit Segment 2.This parameter must be a number between 2 and 128 */
 	uint32_t DataPrescaler; 				 /*!< Specifies the value by which the oscillator frequency is divided for generating the data bit time quanta.This parameter must be a number between 1 and 32 */
 	uint32_t DataSyncJumpWidth; 			  /*!< Specifies the maximum number of time quanta the FDCAN hardware is allowed to lengthen or shorten a data bit to perform resynchronization.This parameter must be a number between 1 and 16 */
 	uint32_t DataTimeSeg1; 					  /*!< Specifies the number of time quanta in Data Bit Segment 1.This parameter must be a number between 1 and 32 */
 	uint32_t DataTimeSeg2; 					  /*!< Specifies the number of time quanta in Data Bit Segment 2.This parameter must be a number between 1 and 16 */
 	uint32_t MessageRAMOffset; 				   /*!< Specifies the message RAM start address.This parameter must be a number between 0 and 2560 */
 	uint32_t StdFiltersNbr; 				   /*!< Specifies the number of standard Message ID filters.This parameter must be a number between 0 and 128 */
 	uint32_t ExtFiltersNbr; 				   /*!< Specifies the number of extended Message ID filters.This parameter must be a number between 0 and 64 */
 	uint32_t RxFifo0ElmtsNbr; 				   /*!< Specifies the number of Rx FIFO0 Elements.This parameter must be a number between 0 and 64 */
 	uint32_t RxFifo0ElmtSize; 				   /*!< Specifies the Data Field Size in an Rx FIFO 0 element.This parameter can be a value of @ref FDCAN_data_field_size */
	uint32_t RxFifo1ElmtsNbr; 				   /*!< Specifies the number of Rx FIFO 1 Elements.This parameter must be a number between 0 and 64 */
 	uint32_t RxFifo1ElmtSize; 				   /*!< Specifies the Data Field Size in an Rx FIFO 1 element.This parameter can be a value of @ref FDCAN_data_field_size */
    uint32_t RxBuffersNbr; 						/*!< Specifies the number of Dedicated Rx Buffer elements.This parameter must be a number between 0 and 64 */
 	uint32_t RxBufferSize;	 					/*!< Specifies the Data Field Size in an Rx Buffer element.This parameter can be a value of @ref FDCAN_data_field_size */
 	uint32_t TxEventsNbr; 						/*!< Specifies the number of Tx Event FIFO elements.This parameter must be a number between 0 and 32 */
	uint32_t TxBuffersNbr; 						/*!< Specifies the number of Dedicated Tx Buffers.This parameter must be a number between 0 and 32 */
 	uint32_t TxFifoQueueElmtsNbr; 				/*!< Specifies the number of Tx Buffers used for Tx FIFO/Queue.This parameter must be a number between 0 and 32 */
 	uint32_t TxFifoQueueMode; 					/*!< Tx FIFO/Queue Mode selection.This parameter can be a value of @ref FDCAN_txFifoQueue_Mode */
 	uint32_t TxElmtSize; 						/*!< Specifies the Data Field Size in a Tx Element.This parameter can be a value of @ref FDCAN_data_field_size */
} FDCAN_InitTypeDef;
```

- `FrameFormat`：设置帧格式

```c
#define FDCAN_FRAME_CLASSIC ((uint32_t)0x00000000U) 					  /* 经典 CAN 模式 */
#define FDCAN_FRAME_FD_NO_BRS ((uint32_t)FDCAN_CCCR_FDOE) 				  /* FD CAN 不带可变波特率 */
#define FDCAN_FRAME_FD_BRS ((uint32_t)(FDCAN_CCCR_FDOE | FDCAN_CCCR_BRSE))  /* FD CAN 带可变波特率 */
```

- `Mode`：设置CAN操作模式

```c
#define FDCAN_MODE_NORMAL ((uint32_t)0x00000000U) 						/*!< 正常模式 */
#define FDCAN_MODE_RESTRICTED_OPERATION ((uint32_t)0x00000001U) 		 /*!< 有限制的操作模式 */
#define FDCAN_MODE_BUS_MONITORING ((uint32_t)0x00000002U) 				 /*!< 总线监测模式 */
#define FDCAN_MODE_INTERNAL_LOOPBACK ((uint32_t)0x00000003U) 			 /*!< 内部环回模式 */
#define FDCAN_MODE_EXTERNAL_LOOPBACK ((uint32_t)0x00000004U) 			 /*!< 外部环回模式 */
```

- `AutoRetransmission`：使能自动重传

- `TransmitPause`：使能或者禁止传输暂停特性。
- `ProtocolException`：使能或者禁止协议异常管理。
- `Nominal Prescaler`：用于 CAN FD 仲裁阶段分频设置，产生标称位时间量，参数范围 1-512。（预分频系数）
- `NominalSyncJumpWidth`：设置 FD CAN 仲裁阶段最大支持的时间量来加长或者缩短一个 bit 来实现再同步，参数范围 1-128。（通常设置为1）
- `NominalTimeSeg1`：设置仲裁阶段 Bit Segment 1 的时间量，范围 2 – 256。（设置此值以配置波特率
- `NominalTimeSeg2`：设置仲裁阶段 Bit Segment 2 的时间量，范围 2 – 128。

- `DataPrescaler`：用于 CAN FD 数据阶段分频设置，范围 1-32。

- `DataSyncJumpWidth`：设置FD CAN数据阶段最大支持的时间量来加长或者缩短一个bit来实现数据再同步，参数范围1-16。

- `DataTimeSeg1`：设置数据阶段 Data Bit Segment 1 的时间量，范围 1 – 32。

- `DataTimeSeg2`：设置数据阶段 Data Bit Segment 2 的时间量，范围 1 – 16。

- `MessageRAMOffset`：设置消息 RAM 起始地址，范围 0 到 2560。（对于FDCAN1无需设置偏移）

- `StdFiltersNbr`：标准 ID 过滤个数，范围 0 到 128。（按照需求设置）

- `ExtFiltersNbr`：扩展 ID 过滤个数，范围 0 到 64。（按照需求设置）

- `RxFifo0ElmtsNbr`：RX FIFO0 元素个数，范围 0 到 64。（设置为32即可）

- `RxFifo0ElmtSize`：RX FIFO0 每个元素中数据大小。

 ```c
 #define FDCAN_DATA_BYTES_8 ((uint32_t)0x00000004U)  /*!< 8 bytes data field */
 #define FDCAN_DATA_BYTES_12 ((uint32_t)0x00000005U) /*!< 12 bytes data field */
 #define FDCAN_DATA_BYTES_16 ((uint32_t)0x00000006U) /*!< 16 bytes data field */
 #define FDCAN_DATA_BYTES_20 ((uint32_t)0x00000007U) /*!< 20 bytes data field */
 #define FDCAN_DATA_BYTES_24 ((uint32_t)0x00000008U) /*!< 24 bytes data field */
 #define FDCAN_DATA_BYTES_32 ((uint32_t)0x0000000AU) /*!< 32 bytes data field */
 #define FDCAN_DATA_BYTES_48 ((uint32_t)0x0000000EU) /*!< 48 bytes data field */
 #define FDCAN_DATA_BYTES_64 ((uint32_t)0x00000012U) /*!< 64 bytes data field */
 ```

- `RxFifo1ElmtsNbr`：RX FIFO1 个数，范围 0 到 64。

- `RxFifo1ElmtSize`：RX FIFO1 每个元素中数据大小。

- `RxBuffersNbr`：设置 Rx Buffer 元素个数，范围 0 - 64：

- `RxBuffersSize`：设置 Rx Buffer 元素中每个数据大小，范围 0 - 64：

- `TxEventsNbr`：Tx Event FIFO 元素个数，范围 0 到 32。

- `TxBuffersNbr`：设置专用的 Tx Buffer 元素个数，范围 0 到 32。

- `TxFifoQueueElmtsNbr`：设置用于 Tx FIFO/Queue 的 Tx Buffers 个数。范围 0 到 32。

- `TxFifoQueueMode`：设置 FIFO 模式或者 QUEUE 队列模式。

```c
#define FDCAN_TX_FIFO_OPERATION ((uint32_t)0x00000000U)      /*!< FIFO mode */
#define FDCAN_TX_QUEUE_OPERATION ((uint32_t)FDCAN_TXBC_TFQM) /*!< Queue mode */
```

- `TxElmtSize`：设置 Tx Element 中的数据域大小。

#### FDCAN 总线过滤结构体 `FDCAN_FilterTypeDef`

```c
typedef struct
{
 	uint32_t IdType; 		    /*!< Specifies the identifier type. This parameter can be a value of @ref FDCAN_id_type */
 	uint32_t FilterIndex; 	 	/*!< Specifies the filter which will be initialized.This parameter must be a number between: - 0 and 127, if IdType is FDCAN_STANDARD_ID - 0 and 63, if IdType is FDCAN_EXTENDED_ID */
 	uint32_t FilterType; 		/*!< Specifies the filter type.This parameter can be a value of @ref FDCAN_filter_type.The value FDCAN_EXT_FILTER_RANGE_NO_EIDM is permitted only when IdType is FDCAN_EXTENDED_ID.This parameter is ignored if FilterConfig is set to FDCAN_FILTER_TO_RXBUFFER */
 	uint32_t FilterConfig; 		/*!< Specifies the filter configuration.This parameter can be a value of @ref FDCAN_filter_config */
 	uint32_t FilterID1; 		/*!< Specifies the filter identification 1.This parameter must be a number between: - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID */
 	uint32_t FilterID2; 		/*!< Specifies the filter identification 2.This parameter is ignored if FilterConfig is set to FDCAN_FILTER_TO_RXBUFFER.This parameter must be a number between: - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID */
 	uint32_t RxBufferIndex; 	/*!< Contains the index of the Rx buffer in which the matching message will be stored.This parameter must be a number between 0 and 63.This parameter is ignored if FilterConfig is different from FDCAN_FILTER_TO_RXBUFFER */
 	uint32_t IsCalibrationMsg; /*!< Specifies whether the filter is configured for calibration messages.This parameter is ignored if FilterConfig is different from FDCAN_FILTER_TO_RXBUFFER.This parameter can be: - 0 : ordinary message - 1 : calibration message */
} FDCAN_FilterTypeDef;
```

- `IdType`：用于设置标准 ID 和扩展 ID。

```c
#define FDCAN_STANDARD_ID ((uint32_t)0x00000000U) /*!< 标准 ID */
#define FDCAN_EXTENDED_ID ((uint32_t)0x40000000U) /*!< 扩展 ID */
```

- `FilterIndex`：用于过滤索引，如果是标准 ID，范围 0 到 127。如果是扩展 ID，范围 0 到 64。

- `FilterType`：用于设置过滤类型。如果成员 FilterConfig 设置为 FDCAN_FILTER_TO_RXBUFFER，本参数将不起

  作用。

  ```c
  #define FDCAN_FILTER_RANGE ((uint32_t)0x00000000U) /*!< 范围过滤从 FilterID1 到 FilterID2 */
  #define FDCAN_FILTER_DUAL ((uint32_t)0x00000001U)  /*!< 专用 ID 过滤，FilterID1 或者 FilterID2 */
  /*!< 精度屏蔽过滤，FilterID1 = filter, FilterID2 = mask */
  #define FDCAN_FILTER_MASK ((uint32_t)0x00000002U) 
  /*!< 仅 ID 扩展模式支持此参数，范围从 FilterID1 到 FilterID2, EIDM mask not applied */
  #define FDCAN_FILTER_RANGE_NO_EIDM ((uint32_t)0x00000003U)
  ```

- `FilterConfig`：用于设置过滤类型。

```c
#define FDCAN_FILTER_DISABLE ((uint32_t)0x00000000U) 	     //禁止过滤
#define FDCAN_FILTER_TO_RXFIFO0 ((uint32_t)0x00000001U)      //如果过滤匹配，将数据保存到 Rx FIFO 0
#define FDCAN_FILTER_TO_RXFIFO1 ((uint32_t)0x00000002U)      //如果过滤匹配，将数据保存到 Rx FIFO 1
#define FDCAN_FILTER_REJECT ((uint32_t)0x00000003U) 	     //如果过滤匹配，拒绝此 ID
#define FDCAN_FILTER_HP ((uint32_t)0x00000004U) 	    	//如果过滤匹配，设置高优先级
#define FDCAN_FILTER_TO_RXFIFO0_HP ((uint32_t)0x00000005U)   //如果过滤匹配，设置高优先级并保存到 FIFO 0
#define FDCAN_FILTER_TO_RXFIFO1_HP ((uint32_t)0x00000006U)   //如果过滤匹配，设置高优先级并保存到 FIFO 1
#define FDCAN_FILTER_TO_RXBUFFER ((uint32_t)0x00000007U)     //如果过滤匹配，保存到 Rx Buffer，并忽略 FilterType配置
```

- `FilterID1`：用于设置过滤 ID1。如果 ID 类型是 `FDCAN_STANDARD_ID`，范围 0 到 0x7FF。如果 ID 类型是`FDCAN_EXTENDED_ID`，范围是 0 到 0x1FFFFFFF。

- `FilterID2`：用于设置过滤 ID2。如果 FilterConfig 设置为 `FDCAN_FILTER_TO_RXBUFFER`，此参数不起作用。如果 ID 类型是 `FDCAN_STANDARD_ID`，范围 0 到 0x7FF。如果 ID 类型是 `FDCAN_EXTENDED_ID`，范围是 0 到 0x1FFFFFFF。

- `RxBufferIndex`：匹配消息存储到 Rx buffer 中的索引。参数范围 0 到 63。如果 FilterConfig 设置为`FDCAN_FILTER_TO_RXBUFFER`，此参数不起作用。

- `IsCalibrationMsg`：用于设置是否配置校准消息。如果 FilterConfig 设置为 `FDCAN_FILTER_TO_RXBUFFER`，此参数不起作用。0 ： 表示正常消息。1 ： 标志校准消息

#### FDCAN 总线消息发送结构体 `FDCAN_TxHeaderTypeDef`

```c
typedef struct
{
 	uint32_t Identifier; 			/*!< Specifies the identifier.This parameter must be a number between:- 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID */
 	uint32_t IdType; 				/*!< Specifies the identifier type for the message that will be transmitted.This parameter can be a value of @ref FDCAN_id_type */
 	uint32_t TxFrameType; 			/*!< Specifies the frame type of the message that will be transmitted.This parameter can be a value of @ref FDCAN_frame_type */
 	uint32_t DataLength; 			/*!< Specifies the length of the frame that will be transmitted.This parameter can be a value of @ref FDCAN_data_length_code */
 	uint32_t ErrorStateIndicator; 	 /*!< Specifies the error state indicator.This parameter can be a value of @ref FDCAN_error_state_indicator */
 	uint32_t BitRateSwitch; 		/*!< Specifies whether the Tx frame will be transmitted with or without bit rate switching.This parameter can be a value of @ref FDCAN_bit_rate_switching */
 	uint32_t FDFormat; 				/*!< Specifies whether the Tx frame will be transmitted in classic or FD format.This parameter can be a value of @ref FDCAN_format */
 	uint32_t TxEventFifoControl; 	 /*!< Specifies the event FIFO control.This parameter can be a value of @ref FDCAN_EFC */
 	uint32_t MessageMarker; 		/*!< Specifies the message marker to be copied into Tx Event FIFO element for identification of Tx message status.This parameter must be a number between 0 and 0xFF */
} FDCAN_TxHeaderTypeDef;
```

- `Identifier`：用于设置 ID，如果 `IdType` 是标准 `FDCAN_STANDARD_ID`，范围 0 到 0x7FF，如果 `IdType` 是`FDCAN_EXTENDED_ID` 扩展 ID，范围 0 到 0x1FFFFFFF。

- `IdType`：用于设置标准 ID 或者扩展 ID。

```c
#define FDCAN_STANDARD_ID ((uint32_t)0x00000000U) /*!< 标准 ID */
#define FDCAN_EXTENDED_ID ((uint32_t)0x40000000U) /*!< 扩展 ID */
```

- `TxFrameType`：用于设置帧类型，数据帧或遥控帧。

```c
#define FDCAN_DATA_FRAME ((uint32_t)0x00000000U)   /*!< 数据帧 */
#define FDCAN_REMOTE_FRAME ((uint32_t)0x20000000U) /*!< 遥控帧 */
```

- `DataLength`：用于设置数据长度。

```c
#define FDCAN_DLC_BYTES_0 ((uint32_t)0x00000000U) /*!< 0 bytes data field */
#define FDCAN_DLC_BYTES_1 ((uint32_t)0x00010000U) /*!< 1 bytes data field */
#define FDCAN_DLC_BYTES_2 ((uint32_t)0x00020000U) /*!< 2 bytes data field */
#define FDCAN_DLC_BYTES_3 ((uint32_t)0x00030000U) /*!< 3 bytes data field */
#define FDCAN_DLC_BYTES_4 ((uint32_t)0x00040000U) /*!< 4 bytes data field */
#define FDCAN_DLC_BYTES_5 ((uint32_t)0x00050000U) /*!< 5 bytes data field */
#define FDCAN_DLC_BYTES_6 ((uint32_t)0x00060000U) /*!< 6 bytes data field */
#define FDCAN_DLC_BYTES_7 ((uint32_t)0x00070000U) /*!< 7 bytes data field */
#define FDCAN_DLC_BYTES_8 ((uint32_t)0x00080000U) /*!< 8 bytes data field */
#define FDCAN_DLC_BYTES_12 ((uint32_t)0x00090000U) /*!< 12 bytes data field */
#define FDCAN_DLC_BYTES_16 ((uint32_t)0x000A0000U) /*!< 16 bytes data field */
#define FDCAN_DLC_BYTES_20 ((uint32_t)0x000B0000U) /*!< 20 bytes data field */
#define FDCAN_DLC_BYTES_24 ((uint32_t)0x000C0000U) /*!< 24 bytes data field */
#define FDCAN_DLC_BYTES_32 ((uint32_t)0x000D0000U) /*!< 32 bytes data field */
#define FDCAN_DLC_BYTES_48 ((uint32_t)0x000E0000U) /*!< 48 bytes data field */
#define FDCAN_DLC_BYTES_64 ((uint32_t)0x000F0000U) /*!< 64 bytes data field */
```

- `ErrorStateIndicator`：用于设置错误状态指示。

```c
#define FDCAN_ESI_ACTIVE ((uint32_t)0x00000000U)  /*!< 传输节点 error active */
#define FDCAN_ESI_PASSIVE ((uint32_t)0x80000000U) /*!< 传输节点 error passive */
```

- `BitRateSwitch`：用于设置发送是否波特率可变。

```c
#define FDCAN_BRS_OFF ((uint32_t)0x00000000U) /*!< FDCAN 帧发送/接收不带波特率可变 */
#define FDCAN_BRS_ON ((uint32_t)0x00100000U)  /*!< FDCAN 帧发送/接收带波特率可变 */
```

- `FDFormat`：用于设置发送帧是经典格式还是 CANFD 格式。

```c
#define FDCAN_CLASSIC_CAN ((uint32_t)0x00000000U) /*!< 帧发送/接收使用经典 CAN */
#define FDCAN_FD_CAN ((uint32_t)0x00200000U)      /*!< 帧发送/接收使用 FDCAN 格式 */
```

- `TxEventFifoControl`：用于设置发送事件 FIFO 控制。

```c
#define FDCAN_NO_TX_EVENTS ((uint32_t)0x00000000U)    /*!< 不存储 Tx events */
#define FDCAN_STORE_TX_EVENTS ((uint32_t)0x00800000U) /*!< 存储 Tx events */
```

- `MessageMarker`：用于设置复制到 TX EVENT FIFO 的消息 Marker，来识别消息状态，范围 0 到 0xFF。

#### FDCAN 总线消息接收结构体 `FDCAN_RxHeaderTypeDef`

```c
typedef struct
{
 	uint32_t Identifier; 		   /*!< Specifies the identifier.This parameter must be a number between: - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID */
    uint32_t IdType; 			   /*!< Specifies the identifier type of the received message.This parameter can be a value of @ref FDCAN_id_type */
 	uint32_t RxFrameType; 		   /*!< Specifies the the received message frame type.This parameter can be a value of @ref FDCAN_frame_type */
 	uint32_t DataLength; 		   /*!< Specifies the received frame length.This parameter can be a value of @ref FDCAN_data_length_code */
 	uint32_t ErrorStateIndicator; 	/*!< Specifies the error state indicator. This parameter can be a value of @ref FDCAN_error_state_indicator */
 	uint32_t BitRateSwitch; 	    /*!< Specifies whether the Rx frame is received with or without bit rate switching.This parameter can be a value of @ref FDCAN_bit_rate_switching */
 	uint32_t FDFormat;               /*!< Specifies whether the Rx frame is received in classic or FD format.This parameter can be a value of @ref FDCAN_format */
 	uint32_t RxTimestamp;            /*!< Specifies the timestamp counter value captured on start of frame reception.This parameter must be a number between 0 and 0xFFFF */
 	uint32_t FilterIndex; 			/*!< Specifies the index of matching Rx acceptance filter element.This parameter must be a number between: - 0 and 127, if IdType is FDCAN_STANDARD_ID - 0 and 63, if IdType is FDCAN_EXTENDED_ID */
 	uint32_t IsFilterMatchingFrame;  /*!< Specifies whether the accepted frame did not match any Rx filter.Acceptance of non-matching frames may be enabled via HAL_FDCAN_ConfigGlobalFilter().This parameter can be 0 or 1 */
} FDCAN_RxHeaderTypeDef;
```

- `Identifier`：用于设置 ID，如果 IdType 是标准`FDCAN_STANDARD_ID`，范围 0 到 0x7FF，如果 IdType 是`FDCAN_EXTENDED_ID`扩展 ID，范围 0 到 0x1FFFFFFF。

- `IdType`：用于设置标志 ID 或者扩展 ID

- `RxFrameType`：用于设置接收帧类型，数据帧或遥控帧

- `DataLength`：用于设置数据长度。

- `ErrorStateIndicator`：用于设置错误状态指示：

- `BitRateSwitch`：用于设置接收是否带波特率切换

- `FDFormat`：用于设置接收帧是经典格式还是 CANFD 格式

- `RxTimestamp`：用于设置帧接收时间戳，范围 0 到 0xFFFF。 

- `FilterIndex`：用于设置接收过滤索引。如果是标准 ID，范围 0 到 127，如果是扩展 ID，范围 0 到 63。 

- `IsFilterMatchingFrame`：用于设置是否接收非匹配帧，通过函数 `HAL_FDCAN_ConfigGlobalFilter()` 可以使能。0：表示不接受。1：表示接收。

### HAL 库函数

```c
/**
  * @brief	FDCAN 初始化函数
  */
HAL_StatusTypeDef HAL_FDCAN_Init(FDCAN_HandleTypeDef *hfdcan);

/**
  * @brief 	FDCAN 过滤器配置函数
  * @param	hfdcan			FDCAN句柄
  * @param	sFilterConfig	 FDCAN过滤器配置结构体
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigFilter(FDCAN_HandleTypeDef *hfdcan, FDCAN_FilterTypeDef *sFilterConfig);

/**
  * @brief	FDCAN 水印设置函数
  * @param	hfdcan			FDCAN句柄
  * @param	FIFO 			水印对应的FIFO，FDCAN_CFG_TX_EVENT_FIFO，FDCAN_CFG_RX_FIFO0，FDCAN_CFG_RX_FIFO1
  * @param	Watermark		水印中断产生时FIFO内的消息数
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigFifoWatermark(FDCAN_HandleTypeDef *hfdcan, uint32_t FIFO, uint32_t 
Watermark);
```

```c
/**
  * @brief	FDCAN 中断设置函数
  * @param	hfdcan			FDCAN句柄
  * @param	ActiveITs		中断类型
  * @param	Tx Buffer Indexes
  */
HAL_StatusTypeDef HAL_FDCAN_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t ActiveITs, uint32_t 
BufferIndexes);

// ActiveITs 可为以下宏定义之一
#define FDCAN_IT_TX_COMPLETE FDCAN_IE_TCE 		 			  /*!< Transmission Completed */
#define FDCAN_IT_TX_ABORT_COMPLETE FDCAN_IE_TCFE  			   /*!< Transmission Cancellation Finished */
#define FDCAN_IT_TX_FIFO_EMPTY FDCAN_IE_TFEE 	 			   /*!< Tx FIFO Empty */
#define FDCAN_IT_RX_HIGH_PRIORITY_MSG FDCAN_IE_HPME 	 	    /*!< High priority message received */
#define FDCAN_IT_RX_BUFFER_NEW_MESSAGE FDCAN_IE_DRXE 	 	    /*!< At least one received message stored into a Rx Buffer */
#define FDCAN_IT_TIMESTAMP_WRAPAROUND FDCAN_IE_TSWE 	 	    /*!< Timestamp counter wrapped around */
#define FDCAN_IT_TIMEOUT_OCCURRED FDCAN_IE_TOOE 		 	   /*!< Timeout reached */
#define FDCAN_IT_CALIB_STATE_CHANGED (FDCANCCU_IE_CSCE << 30) 	/*!< Clock calibration state changed */
#define FDCAN_IT_CALIB_WATCHDOG_EVENT (FDCANCCU_IE_CWEE << 30)  /*!< Clock calibration watchdog event occurred */
#define FDCAN_IT_TX_EVT_FIFO_ELT_LOST FDCAN_IE_TEFLE 			/*!< Tx Event FIFO element lost */
#define FDCAN_IT_TX_EVT_FIFO_FULL FDCAN_IE_TEFFE 			    /*!< Tx Event FIFO full */
#define FDCAN_IT_TX_EVT_FIFO_WATERMARK FDCAN_IE_TEFWE 			/*!< Tx Event FIFO fill level reached watermark */
#define FDCAN_IT_TX_EVT_FIFO_NEW_DATA FDCAN_IE_TEFNE 			/*!< Tx Handler wrote Tx Event FIFO element */
#define FDCAN_IT_RX_FIFO0_MESSAGE_LOST FDCAN_IE_RF0LE 			/*!< Rx FIFO 0 message lost */
#define FDCAN_IT_RX_FIFO0_FULL FDCAN_IE_RF0FE 				    /*!< Rx FIFO 0 full */
#define FDCAN_IT_RX_FIFO0_WATERMARK FDCAN_IE_RF0WE 				/*!< Rx FIFO 0 fill level reached watermark */
#define FDCAN_IT_RX_FIFO0_NEW_MESSAGE FDCAN_IE_RF0NE 			/*!< New message written to Rx FIFO 0 */
#define FDCAN_IT_RX_FIFO1_MESSAGE_LOST FDCAN_IE_RF1LE 			/*!< Rx FIFO 1 message lost */
#define FDCAN_IT_RX_FIFO1_FULL FDCAN_IE_RF1FE 				    /*!< Rx FIFO 1 full */
#define FDCAN_IT_RX_FIFO1_WATERMARK FDCAN_IE_RF1WE 				/*!< Rx FIFO 1 fill level reached watermark */
#define FDCAN_IT_RX_FIFO1_NEW_MESSAGE FDCAN_IE_RF1NE 			/*!< New message written to Rx FIFO 1 */
#define FDCAN_IT_RAM_ACCESS_FAILURE FDCAN_IE_MRAFE 				/*!< Message RAM access failure occurred */
#define FDCAN_IT_ERROR_LOGGING_OVERFLOW FDCAN_IE_ELOE 			/*!< Overflow of FDCAN Error Logging Counter occurred */
#define FDCAN_IT_RAM_WATCHDOG FDCAN_IE_WDIE 				    /*!< Message RAM Watchdog event due to missing READY */
#define FDCAN_IT_ARB_PROTOCOL_ERROR FDCAN_IE_PEAE 				/*!< Protocol error in arbitration phase detected */
#define FDCAN_IT_DATA_PROTOCOL_ERROR FDCAN_IE_PEDE 				/*!< Protocol error in data phase detected */
#define FDCAN_IT_RESERVED_ADDRESS_ACCESS FDCAN_IE_ARAE 			/*!< Access to reserved address occurred */
#define FDCAN_IT_ERROR_PASSIVE FDCAN_IE_EPE 				   /*!< Error_Passive status changed */
#define FDCAN_IT_ERROR_WARNING FDCAN_IE_EWE 					/*!< Error_Warning status changed */
#define FDCAN_IT_BUS_OFF FDCAN_IE_BOE 						   /*!< Bus_Off status changed */
```

```c
/**
  * @brief	FDCAN 启动函数
  * @param	hfdcan			FDCAN句柄
  */
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);

/**
  * @brief	添加消息到 Tx FIFO/Queue 并激活相应的传输请求。
  * @param	hfdcan			FDCAN句柄
  * @param	pTxHeader		发送消息结构体
  * @param	pTxData			发送数据地址
  */
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, 
uint8_t *pTxData);

/**
  * @brief	获取接收到的数据
  * @param	hfdcan			FDCAN句柄
  * @param	RxLocation		读取位置，选择FDCAN_RX_...(FIFO0\FIFO1\BUFFERx)
  * @param	pRxHeader		接收消息结构体
  * @param	pRxData			接收数据地址
  */
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation, 
FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData);
```

```c
/**
  * @brief	FIFO接收中断回调函数
  * @param	hfdcan			FDCAN句柄
  * @param	RxFifo0ITs		中断类型
  * @note	使用时，应判断中断类型，不判断中断句柄
  */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
```

### 配置流程

1. 定义过滤器，发送消息；
2. 配置过滤器，选择中断触发源，开启FDCAN；
3. 发送消息；
4. 在回调函数中接收消息。
