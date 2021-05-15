//client
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

/*tcp客户端初始化*/
int tcpClientInit()
{
    int sock_conn;

    sock_conn = socket(AF_INET, SOCK_STREAM, 0);    //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }
    return sock_conn; 
}

/*套接字绑定地址端口并连接*/
int tcpClientConnect(int sock_conn, char *server_ip, u_short port)
{
    int res;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    res = connect(sock_conn, (sockaddr *)&server_addr, sizeof(server_addr));
    if (res < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "Connect failed\n");
        exit(-1);
    }

    printf("Echo Line Client Starts! \n");
    return 0;
}

/*定长接受数据*/
int recvn(int sock_conn, char * buff, int length)
{
    int res, cnt;
    cnt=length;
    while(cnt>0)
    {
        res=recv(sock_conn,buff,cnt,0);
        if (res > 0)
        {
            std::cout << "The Data Received From Server Is: " << buff << std::endl;
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
        buff+=res;  //缓冲区数据拼接
        cnt-=res;   //更新剩余长度
    }
    return length;
}

int main(int argc, char **argv)
{
    int sock_client, n, result;
    char recvbuff[MAXLINE + 1];
    char sendbuff[MAXLINE + 1];
    char server_ip[] = "127.0.0.1";
    int lengthset = 1000;   //设置定长为1000

    sock_client = tcpClientInit();
    tcpClientConnect(sock_client, server_ip, SERVER_PORT);

    while (printf("Input: ") && std::cin.getline(sendbuff, MAXLINE))
    {
        if (*sendbuff == 'Q' || *sendbuff == 'q')
        {
            printf("Input End!\n");
            result = close(sock_client);
            break;
        }
        
        result = send(sock_client, sendbuff, lengthset, 0); //发送定长数据
        if (result < 0)
        {
            //错误处理
            perror("Error");
            printf("Send Data Error!\n");
            break;
        }

        std::cout << "The Data Sent To Server Is: " << sendbuff << std::endl;
        

        result = recvn(sock_client, recvbuff, lengthset + 5);    //定长接受数据，长度为发送长度加5
        if(result<=0)
            break; 

        memset(recvbuff, 0, sizeof(recvbuff));
        memset(sendbuff, 0, sizeof(sendbuff));
    }

    return 0;
}
