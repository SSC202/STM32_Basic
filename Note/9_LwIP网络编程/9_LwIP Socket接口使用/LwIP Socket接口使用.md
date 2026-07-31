# STM32 LwIP Socket接口

## 1. Socket 接口简介

LwIP 作者为了能更大程度上方便开发者将其他平台上的网络应用程序移植到 LwIP 上，也为了能让更多开发者快速上手 LwIP，设计了第三种应用程序编程接口，即 Socket API，但是该接口受嵌入式处理器资源和性能的限制，部分 Socket 接口并未在 LwIP 中完全实现。

> 在 Socket 中，使用一个套接字来记录网络的一个连接，套接字是一个整数，就像操作文件一样，利用一个文件描述符，可以对它打开、读、写、关闭等操作，类似的，在网络中，也可以对Socket套接字进行这样的操作，比如开启一个网络的连接、读取连接主机发送来的数据、向连接的主机发送数据、终止连接等操作。

在 LwIP 中，Socket API 是基于 NETCONN API 之上来实现的，系统最多提供 `MEMP_NUM_NETCONN` 个 netconn 连接结构，因此 Socket 套接字的个数也是`MEMP_NUM_NETCONN` 个，为了更好对`netconn` 进行封装，LwIP 还定义了一个套接字结构体 —— `lwip_sock`（Socket连接结构），每个 `lwip_sock` 内部都有一个 netconn 的指针，实现对 netconn 的再次封装。

```c
 #define NUM_SOCKETS MEMP_NUM_NETCONN

 /** 全局可用套接字数组（默认是4） */
 static struct lwip_sock sockets[NUM_SOCKETS];

 union lwip_sock_lastdata
 {
     struct netbuf *netbuf;
     struct pbuf *pbuf;
 };

 /** 包含用于套接字的所有内部指针和状态 */
 struct lwip_sock
 {
     /** 套接字当前是在netconn上构建的，每个套接字都有一个netconn */
     struct netconn *conn;
     /** 从上一次读取中留下的数据 */
     union lwip_sock_lastdata lastdata;
     /** 收到数据的次数由event_callback()记录，下面的字段在select机制上使用 */
     s16_t rcvevent;
     /** 发送数据的次数，也是由回调函数记录的 */
     u16_t sendevent;
     /** Socket上的发生的错误次数 */
     u16_t errevent;
     /** 使用select等待此套接字的线程数 */
     SELWAIT_T select_waiting;
 };
```

- Socket 接口函数

  - `socket()`

    向内核申请一个套接字，在本质上该函数其实就是对 `netconn_new()` 函数进行了封装。

    ```c
    #define socket(domain,type,protocol) lwip_socket(domain,type,protocol)
    
    /**
      * @param	domain	该套接字使用的协议簇,对于TCP/IP协议来说，该值始终为AF_INET。
      * @param	type	指定了套接字使用的服务类型
      *			SOCK_STREAM：提供可靠的（即能保证数据正确传送到对方）面向连接的 Socket 服务，多用于资料（如文件）传输，如 TCP 协议。
      *			SOCK_DGRAM：是提供无保障的面向消息的 Socket 服务，主要用于在网络上发广播信息，如UDP协议，提供无连接不可靠的数据报交付服务。
      * 		SOCK_RAW：表示原始套接字，它允许应用程序访问网络层的原始数据包。
      * @param	protocol 	指定了套接字使用的协议，在IPv4中，只有TCP协议提供SOCK_STREAM这种可靠的服务，只有UDP协议提供SOCK_DGRAM服务，对于这两种协议，protocol的值均为0。
      */
    int lwip_socket(int domain, int type, int protocol);
    
    #define AF_INET         2
    
    /* Socket服务类型 (TCP/UDP/RAW) */
    #define SOCK_STREAM     1
    #define SOCK_DGRAM      2
    #define SOCK_RAW        3
    ```

  - `bind()`

    功能与 `netconn_bind()` 函数是一样的，用于服务器端绑定套接字与网卡信息， 实际上就是对 `netconn_bind()` 函数进行了封装，可以将一个申请成功的套接字与网卡信息进行绑定。

    ```c
    #define bind(s,name,namelen) lwip_bind(s,name,namelen)
    
    /**
      * @param	s 		要绑定的Socket套接字
      * @param	name 	一个指向sockaddr结构体的指针，其中包含了网卡的IP地址、端口号等重要的信息
      */
    int lwip_bind(int s,const struct sockaddr *name,socklen_t namelen);
    
    struct sockaddr
    {
        u8_t        sa_len;     		/* 长度 */
        sa_family_t sa_family;  		/* 协议簇 */
        char        sa_data[14];        /* 连续的14字节信息 */
    };
    
    // 一般使用以下结构体
    // sin_port字段是端口号信息，sin_addr字段是IP地址信息
    struct sockaddr_in
    {
        u8_t            sin_len;
        sa_family_t     sin_family;
        in_port_t       sin_port;
        struct in_addr  sin_addr;
    #define SIN_ZERO_LEN 8
        char            sin_zero[SIN_ZERO_LEN];
    };
    ```

  - `connect()`

    与 `netconn_connect()` 函数的作用基本一致。

    ```c
    #define connect(s,name,namelen) lwip_connect(s,name,namelen)
    
    int lwip_connect(int s,const struct sockaddr *name,socklen_t namelen);
    ```

  - `listen()`

    与 `netconn_listen()` 函数的作用基本一致。

    ```c
    #define listen(s,backlog) lwip_listen(s,backlog)
    
    int lwip_listen(int s, int backlog);
    ```

  - `accept()`

    与 `netconn_accept()` 函数的作用基本一致。用于 TCP 服务器中，等待着远端主机的连接请求， 并且建立一个新的 TCP 连接，在调用这个函数之前需要通过调用`listen()` 函数让服务器进入监听状态。`accept()` 函数的调用会阻塞应用线程直至与远程主机建立 TCP 连接。参数 `addr` 是一个返回结果参数，值由 `accept()` 函数设置，其实就是远程主机的地址与端口号等信息，当新的连接已经建立后，远端主机的信息将保存在连接句柄中，它能够唯一的标识某个连接对象。同时函数返回一个int类型的套接字描述符，根据它能索引到连接结构，如果连接失败则返回-1。

    ```c
    #define accept(s,addr,addrlen) lwip_accept(s,addr,addrlen)
    int lwip_accept(int s,struct sockaddr *addr,socklen_t *addrlen);
    ```

  - `read()`/`recv()`/`recvfrom()`

    `read()` 与 `recv()` 函数的核心是调用 `recvfrom()` 函数，而 `recvfrom()` 函数是基于 `netconn_recv()` 函数来实现的，`recv()` 与 `read()` 函数用于从Socket 中接收数据，它们可以是 TCP 协议和 UDP 协议。

    ```c
    #define read(s,mem,len) lwip_read(s,mem,len)
    ssize_t lwip_read(int s, void *mem, size_t len)
    {
        return lwip_recvfrom(s, mem, len, 0, NULL, NULL);
    }
    
    #define recv(s,mem,len,flags) lwip_recv(s,mem,len,flags)
    ssize_t lwip_recv(int s, void *mem, size_t len, int flags)
    {
        return lwip_recvfrom(s, mem, len, flags, NULL, NULL);
    }
    
    #define recvfrom(s,mem,len,flags,from,fromlen) lwip_recvfrom(s,mem,len,flags,from,fromlen)
    ssize_t lwip_recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen);
    ```

  - `sendto()`

    用于 UDP 协议传输数据中，它向另一端的 UDP 主机发送一个 UDP 报文，本质上是对 `netconn_send()` 函数的封装，参数 `data` 指定了要发送数据的起始地址，`size` 则指定数据的长度，参数 `flag` 指定了发送时候的一些处理，比如外带数据等，一般设置为0即可，参数 `to` 是一个指向 `sockaddr` 结构体的指针，需要提供远端主机的IP地址与端口号，并且用 `tolen` 参数指定这些信息的长度。

    ```c
    #define sendto(s,dataptr,size,flags,to,tolen) lwip_sendto(s,dataptr,size,flags,to,tolen)
    
    ssize_t lwip_sendto(int s, const void *data, size_t size, int flags,const struct sockaddr *to, socklen_t tolen);
    ```

  - `send()`

    `send()` 函数可以用于 UDP 协议和 TCP 连接发送数据。在调用 `send()` 函数之前，必须使用 `connect()` 函数将远端主机的IP地址、 端口号与Socket连接结构进行绑定。对于 UDP 协议，`send()` 函数将调用 `lwip_sendto()` 函数发送数据，而对于 TCP 协议， 将调用 `netconn_write_partly() `函数发送数据。

    ```c
    #define send(s,dataptr,size,flags) lwip_send(s,dataptr,size,flags)
    ssize_t lwip_send(int s, const void *data, size_t size, int flags);
    ```

  - `write()`

    一般用于处于稳定的 TCP 连接中传输数据，当然也能用于 UDP 协议中。

    ```c
    #define write(s,dataptr,len) lwip_write(s,dataptr,len)
    
    ssize_t lwip_write(int s, const void *data, size_t size)
    {
        return lwip_send(s, data, size, 0);
    }
    ```

  - `close()`

    用于关闭一个指定的套接字，在关闭套接字后，将无法使用对应的套接字描述符索引到连接结构， 该函数的本质是对`netconn_delete()`函数的封装，如果连接是 TCP 协议，将产生一个请求终止连接的报文发送到对端主机中，如果是 UDP 协议，将直接释放 UDP 控制块的内容。

    ```c
    #define close(s) lwip_close(s)
    
    int lwip_close(int s)
    ```

  - `ioctl()`/`ioctlsocket()`

    获取与设置套接字相关的操作参数，参数 `cmd` 指明对套接字的操作命令，在LwIP中只支持 `FIONREAD` 与 `FIONBIO` 命令：

    - `FIONREAD` 命令确定套接字s自动读入的数据量，这些数据已经被接收，但应用线程并未读取的，可以使用这个函数来获取这些数据的长度，在这个命令状态下，`argp` 参数指向一个无符号长整型，用于保存函数的返回值（即未读数据的长度）。如果套接字是 `SOCK_STREAM` 类型，则 `FIONREAD` 命令会返回 `recv()` 函数中所接收的所有数据量，这通常与在套接字接收缓存队列中排队的数据总量相同；而如果套接字是 `SOCK_DGRAM` 类型的，则`FIONREAD` 命令将返回在套接字接收缓存队列中排队的第一个数据包大小。
    - `FIONBIO` 命令用于允许或禁止套接字的非阻塞模式。在这个命令下，`argp` 参数指向一个无符号长整型， 如果该值为0则表示禁止非阻塞模式，而如果该值非0则表示允许非阻塞模式则。当创建一个套接字的时候，它就处于阻塞模式，也就是说非阻塞模式被禁止，这种情况下所有的发送、接收函数都会是阻塞的，直至发送、接收成功才得以继续运行；而如果是非阻塞模式下，所有的发送、接收函数都是不阻塞的，如果发送不出去或者接收不到数据，将直接返回错误代码给用户，需要用户对这些意外情况进行处理，保证代码的健壮性，这与BSD Socket是一致的。

    ```c
    #define ioctl(s,cmd,argp) lwip_ioctl(s,cmd,argp)
    #define ioctlsocket(s,cmd,argp) lwip_ioctl(s,cmd,argp)
    
    int lwip_ioctl(int s, long cmd, void *argp);
    ```

  - `setsockopt()`

    设置套接字的一些选项。

    ```c
    #define setsockopt(s,level,optname,opval,optlen) lwip_setsockopt(s,level,optname,opval,optlen)
    
    /**
      * @param	level 		SOL_SOCKET：表示在Socket层。
      						IPPROTO_TCP：表示在TCP层。
    						IPPROTO_IP： 表示在IP层。
      * @param	optname 	对于SOL_SOCKET选项，可以是SO_REUSEADDR（允许重用本地地址和端口）、SO_SNDTIMEO（设置发送数据超时时间）、 SO_SNDTIMEO（设置接收数据超时时间）、SO_RCVBUF（设置发送数据缓冲区大小）等等。
    						对于IPPROTO_TCP选项，可以是TCP_NODELAY（不使用Nagle算法）、TCP_KEEPALIVE（设置TCP保活时间）等等。
    						对于IPPROTO_IP选项，可以是IP_TTL（设置生存时间）、IP_TOS（设置服务类型）等等。
      */
    int lwip_setsockopt(int s,int level,int optname,const void *optval,socklen_t optlen);
    ```

    