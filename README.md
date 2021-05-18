# NetworkExperiments
Some computer network experiments about the echo of C/S model.

## Socket

Introduction of some important function.

### TCP

#### send()

```c++
int send(SOCKET s, char *buf, int len, int flags);
```

* `send` 函数的实际功能并不是发送数据
* 实际功能：将用户缓冲区`buf`中的数据拷贝至内核的发送缓冲区
* 数据在网络中的发送和传输是由协议完成的（对程序员不可见）

#### recv()

```c++
int recv( SOCKET s, char *buf, int  len, int flags)
```



* `recv`函数的功能也不是接收数据
* 实际功能：从内核接受缓冲区中将长度为`len`的数据拷贝至用户缓冲区`buf`
* 数据的接收是由协议完成的

#### summary

* 因为`send`和`recv`函数的上述特性，`send`和`recv`函数不是一一对应的
* 因此在流式套接字编程中调用`send`和`recv`函数接收数据时，需要循环接收数据

### UDP

#### sendto()

```c++
int sendto (int s, const void *buf, int len, unsigned int flags, const struct sockaddr *to, int tolen);
```

* sendto函数实现将`buf`中的内容拷贝内核缓冲区
* 向`to `地址(IP地址和端口信息)发送buf中的内容

#### recvfrom()

```c++
int recvfrom(int s, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
```

* `recvfrom`函数实现无连接的接收数据报
* 同时发送方的IP地址和端口信息会被复制到`from`中

#### summary

* 对于数据报类套接口，必需注意发送数据长度不应超过通讯子网的IP包最大长度
* 相较于TCP流式套接字的传输，UDP套接字的传输在不超过通讯子网的最大长度的情况下，`sendto`和`recvfrom`基本是一一对应的

## TCP

### Time Server

* 服务器返回当前时间

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210514222120038.png" alt="image-20210514222120038" width="500" />

#### Server

* 循环服务器

#### Client

* 客户端接收信息时需要循环接收

### Echoline

* 服务器反射客户端发送的数据的一行

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210514223609695.png" alt="image-20210514223609695" width="500" />

#### Server

* 循环服务器
* 接收到客户端发来的信息后根据'\n'字符进行切割
* 将一行信息发送给客户端

#### Client

* 客户端接收信息时需要循环接收

### Echofix

* 客户端发送数据，客户端反射定长数据

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515173144728.png" alt="image-20210515173144728" width="500" />

#### Server

* 循环服务器
* 定长接收数据后，加上“echo”前缀反射给客户端

#### Client

* 使用定长接收数据

### Echovaria

* 客户端与服务器之间变长发送数据并回射
* 变长数据接收并回射的实现
  * 通过两次定长数据接收实现
  * 第一次传送数据长度，第二次传送协商好数据长度的数据

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515184702490.png" alt="image-20210515184702490" width="500" />

#### Server

* 循环服务器
* 两次定长接收数据
* 在协商长度时，要注意主机字节序到网络字节序的转换

#### Client

* 两次发送数据，第一次发送信息长度，第二次发送信息主体
* 发送信息长度时，注意主机字节序到网络字节序的转换

### Fork

使用**多进程**实现服务器端的并发处理

#### Server

* Fork()可以复制一份主程序，即创建主进程的子进程，并在fork之后继续运行。
* 在父进程中fork()函数将会返回子进程的pid
* 而在子进程中fork()函数的返回值为0
* 在程序中对fork的返回值进行判断
  * 若为0，则是子进程，完成服务器的反射任务
  * 若为1，则是父进程，该当进入下一轮的循环，调用accpet阻塞函数，等待下一个连接

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515195046177.png" alt="image-20210515195046177" width="500" />

#### Client

* 与循环服务器的相同，无需改动

### Thread

使用多线程实现服务器端的并发处理

#### pthread_create

```c++
int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict_attr, void*（*start_rtn)(void*), void *restrict arg);
```

* Param 
  * tidp: 执行线程标识符的指针
  * restrict_attr: 线程属性，可取NULL
  * start_rtn: 线程函数指针
  * arg: 传递给线程函数的参数

#### Server

* 使用`pthread_create()`函数来创建子线程
* 要注意传递给线程函数的参数类型为`void*`类型，在TCP线程并发服务器中，需要传递的参数为连接套接字socket，其类型为`int`
  * 因此需要在调用`pthread_create()`时强制转换类型
  * 在线程函数中要将变量强制转换类型为`int`才能作为套接字使用

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515210200789.png" alt="image-20210515210200789" width="500" />

#### Client

* 与循环服务器的相同，无需改动

## UDP

### Echo

服务器端实现简单的客户端信息反射

![image-20210518092803702](https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210518092803702.png)

#### Server

* 使用循环服务器
* 接收数据后，加上“echo”前缀反射给客户端
* `recvfrom`在接收到数据后，`from`也被更新，发送数据时要以`from`为目的地址

#### Client

* 客户端的`recvfrom`函数中的发送方地址参数`to`不能指定发送方，而是会在收到数据报后更新地址



