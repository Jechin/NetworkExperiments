//server
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <iostream>

#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>


#define MAXLINE 4096     
#define LISTENQ 1024     
#define SERVER_PORT 13131
#define CLIENT_PORT 13131
#define TIMEPORT 10000

/*tcp服务器端初始化*/
int tcpServerInit(u_short port)
{
    int res;
    int sock_conn;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_conn = socket(AF_INET, SOCK_STREAM, 0);    //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }

    if (bind(sock_conn, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0) //绑定端口地址
    {
        //错误处理
        perror("error");
        fprintf(stderr, "bind failed \n");
        exit(-1);
    }
    
    printf("Bind Sucessfully. Server Starts!\n");
    return sock_conn;
}

/*接收数据并回送客户端*/
int RecvAndSend(int sock_conn, char * recvbuff, int maxlength)
{
    int n,result;
    char buff[1000 + 5];
    char echo[5] = {'E', 'C', 'H', 'O', ':'};
    int lengthset = 1000;   //设置数据长度为1000
    if(1000 > MAXLINE)  //设定长度大与缓冲区最大长度时重新设定数据长度
    {
        printf("Input a length less than %d: ", MAXLINE);
        std::cin>>lengthset;
    }

    do
    {
        memset(buff, 0 ,sizeof(buff));
        memset(recvbuff, 0, sizeof(recvbuff));

        result = recv(sock_conn, buff, lengthset, 0); //定长接受1000字节的信息
        std::cout<<"The Data Recveiced From CLient Is : "<<buff<<std::endl;
        for(int i = 0; i < sizeof(buff); i++)   //数据头部增加echo:
        {
            if(i<5) recvbuff[i] = echo[i];
            else recvbuff[i] = buff[i-5];
        }

        n = send(sock_conn, recvbuff, lengthset+5, 0);  //发送数据长度，数据长度加5
        if(n > 0) printf("Echo Successfully!\n");
        else
        {
            //错误处理
            perror("Error");
            result = -1;
            break;
        }
    }while(result > 0);

    return result;
}

int main()
{
    int sock_server;    //服务器套接字
    int sock_conn;      //连接套接字

    char recvbuff[MAXLINE + 1];
    char sendbuff[MAXLINE + 1];

    int result;

    sock_server = tcpServerInit(CLIENT_PORT);   //服务器套接字初始化

    listen(sock_server, LISTENQ);   //开始监听

    for( ; ; )
    {
        memset(recvbuff, 0, sizeof(recvbuff));
        sock_conn = accept(sock_server, (struct sockaddr*) NULL, NULL);    //tcp建立连接
        printf("Accept a Connection!\n");

        RecvAndSend(sock_conn, recvbuff, MAXLINE);

        if(result == 0) printf("Client Close.\n");
    }

    close(sock_conn);
    printf("Connection closed.\n");

    return 0;
}