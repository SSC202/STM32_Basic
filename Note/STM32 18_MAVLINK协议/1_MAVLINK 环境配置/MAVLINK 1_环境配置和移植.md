# MAVLINK 1_环境配置和移植

## 1. Windows系统

准备工作：
>1. 安装Python https://www.python.org/downloads/windows/
>2. 下载`Future`模块 : `pip3 install future`

注意：Python应尽量安装到系统盘之外，因为系统盘内为只读状态。

下载MAVLINK编辑器：
```shell
git clone https://github.com/mavlink/mavlink
```
（安装在与Python相同的目录下即可）

在文件夹中打开终端
```shell
cd mavlink
```

更新子模块
```shell
git submodule update --init --recursive
```

执行mavgenerate.py，这是用Python语言编写的MAVLink的图形用户界面代码生成器。
```shell
python mavgenerate.py
```

至此配置完成。