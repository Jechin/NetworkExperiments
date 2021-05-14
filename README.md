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

```sequence
Title: time server
Note over server: bind()
Note over server: lisetn()
Note over server: accept()
client->server: connect()
server->client: send(time)
note over client: recv()
note over server, client: close()
```



#### Server

* 循环服务器

#### Client

* 客户端接收信息时需要循环接收















