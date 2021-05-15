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
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //任意地址连接

    sock_conn = socket(AF_INET, SOCK_STREAM, 0);    //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }

    if (bind(sock_conn, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0) //利用bind函数将端口和地址与套接字相连接
    {
        //错误处理
        perror("error");
        fprintf(stderr, "bind failed \n");
        exit(-1);
    }
    
    printf("Bind Sucessfully. Server Starts!\n");
    return sock_conn;
}

/*定长接受数据*/
int recvn(int sock_conn, char * buff, int length)
{
    int res, cnt;
    cnt=length;
    while(cnt>0)    //循环接受
    {
        res=recv(sock_conn,buff,cnt,0); 
        if (res > 0)
        {
            std::cout << "The Data Received From Client Is: " << buff << std::endl;
            return 1;
        }
        else
        {

            if (res == 0)
            {
                printf("Server is closed!\n");
                return length-cnt;
            }
            else
            {
                printf("Receive Error!");
                perror("Error");
                res = -1;
                return -1;
            }
        }
        buff+=res;  //接受到的数据进行拼接
        cnt-=res;   //更新剩余长度
    }
    return length;
}

/*变长数据接收*/
int recvvl(int sock_conn, char * buff, int maxlength)
{
    int result=0;
    int len;
    result = recv(sock_conn, (char *)&len, sizeof(int), 0); //接收发送的长度
    if(result < 0)
    {
        //错误处理
        perror("Error");
        printf("Receive Length Error!\n");
        return -1;
    }
    else if(result == 0)
    {
        printf("Client closed!\n");
    }
    else
    {
        std::cout<<"result: "<<result<<std::endl;
        len = ntohl(len);   //字节序转换
        printf("Receive length successfully, the length is %d. \n", len);
    }
    if ( len > maxlength )
    {
        printf("The length of the message is bigger than receive cache!\n");
        //长度超出缓存区最大长度不予接收
        return -1;
    }
    result = recvn( sock_conn, buff, len ); //按接收到的长度进行定长数据接收
    
    return result;
}

/*接受信息 添加echo: 并转发函数*/
int RecvAndSend(int sock_conn, char * recvbuff, int maxlength)
{
    int n,result;
    char  buff[maxlength + 5];
    char echo[5] = {'E', 'C', 'H', 'O', ':'};
    int len;
    
    do
    {
        memset(buff, 0 ,sizeof(buff));
        memset(recvbuff, 0, sizeof(recvbuff));

        result = recvvl(sock_conn, buff, maxlength);    //变长数据接受至buff缓存区
        if(result < 0)
        {
            result = 0;
            break;
        }

        for(int i = 0; i < strlen(buff) + 5; i++)   //在数据前添加echo:头部
        {
            if(i<5) recvbuff[i] = echo[i];
            else recvbuff[i] = buff[i-5];
        }

        len = strlen(recvbuff);
        len = htonl(len);
        result = send(sock_conn, (char *)&len, sizeof(int), 0); //首先发送echo信息长度
        if (result < 0)
        {
            //错误处理
            perror("Error");
            printf("Send Length Error!\n");
            break;
        }
        else printf("Send length successfully, the length is %d.\n", ntohl(len));

        n = send(sock_conn, recvbuff, (int) ntohl(len), 0); //发送数据
        if(n > 0) printf("Echo Successfully!\n\n");
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

    sock_server = tcpServerInit(CLIENT_PORT);   //服务器端初始化

    listen(sock_server, LISTENQ);   //启动监听

    for( ; ; )  //循环等待连接
    {
        memset(recvbuff, 0, sizeof(recvbuff));
        sock_conn = accept(sock_server, (struct sockaddr*) NULL, NULL) ;    //建立连接
        printf("Accept a Connection!\n");

        RecvAndSend(sock_conn, recvbuff, MAXLINE);

        if(result == 0) printf("Client Close.\n");
    }

    close(sock_conn);
    printf("Connection closed.\n");

    return 0;
}