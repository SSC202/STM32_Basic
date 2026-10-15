# STM32 LwIP RAW接口

RAW API是基于回调函数实现的API接口，它是很底层的API接口，这需要开发者对 LwIP 有较深的了解才能很好使用它，RAW API的核心就是对控制块的处理，因为对于报文数据的处理、注册回调函数等都是需要开发者自己去实现，都是比较麻烦的，但是有一个优点，那就是处理数据效率高。

## 1. UDP 接口

|RAW 的 API 函数| 描述|
|-|-|
|`udp_new` |新建一个 UDP 的 PCB 块|
|`udp_remove`| 将一个 PCB 控制块从链表中删除，并且释放这个控制块的内存|
|`udp_bind` |为 UDP 的 PCB 控制块绑定一个本地 IP 地址和端口号|
|`udp_connect` |连接到指定 IP 地址主机的指定端口上|
|`udp_disconnect` |断开连接，将控制块设置为非连接状态|
|`udp_send` |通过一个 PCB 控制块发送数据|
|`udp_recv` |创建的一个回调函数|

- UDP 配置步骤

  > 1) 创建 UDP 控制块：调用函数 `udp_new` 创建 UDP 控制块。
  > 2) 连接指定的 IP 地址和端口号：调用函数 `udp_connect` 绑定远程 IP 地址和远程端口号。
  > 3) 绑定本地 IP 地址与端口号：调用函数 `udp_bind` 绑定本地 IP 地址和本地端口号。
  > 4) 注册接收回调函数：`udp_recv` 是注册接收回调函数，该函数需要自己编写。
  > 5) 发送数据：调用函数 `udp_send` 发送数据。

- UDP 接口

  - `udp_new()`

    在使用 UDP 协议进行通信之前，必须创建一个 UDP 控制块，然后将控制块与对应的端口号进行绑定，才能发送报文，而在接收 UDP 报文的时候，这个端口号就是 UDP 报文唯一识别的标志，否则 UDP 报文将无法递交到应用层去处理，即无法通过 UDP 控制块的接收回调函数递交给应用层。

    ```c
    struct udp_pcb *udp_new(void);
    ```

  - `udp_bind()`

    绑定控制块的作用其实就是将本机 IP 地址与端口号填写在 UDP 控制块中，以便表示唯一的应用，并且能正常与远端主机进行 UDP 通信，在这个函数中，它会将 UDP 控制块的`local_ip` 与 `local_port` 字段进行初始化，并且把UDP控制块添加到 `udp_pcbs` 链表中。

    ```c
    err_t udp_bind(struct udp_pcb *pcb,const ip_addr_t *ipaddr,u16_t port);
    ```

  - `udp_connect()`

    设置控制块中的远端 IP 地址与端口号，然后将 UDP 控制块的状态设置为会话状态 `UDP_FLAGS_CONNECTED`， 并且将 UDP 控制块插入 `udp_pcbs` 链表中。

    ```c
    err_t udp_connect(struct udp_pcb *pcb,const ip_addr_t *ipaddr,u16_t port);
    ```

  - `udp_disconnect()`

    清除控制块的远端IP地址与端口号， 并且将UDP控制块的状态清除，当然，断开会话也不会发送任何的信息到对端主机中。

    ```c
    void udp_disconnect(struct udp_pcb *pcb);
    ```

  - `udp_recv()`

    设置UDP控制块中的 `recv` 与 `recv_arg` 字段，这在 UDP 控制块就已经讲解的内容，`recv` 是一个函数指针，指向一个 `udp_recv_fn` 类型的回调函数，它非常重要，是内核与应用程序交互的桥梁，当内核接收到数据的时候，就会调用这个回调函数，进而将数据递交到应用层处理，在 `recv` 回调函数中，`pcb`、`p`、`addr`、`port` 等作为参数传递进去，方便用户的处理，其中 `pcb` 就是指向 UDP 控制块的指针，标识一个 UDP 会话，`p` 是指向 `pbuf` 的指针，里面包含着接收到的数据，而 `addr` 与 `port` 记录着发送数据段的 IP 地址与端口号。

    ```c
    typedef void (*udp_recv_fn)(void *arg,struct udp_pcb *pcb,struct pbuf *p,const ip_addr_t *addr,u16_t port);
    
    void udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg);
    ```

  - `udp_send()`/`udp_sendto()`

    UDP 发送数据也是依赖 IP 层，在用户使用发送数据的时候，应该为数据开辟一个 `pbuf` 用于存储数据， 并且 `pbuf` 中为 UDP、IP、以太网首部预留足够的空间，然后用户调用`udp_send()` 或者 `udp_sendto()` 函数将 `pbuf` 作为参数传递进去， 在发送数据的时候，UDP 协议会将 UDP 首部相关的内容进行填充，形成一个完整的 UDP 报文递交到IP层，IP层也会为这个数据报添加 IP 首部， 形成完整的 IP 数据报递交到链路层中，然后添加以太网首部再发送出去。

    ```c
    err_t udp_send(struct udp_pcb *pcb, struct pbuf *p);
    
    err_t udp_sendto(struct udp_pcb *pcb, struct pbuf *p,const ip_addr_t *dst_ip, u16_t dst_port);
    ```

## 2. TCP 接口

|RAW 的 API 函数 |函数功能描述|
|-|-|
|`tcp_new()` |创建一个 TCP 的 PCB 控制块|
|`tcp_bind()`| 为 TCP 的 PCB 控制块绑定一个本地 IP 地址和端口号|
|`tcp_listen()`| 开始 TCP 的 PCB 监听|
|`tcp_accept()` |控制块 accept 字段注册的回调函数，侦听到连接时被调用|
|`tcp_accepted()` |通知 LWIP 协议栈一个 TCP 连接被接受了|
|`tcp_connect()` |连接远端主机|
|`tcp_write()`| 构造一个报文并放到控制块的发送缓冲队列中|
|`tcp_sent()` |控制块 sent 字段注册的回调函数，数据发送成功后被回调|
|`tcp_output()` |将发送缓冲队列中的数据发送出去|
|`tcp_recv()` |控制块 recv 字段注册的回调函数，当接收到新数据时被调用|
|`tcp_recved()`| 当程序处理完数据后一定要调用这个函数，通知内核更新接收窗口轮询函数 |
|`tcp_poll()` |控制块 poll 字段注册的回调函数，该函数周期性调用|
|`tcp_close()` |关闭一个 TCP 连接|
|`tcp_err()` |控制块 err 字段注册的回调函数，遇到错误时被调用|
|`tcp_abort()` |中断 TCP 连接|

- TCP 接口

  - `tcp_new()`

    用于分配一个 TCP 控制块，它通过 `tcp_alloc()` 函数分配一个 TCP 控制块结构来存储 TCP 控制块的数据信息， 如果没有足够的内容分配空间，那么 `tcp_alloc() `函数就会尝试释放一些不太重要的 TCP 控制块， 比如就会释放处于`TIME_WAIT`、`CLOSING` 等状态的 TCP 控制块，或者根据控制块的优先级进行释放， 释放一些不重要的 TCP 控制块，以完成新 TCP 控制块的分配，分配完成后，内核会初始化 TCP 控制块的各个字段内容。

    ```c
     struct tcp_pcb *tcp_new(void);
    ```

  - `tcp_bind()`

    对应 TCP 服务器端的程序，一般在创建一个 TCP 控制块的时候，就会调用 `tcp_bind()` 函数将本地的 IP 地址、端口号与一个控制块进行绑定。

    ```c
    err_t tcp_bind(struct tcp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port);
    ```

  - `tcp_listen()`

    让服务器处于监听状态，等待 TCP 客户端的连接并且去处理。

    ```c
    struct tcp_pcb *tcp_listen_with_backlog_and_err(struct tcp_pcb *pcb,u8_t backlog,err_t *err);
    ```

  - `tcp_accept()`

    在服务器端，处理客户端连接的函数。当让服务器进入监听状态后，就需要立即调用这个函数，它向监听 TCP 控制块中的`accept` 字段注册一个 `tcp_accept_fn` 类型的函数，当检测到客户端的连接时，内核就会调用这个回调函数，以完成连接操作，而在 `accept()` 函数中，需要用户去处理这些连接。

    ```c
    typedef err_t (*tcp_accept_fn)(void *arg,struct tcp_pcb *newpcb,err_t err);
    
    void tcp_accept(struct tcp_pcb *pcb,tcp_accept_fn accept);
    ```

  - `tcp_connect()`

    在客户端，主动与服务器建立连接的函数。将 TCP 控制块从`tcp_bound_pcbs` 绑定链表中取下并且放到 `tcp_active_pcbs` 链表中，并且发送一个连接请求报文，不过在处理这些事情之前，它会填写 TCP 控制块中发送窗口与接收窗口的相关字段，以达到最适的 TCP 连接，然后调用`tcp_enqueue_flags()`函数构造一个连接请求报文，将SYN标志置1。在使用这个函数的时候，除了需要传递 IP 地址与端口号以外，还需要传入一个 `tcp_connected_fn` 类型的回调函数， 内核会自动注册在 TCP 控制块中，当建立连接之后，就会调用 `connected()` 这个回调函数。

    ```c
    typedef err_t (*tcp_connected_fn)(void *arg,struct tcp_pcb *tpcb,err_t err);
    
    err_t tcp_connect(struct tcp_pcb *pcb,const ip_addr_t *ipaddr,u16_t port,tcp_connected_fn connected);
    ```

  - `tcp_close()`

    主动终止一个 TCP 连接。当 TCP 控制块处于关闭状态 CLOSED 的时候，会将 TCP 控制块从绑定链表`tcp_bound_pcbs` 中移除，并且释放 TCP 控制块的内存空间；当 TCP 控制块处于监听状态的时候， 那么会将 TCP 控制块从监听链表 `tcp_listen_pcbs` 中移除，并且释放控制块的内存空间；当 TCP 控制块处于 SYN_SENT 状态时， 就将 TCP 控制块从 `tcp_active_pcbs` 链表中删除，并且释放控制块的内存空间；而对于处于其他状态的 TCP 控制块，直接通过 `tcp_close_shutdown_fin()` 函数来处理，主动关闭 TCP 连接。

    ```c
    err_t tcp_close(struct tcp_pcb *pcb);
    ```

  - `tcp_recv()`

    向控制块中的 `recv` 字段注册一个回调函数，当内核收到数据的时候就会调用这个回调函数，进而让数据递交到应用层中。回调函数的传入参数有4个，其中主要的是 `tpcb`，它是 TCP 控制块，表示了哪个 TCP 连接； `p` 是 `pbuf` 指针，指向接收到数据的 `pbuf` ，当内核检测到对方主动终止 TCP 连接的时候，也会触发回调函数，此时的 `pbuf` 为空， 而对于这种情况，用户就需要进行处理，也需要调用 `tcp_close()` 函数来终止本地到远端方向上的 TCP 连接。一般来说，这个函数在 `connected() `函数中调用。

    ```c
    typedef err_t (*tcp_recv_fn)(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);
    
    void tcp_recv(struct tcp_pcb *pcb, tcp_recv_fn recv);
    ```

  - `tcp_sent()`

    注册一个发送的回调函数，即将一个 `tcp_sent_fn` 类型的函数注册到 TCP 控制块的 `sent` 字段中， 当发送的数据被对方确认接收后，内核会将发送窗口向后移动，并且调用这个注册的回调函数告诉应用，数据已经被对方接收了，那么用户就可以根据这个函数来将那些已经发送的数据删除掉或者发送新的数据。

    ```c
    typedef err_t (*tcp_sent_fn)(void *arg,struct tcp_pcb *tpcb,u16_t len);
    
    void tcp_sent(struct tcp_pcb *pcb, tcp_sent_fn sent);
    ```

  - `tcp_err()`

    注册一个异常处理的函数，它向 TCP 控制块的 `err` 字段中注册一个 `tcp_err_fn` 类型的异常处理函数，用户需要自行编写这个函数，可以拥有完成在连接异常的一些处理，比如连接失败的时候，可以释放 TCP 控制块的内存空间、 或者选择重连等等。

    ```c
    typedef void (*tcp_err_fn)(void *arg, err_t err);
    
    void tcp_err(struct tcp_pcb *pcb, tcp_err_fn err);
    ```

  - `tcp_poll()`

    在 TCP 控制块的 `poll` 字段注册一个类型为 `tcp_poll_fn` 的回调函数，内核会周期性调用控制块中的`poll` 回调函数，调用的周期为 `interval` * 0.5s，因为0.5s 是内核定时器的处理周期，用户可以适当使用 `poll` 回调函数完成一些周期性的事件，比如检测连接的情况、周期性发送一些数据等等。

    ```c
    typedef err_t (*tcp_poll_fn)(void *arg, struct tcp_pcb *tpcb);
    
    void tcp_poll(struct tcp_pcb *pcb, tcp_poll_fn poll, u8_t interval);
    ```

  - `tcp_write()`

    发送 TCP 数据，并不是一经使用就会立刻发送数据，而是指定的数据放到发送队列，由协议内核来决定发送，发送队列中可用字节大小并通过函数 `tcp_sendbuf()`来获取，使用该函数就是返回字节大小的数据，如果该函数返回值为 `ERR_MEM`，那么应用程序需要等待一段时间，直到发送队列中的数据被远程主机成功接收，然后才可以发送出去。该函数可以用来构建 TCP 报文段。

    ```c
     err_t tcp_write(struct tcp_pcb *pcb,const void *dataptr,u16_t len,u8_t apiflags);
    ```

  - `tcp_recved()`

    在用户接收到数据之后，调用此函数来更新接收窗口，因为内核不知道应用层是否真正接收到数据，如果不调用这个函数，就没法进行确认，而发送的一方会认为对方没有接收到，因此会重发数据。在这个函数中，调用 `tcp_update_rcv_ann_wnd()` 函数进行更新接收窗口，以告知发送方能发送多大的数据，参数`len` 表示应用程序已经处理完的数据长度，那么接收窗口也会增大 `len` 字节的长度。

    ```c
    void tcp_recved(struct tcp_pcb *pcb, u16_t len);
    ```

    
