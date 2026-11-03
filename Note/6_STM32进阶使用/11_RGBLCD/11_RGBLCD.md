# STM32 进阶 11_RGBLCD

## 1. RGBLCD 简介

### 接口简介

RGBLCD 是 TFTLCD 的一种，但是引出的是 RGB 接口而非 MCU 接口。RGB 数据线为 24 根颜色数据线（RGB 各占 8 根即 RGB888，1600W 色）。

| **裸屏信号线** | **信号说明**          |
| -------------- | --------------------- |
| `R [0 : 7]`    | 红色数据线，一般为8位 |
| `G [0 : 7]`    | 绿色数据线，一般为8位 |
| `B [0 : 7]`    | 蓝色数据线，一般为8位 |
| `DE`           | 数据使能线            |
| `VSYNC`        | 垂直同步信号线        |
| `HSYNC`        | 水平同步信号线        |
| `DCLK`         | 像素时钟信号线        |

### 驱动模式（刷新方式）

![NULL](./assets/picture_1.jpg)

1. **DE 同步模式**：使用 DE 信号来确定有效数据；
2. **HV 同步模式**：需要行和场同步信号来表示扫描的行和列。
> 1. 水平扫描时序
> - $t_{hpw}$：水平同步有效信号脉宽，用于表示一行数据的开始；
> - $t_{hb}$：水平后廊，表示从水平有效信号开始，到有效数据输出之间的像素时钟个数；
> - $t_{hfp}$：水平前廊，表示一行数据结束后，到下一个水平同步信号开始之前的像素时钟个数。
>
> 2. 垂直扫描时序
>
> ![NULL](./assets/picture_2.jpg)
>
> - $t_{vpw}$：垂直同步有效信号脉宽，用于表示一帧数据的开始；
> - $t_{vb}$：垂直后廊，表示垂直同步信号以后的无效行数；
> - $t_{vfp}$：垂直前廊，表示一帧数据输出结束后，到下一个垂直同步信号开始之前的无效行数。
>
> 3. RGBLCD 时序参数
>
> ![NULL](./assets/picture_3.jpg)

## 2. STM32 LTDC 简介

### LTDC 框图

LTDC，为 LCD-TFT 显示控制器。通过 LTDC 外设，可使芯片直接外接 RGBLCD 屏，实现液晶屏驱动。

> 1. 与其他接口相比，LTDC 可连接低成本显示模块；
> 2. 可以驱动 16 位、18 位和 24 位显示模块；
> 3. 分辨率范围(宽度 4096,高度 2048)，像素时钟高达 83MHz；
> 4. 支持 RGB888、RGB565、ARGB8888 和 ARGB1555（A 表示透明像素）；
> 5. LTDC 硬件完全能够管理数据读取、RGB 输出和信号控制；
> 6. 2 层数据源:前景层和背景层，LTDC 混合后输出到 LCD 有一层数据。

![NULL](./assets/picture_4.jpg)

1. 像素处理单元（像素时钟域内）

LTDC 有两层，每层都可以分别启用、禁用和配置。图层显示的顺序是固定，由下至上。

![NULL](./assets/picture_5.jpg)

> ARGB8888 格式：带 8 位透明通道，最高 8 位为透明通道参数，表示透明度，值越大越不透明 ，值越小越透明。剩余 24 位为 RGB 参数。

![NULL](./assets/picture_6.jpg)

2. 配置和状态寄存器

控制整个 LTDC 工作参数：各信号的有效电平、垂直/水平同步时间参数、像素格式、数据使能。

![NULL](./assets/picture_7.jpg)

3. AHB 接口

![NULL](./assets/picture_8.jpg)

> 由于图像数据所需的数据量很大，通常使用外部 SDRAM 作为显存，通过 DMA2D 加速将显存数据传入 LTDC FIFO。

### LTDC 寄存器

#### LTDC 全局控制寄存器

![NULL](./assets/picture_9.jpg)

> - `HSPOL`：水平同步极性，控制 HSYNC 信号极性，根据 LCD 面板特性设置。
> - `VSPOL`：垂直同步极性，控制 VSYNC 信号极性，根据 LCD 面板特性设置。
> - `DEPOL`：数据使能极性，控制 DE 信号极性，根据 LCD 面板特性设置。
> - `PCPOL`：像素时钟极性，控制像素时钟极性，根据 LCD 面板特性设置。
> - `LTDCEN`：TFTLCD 控制器使能位，即 LTDC 开关。

**显示寄存器组**

![NULL](./assets/picture_10.jpg)

#### LTDC 同步大小配置寄存器 `LTDC_SSCR`

![NULL](./assets/picture_11.jpg)

> - `HSW`：表示水平同步宽度（以像素时钟为单位），表示水平同步脉冲减 1，即 HSW-1。
> - `VSH`：表示垂直同步高度（以水平扫描行为单位），表示垂直同步宽度减 1，即 VSW-1。

#### LTDC 后沿配置寄存器 `LTDC_BPCR`

![NULL](./assets/picture_12.jpg)

> - `AHBP`：累加水平后沿（以像素时钟为单位），表示 HSW+HBP-1；
> - `AVBP`：累加垂直后沿（以水平扫描行为单位），表示 VSW+VBP-1；

#### LTDC 有效宽度配置寄存器 `LTDC_AWCR`

![NULL](./assets/picture_13.jpg)

> - `AAW`：累加有效宽度（以像素时钟为单位），表示 HSW+HBP+有效宽度-1
> - `AAH`：累加有效高度（以水平扫描行为单位），表示 VSW+VBP+有效高度-1

#### LTDC 总宽度配置寄存器 `LTDC_TWCR`

![NULL](./assets/picture_14.jpg)

> - `TOTALW`：总宽度（以像素时钟为单位），表示 HSW+HBP+有效宽度+HFP-1
> - `TOTALH`：总高度（以水平扫描行为单位），表示 VSW+VBP+有效高度+VFP-1

**图层显示寄存器组**

#### LTDC 背景色配置寄存器 `LTDC_BCCR`

![NULL](./assets/picture_15.jpg)

> 定义背景层颜色 RGB888，通过低 24 位配置，一般全设置为 0 即可。可以使用该图层验证 LTDC 配置是否正确。

#### LTDC 层像素格式配置寄存器 `LTDC_LxPFCR`

![NULL](./assets/picture_16.jpg)

> - 000：ARGB888		
> - 001：RGB888			
> - 010：RGB565
> - 011：ARGB1555		
> - 100：ARGB444		
> - 101：L8(8 位 Luminance)
> - 110：AL44(4 位 Alpha，4 位 Luminance)			
> - 111：AL88(8 位 Alpha，8 位 Luminance)

#### LTDC 层窗口水平位置配置寄存器 `LTDC_LxWHPCR`

![NULL](./assets/picture_17.jpg)

> - `WHSTPOS`：窗口水平起始位置，定义层窗口的一行的第一个可见像素；
> - `WHSPPOS`：窗口水平停止位置，定义层窗口的一行的最后一个可见像素；

#### LTDC 层窗口垂直位置配置寄存器 `LTDC_LxWVPCR`

![NULL](./assets/picture_18.jpg)

> - `WVSTPOS`：窗口垂直起始位置，定义层窗口的第一个可见行；
> - `WVSPPOS`：窗口垂直停止位置，定义层窗口的最后一个可见行；

#### LTDC 层颜色帧缓冲区地址寄存器 `LTDC_LxCFBAR`

![NULL](./assets/picture_19.jpg)

> 定义一层显存的起始地址。LTDC 支持 2 个层，所以总共有两个寄存器，分别设置层 1 和层 2 的显存起始地址。

#### LTDC 层颜色帧缓冲区长度寄存器 `LTDC_LxCFBLR`

![NULL](./assets/picture_20.jpg)

> - `CFBP`：定义从像素某行的起始处到下一行的起始处的增量（以字节为单位）
> - `CFBLL`：定义一行像素的长度（以字节为单位）+ 3

#### LTDC 层颜色帧缓冲区行数寄存器 `LTDC_LxCFBLNR`

![null](./assets/picture_21.jpg)

> - `CFBLNBR`：用于定义帧缓冲区行数

#### LTDC 层恒定 Alpha 配置寄存器 `LTDC_LxCACR`

![NULL](./assets/picture_22.jpg)

> - `CONSTA`：配置混合时使用的恒定 Alpha。恒定 Alpha 由硬件实现 255 分频。

#### LTDC 层混合系数配置寄存器 `LTDC_LxBFCR`

![NULL](./assets/picture_23.jpg)

> $$
> BC = BF_1C+BF_2C_S
> $$
>
> $BC$：混合后的颜色；$BF_1$，$BF_2$：混合系数。$C$：当前层的颜色；$C_S$：底层混合后的颜色。
>
> 对于层 1 来说，$C_S$为背景层颜色，对于层 2 来说，$C_S$为背景层和层 1 混合后的颜色。

### LTDC 配置

**以 800*480 4.3 寸 RGBLCD ，以 RGB565 显示格式为例**

![NULL](./assets/picture_24.jpg)

![NULL](./assets/picture_25.jpg)

![NULL](./assets/picture_26.jpg)

## 3. STM32 DMA2D 简介

### DMA2D 框图

DMA2D 是由 ST 公司设计的一个专用于图像处理的专业 DMA(Chrom-Art Accelerator)。 存在 DMA 数据传输功能、2D 图形加速功能。

- 工作模式

> 1. 寄存器到存储器：快速单色填充（清屏），无 FG 和 BG，仅输出阶段激活。
>
> 2. 存储器到存储器：快速图像填充，仅激活 FG FIFO，作缓冲区。
>
> 3. 存储器到存储器并执行像素颜色格式转换，不会激活 BG FIFO。
>
> 4. 存储器到存储器且支持像素颜色格式转换和透明度混合，执行 PFC 和混合时的 FG 和 BG 获取。

![NULL](./assets/picture_27.jpg)

![NULL](./assets/picture_28.jpg)

通常使用寄存器操作 DMA2D 实现最大程度加速。

| **寄存器**      | **名称**               | **作用**                     |
| --------------- | ---------------------- | ---------------------------- |
| `DMA2D_CR`      | 控制寄存器             | 用于设置工作模式及启动DMA2D  |
| `DMA2D_OMAR`    | 输出存储器地址寄存器   | 用于设置输出存储器地址       |
| `DMA2D_OOR`     | 输出偏移寄存器         | 用于设置输出行偏移           |
| `DMA2D_NLR`     | 行数寄存器             | 用于设置行数和每行像素数     |
| `DMA2D_OCOLR`   | 输出颜色寄存器         | 用于设置指定的颜色填充       |
| `DMA2D_OPFCCR`  | 输出FPC控制寄存器      | 用于设置输出的颜色格式       |
| `DMA2D_FGPFCCR` | 前景层PFC控制寄存器    | 用于设置前景层输出的颜色格式 |
| `DMA2D_FGMAR`   | 前景层存储器地址寄存器 | 用于设置前景层的存储器地址   |
| `DMA2D_FGOR`    | 前景层偏移寄存器       | 用于控制前景层的偏移         |
| `DMA2D_ISR`     | 中断状态寄存器         | 表示DMA2D各种状态标识        |
| `DMA2D_IFCR`    | 中断标志清零寄存器     | 用于清除ISR寄存器对应的标志  |

**`DMA2D_CR`，`DMA2D_OMAR`，`DMA2D_OOR` 用于目标区域设置；`DMA2D_NLR` 在 R2M 中用到；`DMA2D_OCOLR`，`DMA2D_OPFCCR` 用于像素格式设置；`DMA2D_FGPFCCR`，`DMA2D_FGMAR`，`DMA2D_FGOR` 用于源区域设置。**

### DMA2D 寄存器

#### DMA2D 控制寄存器 `DMA2D_CR`

![NULL](./assets/picture_29.jpg)

> `MODE`： 表示 DMA2D 的工作模式（四种）	        
>
> >  00：存储器到存储器				
> >  01：存储器到存储器并执行 PFC	        
> >  10：存储器到存储器并执行混合    	
> >  11：寄存器到存储器
> 
> `START`： 控制 DMA2D 的启动，配置完成后，设置为 1，启动 DMA2D 传输

#### DMA2D 输出存储器地址寄存器 `DMA2D_OMAR`

![image-20240429201724613](./assets/picture_30.jpg)

> 用于设置输出存储器地址，也就是输出 FIFO 所存储的数据地址，该地址需要根据开窗的起始坐标来进行设置。
>
> 前景层存储器地址寄存器 `DMA2D_FGMAR` 与 `DMA2D_OMAR` 一样，不过设置的是前景层的存储器地址。

#### DMA2D 输出偏移寄存器 `DMA2D_OOR`

![NULL](./assets/picture_31.jpg)

> 用于设置输出行偏移，作用于显示层，以像素为单位。行偏移将添加到各行末尾，用于确定下一行的起始地址。
>
> 前景层偏移寄存器 `DMA2D_FGOR` 与 `DMA2D_OOR` 一样，不过设置的是前景层的行偏移。

#### DMA2D 行数寄存器 `DMA2D_NLR`

![NULL](./assets/picture_32.jpg)

> 该寄存器用于控制每行的像素和行数，该寄存器的设置对前景层和显示层均有效，通过该寄存器的配置，就可以设置开窗的大小。
>
> `NL`：设置待传输区域的行数，用于确定窗口的高度。
>
> `PL`：设置待传输区域的每行像素数，用于确定窗口的宽度。

#### DMA2D 输出颜色寄存器 `DMA2D_OCOLR`

![NULL](./assets/picture_33.jpg)

> 用于配置在寄存器到存储器模式下，填充时所用的颜色值。

#### DMA2D 输出 PFC 控制寄存器 `DMA2D_OPFCCR`

![NULL](./assets/picture_34.jpg)

> 该寄存器用于设置寄存器到存储器工作模式下的颜色格式。
>
> CM[2:0]表示颜色格式：
>
> > 000：ARGB8888； 
> >
> > 001：RGB888； 
> >
> > 010：RGB565； 
> >
> > 011：ARGB1555； 
> >
> > 100：ARGB1444；

#### DMA2D 前景层 PFC 控制寄存器 `DMA2D_FGPFCCR`

![NULL](./assets/picture_35.jpg)

> 该寄存器用于设置存储器到存储器工作模式下的颜色格式。
>
> CM[3:0]表示颜色格式：
>
> > 0000：ARGB8888；
> >
> > 0001：RGB888； 
> >
> > 0010：RGB565； 
> >
> > 0011：ARGB1555；
> >
> > 0100：ARGB4444；
> >
> > 0101：L8； 
> >
> > 0110：AL44； 
> >
> > 0111：AL88； 
> >
> > 1000：L4； 
> >
> > 1001：A8； 
> >
> > 1010：A4；

#### DMA2D 中断状态寄存器 `DMA2D_ISR`

![NULL](./assets/picture_36.jpg)

> - `TCIF` 位表示 DMA2D 的传输完成中断标志。当 DMA2D 传输操作完成时，该位置 1，表示可以进行下一次 DMA2D 传输。
> - DMA2D 中断标志清零寄存器 `DMA2D_IFCR`，用于清除 DMA2D_ISR 寄存器对应位的标志。若 DMA2D 传输完成，向 `CTCIF` 位写 1 即可清除 DMA2D_ISR 寄存器的 `TCIF` 位标志。

### DMA2D 的使用

[DMA2D 使用示例][https://blog.csdn.net/rtthreadiotos/article/details/118347934]

1. 颜色填充

> - DMA2D 设置为 R2M 模式
> - 使用 `OMAR` 确定起始区域，使用 `NLR` 确定填充宽度和填充高度，使用 `OOR` 确定填充一行后需要跳过多少像素。
> - 填充内容使用 `OCOLR` 和 `OPFCCR` 设置。
> - 启用 DMA2D ,检测 `ISR` 和使用 `IFCR` 清理标志位。

```c
static inline void DMA2D_Fill( void * pDst, uint32_t width, uint32_t height, uint32_t lineOff, uint32_t pixelFormat,  uint32_t color) 
{
    /* DMA2D配置 */  
    DMA2D->CR      = 0x00030000UL;                                  // 配置为寄存器到储存器模式
    DMA2D->OCOLR   = color;                                         // 设置填充使用的颜色，格式应该与设置的颜色格式相同
    DMA2D->OMAR    = (uint32_t)pDst;                                // 填充区域的起始内存地址
    DMA2D->OOR     = lineOff;                                       // 行偏移，即跳过的像素，注意是以像素为单位
    DMA2D->OPFCCR  = pixelFormat;                                   // 设置颜色格式
    DMA2D->NLR     = (uint32_t)(width << 16) | (uint16_t)height;    // 设置填充区域的宽和高，单位是像素

    /* 启动传输 */
    DMA2D->CR   |= DMA2D_CR_START;   

    /* 等待DMA2D传输完成 */
    while (DMA2D->CR & DMA2D_CR_START) {} 
}
```

2. 图像传输

> - DMA2D 设置为 M2M 模式。
> - 使用 `OMAR`，`FGMAR` 确定起始区域，使用 `NLR` 确定填充宽度和填充高度，使用 `OOR`，`FGOR` 确定填充一行后需要跳过多少像素。
> - 填充内容使用 `OCOLR` 和 `OPFCCR` 设置。
> - 启用 DMA2D ,检测 `ISR` 和使用 `IFCR` 清理标志位。

```c
static void DMA2D_MemCopy(uint32_t pixelFormat, void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst)
{
    /* DMA2D配置 */
    DMA2D->CR      = 0x00000000UL;
    DMA2D->FGMAR   = (uint32_t)pSrc;
    DMA2D->OMAR    = (uint32_t)pDst;
    DMA2D->FGOR    = OffLineSrc;
    DMA2D->OOR     = OffLineDst;
    DMA2D->FGPFCCR = pixelFormat;
    DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint16_t)ySize;

    /* 启动传输 */
    DMA2D->CR   |= DMA2D_CR_START;

    /* 等待DMA2D传输完成 */
    while (DMA2D->CR & DMA2D_CR_START) {}
}
```

