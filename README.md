# STM32_Basic
## 本库的使用方法

本库是基于 STM32F103RCT6 的学习笔记，适用于 STM32 的初步学习。目前主线部分已经完结。

### 相关笔记

STM32 FreeRTOS（更新完成）：[Click Here](https://github.com/SSC202/FreeRTOS)

STM32 电机驱动（更新中）：[Click Here](https://github.com/SSC202/STM32_Engine)

STM32 传感器驱动（更新中）：[Click Here](https://github.com/SSC202/STM32_Hardware)

STM32 LVGL 图形库（更新完成）：[Click Here](https://github.com/SSC202/STM32_LVGL)

STM32 H7 系列（更新中）：[Click Here](https://github.com/SSC202/STM32H7_Basic)

OpenCV 学习（更新中）：[Click Here](https://github.com/SSC202/OpenCV)

ROS2 学习（更新中）：[Click Here](https://github.com/SSC202/ROS2)

### 使用此库的前置条件

1. 学会使用Github，CSDN，知乎，B站等常用论坛，具备良好的检索能力；
2. 学会独立解决硬件开发问题，具有一定的调试能力；
3. 学会安装对应的环境；
4. 学会花钱；
5. 保持一颗探索的精神。

### 对应Note和相关Code说明

| Note                                                         | Note说明                     | Code                                                         |
| ------------------------------------------------------------ | ---------------------------- | ------------------------------------------------------------ |
| [1_环境配置](https://github.com/SSC202/STM32_Basic/tree/main/Note/1_环境配置) | STM32基本开发环境配置        | 1. [EIDE链测试](https://github.com/SSC202/STM32_Basic/tree/main/Code/Clion_Project/1_Test) 2. [Clion 测试](https://github.com/SSC202/STM32_Basic/tree/main/Code/Clion_Project/1_Test) |
| [2_STM32最小系统电路](https://github.com/SSC202/STM32_Basic/tree/main/Note/2_STM32%20最小系统电路) | STM32系列的最小系统电路简介  | 无                                                           |
| [3_STM32 总体架构和寄存器](https://github.com/SSC202/STM32_Basic/tree/main/Note/3_STM32%20总体架构和寄存器) | STM32总体架构和寄存器介绍    | 无                                                           |
| [4_GPIO](https://github.com/SSC202/STM32_Basic/tree/main/Note/4_GPIO) | STM32 GPIO-通用输入输出 介绍 | 1. [LED点亮实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/1_GPIO/1_LED_Test) 2. [单按键读取实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/1_GPIO/2_KEY_Test) |
| [5_EXTI](https://github.com/SSC202/STM32_Basic/tree/main/Note/5_EXTI) | STM32 外部中断 介绍          | [外部中断按键实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/2_EXTI/1_Button_Light) |
| [6_TIM](https://github.com/SSC202/STM32_Basic/tree/main/Note/6_TIM) | STM32 定时器介绍             | 1. [定时器微秒延时实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/3_TIM/1_TIM_Delay) 2. [呼吸灯实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/3_TIM/2_PWM_LED) 3. [输入捕获读取按键时间实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/3_TIM/3_Input_Caputure_Key) |
| [7_USART](https://github.com/SSC202/STM32_Basic/tree/main/Note/7_USART) | STM32 串口介绍               | 1. [串口轮询实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/4_USART/1_USART_Polling_Transmit_Receive) 2. [串口中断收发实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/4_USART/2_USART_IT_Transmit_Receive) 3. [串口数据包发送实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/4_USART/3_HEX_Frame) 4. [串口DMA发送实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/4_USART/4_USART_DMA_Transmit_Receive) |
| [8_SPI](https://github.com/SSC202/STM32_Basic/tree/main/Note/8_SPI) | STM32 SPI 通信介绍           | 无（见传感器驱动）                                           |
| [9_IIC](https://github.com/SSC202/STM32_Basic/tree/main/Note/9_IIC) | STM32 IIC 通信介绍           | 无（见传感器驱动）                                           |
| [10_ADC](https://github.com/SSC202/STM32_Basic/tree/main/Note/10_ADC) | STM32 ADC 模数转换器介绍     | 1. [ADC单通道转换实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/5_ADC/1_ADC_Single_Channel) 2. [ADC多通道转换实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/5_ADC/2_ADC_DMA) |
| [11_CAN](https://github.com/SSC202/STM32_Basic/tree/main/Note/11_CAN) | STM32 CAN 总线通信介绍       | [CAN回环发送实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/6_CAN/1_CAN_Transmit_Receive) |
| [12_CRC](https://github.com/SSC202/STM32_Basic/tree/main/Note/12_CRC) | STM32 CRC 校验介绍           | [CRC校验例程](https://github.com/SSC202/STM32_Basic/tree/main/Code/7_CRC/CRC_Test) |
| [13_IWDG WWDG](https://github.com/SSC202/STM32_Basic/tree/main/Note/13_IWDG%20WWDG) | STM32 看门狗介绍             | 1. [独立看门狗实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/8_IWDG%20WWDG/IWDG_Test) 2. [窗口看门狗实验](https://github.com/SSC202/STM32_Basic/tree/main/Code/8_IWDG%20WWDG/WWDG_Test) |

### 补充内容

1. C语言补充：[Click Here](https://github.com/SSC202/STM32_Basic/blob/main/Note/A1_C语言扩展/A1_C语言扩展.md)

2. 环形缓冲区：[Click Here](https://github.com/SSC202/STM32_Basic/blob/main/Note/A3_环形缓冲区/环形缓冲区.md)

   
