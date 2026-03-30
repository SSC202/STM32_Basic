# CANopen 基本原理

CAN 总线仅仅定义了物理层和数据链路层，这两层由硬件实现。但是 CAN 没有规定应用层。

![NULL](./assets/picture_1.jpg)

CANopen 协议定义在应用层上，支持各种 CAN 厂商设备的互用性、互换性，能够实现在 CAN 网络中提供标准的、统一的系统通讯模式，提供设备功能描述方式，执行网络管理功能。包括：

- 应用层 (Application layer)：为网络中每一个有效设备都能够提供一组有用的服务与协议。
- 通讯描述 (Communication profile)：提供配置设备、通讯数据的含义，定义数据通讯方式。
- 设备描述 (Device profile)：为设备（类）增加符合规范的行为。

## 1. CANopen 简介

