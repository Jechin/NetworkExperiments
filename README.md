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

![image-20210514222120038](https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210514222120038.png)

#### Server

* 循环服务器

#### Client

* 客户端接收信息时需要循环接收

### Echoline

* 服务器反射客户端发送的数据的一行

![image-20210514223609695](https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210514223609695.png)

#### Server

* 循环服务器
* 接收到客户端发来的信息后根据'\n'字符进行切割
* 将一行信息发送给客户端

#### Client

* 客户端接收信息时需要循环接收

### Echofix

* 客户端发送数据，客户端反射定长数据

![image-20210515173144728](https://raw.githubusercontent.com/Jechin/PicLib/main/image/image-20210515173144728.png)

#### server

* 循环服务器
* 定长接收数据后，加上“echo”前缀反射给客户端

#### client

* 使用定长接收数据









