# STM32 A4_PySerial 串口通信

## 1. 上位机的简单串口通信`PySerial`

### `pyserial`包的下载和使用

下载`pyserial`包（注意不要下载`serial`包）

```shell
pip install pyserial
```

在Python代码中使用`pyserial`包

```python
import serial
```

### 串口号的确定

```python
import serial
import serial.tools.list_ports
 
# 获取所有串口设备
ports_list = list(serial.tools.list_ports.comports())
if len(ports_list) <= 0:
    print("No used serial")
else:
    print("serial port list:")
    for comport in ports_list:
        print(list(comport)[0], list(comport)[1])
```

### 打开串口

```python
import serial
 
ser = serial.Serial("/dev/ttyUSB0", 115200)    		# 打开ttyUSB0，将波特率配置为115200，其余参数使用默认值
if ser.is_open:                        			    # 判断串口是否成功打开
    print("Sussess to open the serial.")
    print(ser.name)    
else:
    print("Fail to open the serial.")
```

`Serial`构造函数传递的参数如下：

> - `port` - 串口设备名。
> - `baudrate` - 波特率，可以是50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600, 1000000, 1152000, 1500000, 2000000, 2500000, 3000000, 3500000, 4000000。
> - `bytesize` - 数据位，可取值为：`FIVEBITS`, `SIXBITS`, `SEVENBITS`, `EIGHTBITS`。
> - `parity` - 校验位，可取值为：`PARITY_NONE`, `PARITY_EVEN`, `PARITY_ODD`, `PARITY_MARK`, `PARITY_SPACE`。
> `stopbits` - 停止位，可取值为：`STOPBITS_ONE`, `STOPBITS_ONE_POINT_FIVE`, `STOPBITS_TOW`。
> - `xonxoff` - 软件流控，可取值为 `True`, `False`。
> - `rts`/`cts` - 硬件（`RTS`/`CTS`）流控，可取值为 `True`, `False`。
> - `dsr`/`dtr` - 硬件（`DSR`/`DTR`）流控，可取值为 `True`, `False`。
> - `timeout` - 读超时时间，可取值为 None, 0 或者其他具体数值（支持小数）。当设置为 None 时，表示阻塞式读取，一直读到期望的所有数据才返回；当设置为 0 时，表示非阻塞式读取，无论读取到多少数据都立即返回；当设置为其他数值时，表示设置具体的超时时间（以秒为单位），如果在该时间内没有读取到所有数据，则直接返回。
> - `write_timeout`: 写超时时间，可取值为 None, 0 或者其他具体数值（支持小数）。

关闭串口使用`ser.close`即可。

### 数据发送

1. 对于字符串的发送，通常使用`字符串.encode('utf-8')`的方式进行发送；
2. 对于数据帧的发送，则需要导入`struct`包。

> `struct`包的使用：
>
> > 1. 按照指定格式将Python数据转换为字符串,该字符串为字节流,如网络传输时,不能传输int,此时先将int转化为字节流,然后再发送;
> > 2. 按照指定格式将字节流转换为Python指定的数据类型;
> > 3. 处理二进制数据,如果用`struct`来处理文件的话,需要用’`wb`’,`’rb`’以二进制(字节流)写,读的方式来处理文件;
> > 4. 处理c语言中的结构体;
>
> `struct`包中的函数：
>
> | 函数                                  | return        | explain                                                      |
> | ------------------------------------- | ------------- | ------------------------------------------------------------ |
> | `pack(fmt,v1,v2…)`                    | `string`      | 按照给定的格式(`fmt`),把数据转换成字符串(字节流),并将该字符串返回. |
> | `pack_into(fmt,buffer,offset,v1,v2…)` | `None`        | 按照给定的格式(`fmt`),将数据转换成字符串(字节流),并将字节流写入以`offset`开始的`buffer`中.(`buffer`为可写的缓冲区,可用`array`模块) |
> | `unpack(fmt,v1,v2…..)`                | `tuple`       | 按照给定的格式(`fmt`)解析字节流,并返回解析结果               |
> | `pack_from(fmt,buffer,offset)`        | `tuple`       | 按照给定的格式(`fmt`)解析以`offset`开始的缓冲区,并返回解析结果 |
> | `calcsize(fmt)`                       | `size of fmt` | 计算给定的格式(`fmt`)占用多少字节的内存，注意对齐方式        |
>
>
> 当打包或者解包的时,需要按照特定的方式来打包或者解包。该方式就是格式化字符串,它指定了数据类型,除此之外,还有用于控制字节顺序、大小和对齐方式的特殊字符。	
>
>
> 为了同c中的结构体交换数据，还要考虑c或c++编译器使用了字节对齐，通常是以4个字节为单位的32位系统，故而`struct`根据本地机器字节顺序转换.可以用格式中的第一个字符来改变对齐方式。
>
> | Character | Byte order    | Size | Alignment       |
> | :-------- | :------------ | :--- | :-------------- |
> | @(默认)   | 本机          | 本机 | 本机,凑够4字节  |
> | =         | 本机          | 标准 | none,按原字节数 |
> | <         | 小端          | 标准 | none,按原字节数 |
> | >         | 大端          | 标准 | none,按原字节数 |
> | !         | network(大端) | 标准 | none,按原字节数 |
> 
> 格式符定义如下：
> |格式符	|C语言类型	|Python类型	|Standard size|
> |----|----|----|----|
>|`x`	|`pad byte`(填充字节)	|no value	 ||
>|`c`	|`char`	|string of length 1	|1|
>|`b`	|`signed char`	|`integer`	|1|
>|`B`	|`unsigned char`	|`integer`	|1|
>|`?`	|`_Bool`	|`bool`	|1|
>|`h`	|`short`	|`integer`	|2|
>|`H`	|`unsigned short`	|`integer`	|2|
>|`i`	|`int`	|`integer`	|4|
>|`I`	|`unsigned int`	|`integer`	|4|
>|`l`	|`long`	|`integer`	|4|
>|`L`	|`unsigned long`	|`long`	|4|
>|`q`	|`long long`	|`long`	|8|
>|`Q`	|`unsigned long long`	|`long`	|8|
>|`f`	|`float`	|`float`	|4|
>|`d`	|`double`	|`float`	|8|
>|`s`	|`char[]`	|`string`	 ||
>|`p`	|`char[]`	|`string`	||
>|`P`	|`void *`	|`long`	 ||

使用`ser.write()`进行发送。

### 数据接收

使用`ser.read()`函数，传入需要读取的字节数。此函数具有阻塞性质，如果未设置超时时间，则会一直等待接收。

可以通过以下的方式实现一直接收。

```python
import serial
import time

if __name__ == '__main__':
    com = serial.Serial('/dev/ttyUSB0', 115200)
    over_time = 30
    start_time = time.time()
    while True:
        end_time = time.time()
        if end_time - start_time < over_time:
            data = com.read(com.inWaiting())
            data = str(data)
            if data != '':
                print data
```

