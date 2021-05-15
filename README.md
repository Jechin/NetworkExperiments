# NetworkExperiments
Some computer network experiments about the echo of C/S model.

## Socket

Introduction of some important function.

### TCP

#### send()

* int send( SOCKET s,char *buf,int len,int flags );

* Send 函数的实际功能并不是发送数据
* 实际功能：将用户缓冲区buf中的数据拷贝至内核的发送缓冲区
* 数据在网络中的发送和传输是由协议完成的（对程序员不可见）

#### recv()

* int recv( SOCKET s, char *buf, int  len, int flags)
* recv函数的功能也不是接收数据
* 实际功能：从内核接受缓冲区中将长度为len的数据拷贝至用户缓冲区buf
* 数据的接收是由协议完成的

#### summary

* 因为send和recv函数的上述特性，send和recv函数不是一一对应的
* 因此在流式套接字编程中调用send和recv函数接收数据时，需要循环接收数据

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

#### server

* 循环服务器
* 定长接收数据后，加上“echo”前缀反射给客户端

#### client

* 使用定长接收数据

### Echovaria

* 客户端与服务器之间变长发送数据并回射
* 变长数据接收并回射的实现
  * 通过两次定长数据接收实现
  * 第一次传送数据长度，第二次传送协商好数据长度的数据

<img src="https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515184702490.png" alt="image-20210515184702490" width="500" />

#### server

* 循环服务器
* 两次定长接收数据
* 在协商长度时，要注意主机字节序到网络字节序的转换

#### client

* 两次发送数据，第一次发送信息长度，第二次发送信息主体
* 发送信息长度时，注意主机字节序到网络字节序的转换







