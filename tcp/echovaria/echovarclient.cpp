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

/*客户端初始化*/
int tcpClientInit()
{
    int sock_conn;

    sock_conn = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_conn < 0)
    {
        //错误处理
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }
    
    return sock_conn; 
}

/*客户端地址端口绑定并连接*/
int tcpClientConnect(int sock_conn, char *server_ip, u_short port)
{
    int res;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip); //默认127.0.0.1本机地址

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
    while(cnt>0)    //循环接受
    {
        res=recv(sock_conn,buff,cnt,0);
        if (res > 0)
        {
            std::cout << "The Data Received From Server Is:" << buff << std::endl;
            return 1;
        }
        else
        {
            //错误处理
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

/*变长接收数据*/
int recvvl(int sock_conn, char * buff, int maxlength)
{
    int result=0;
    int len;
    result = recv(sock_conn, (char *)&len, sizeof(int), 0); //接收数据长度
    if(result < 0)
    {
        //错误处理
        perror("Error");
        printf("Receive Length Error!\n");
        return -1;
    }
    else
    {
        len = ntohl(len);   //字节序转换
        printf("Receive length successfully, the length is %d. \n", len);
    }
    if ( len > maxlength )
    {
        while(len>0)
        {
            result = recvn( sock_conn, buff, maxlength );
            if ( result == -1 )
            {
                perror("Error");
                printf("Connect Error!\n");
            }
            else if(!result) printf("Connection Closed!\n");
            
            len-=maxlength;
            if ( len < maxlength ) maxlength = len;
        }
        printf("The length of the message is bigger than receive cache!\n");
        //数据长度大于缓冲区最大长度时，不予接收
        return -1;
    }
    result = recvn( sock_conn, buff, len ); //按收到的数据长度定长接收

    printf("\n");   //分割
    return result;
}



int main(int argc, char **argv)
{
    int sock_client, n, result;
    char recvbuff[MAXLINE + 1];
    char sendbuff[MAXLINE + 1];
    char server_ip[] = "127.0.0.1"; //本机地址

    int len;

    sock_client = tcpClientInit();  //客户端套接字初始化
    tcpClientConnect(sock_client, server_ip, SERVER_PORT);  //套接字绑定并连接

    while (printf("Input: ") && std::cin.getline(sendbuff, MAXLINE)) //循环读入行
    {
        std::cout<<sendbuff<<std::endl;
        if (*sendbuff == 'Q' || *sendbuff == 'q')   //输入Q时退出
        {
            std::cout << "Input End!" << std::endl;
            //result = shutdown(sock_client, SHUT_WR);
            result = close(sock_client);
            if (result < 0)
            {
                //错误处理
                perror("Error");
                printf("Quit Error!\n");
            }
            break;
        }

        len = strlen(sendbuff);
        len=htonl(len);
        result = send(sock_client, (char *)&len, sizeof(int), 0);   //发送数据长度
        if (result < 0)
        {
            //错误处理
            perror("Error");
            printf("Send Data Error!\n");
            break;
        }
        else printf("Send length successfully, the length is %d. \n", ntohl(len));
        
        result = send(sock_client, sendbuff, (int)strlen(sendbuff), 0); //发送数据
        if (result < 0)
        {
            //错误处理
            perror("Error");
            printf("Send Data Error!\n");
            break;
        }

        std::cout << "The Data Sent To Server Is: " << sendbuff << std::endl;

        int flag;
        flag=recvvl(sock_client,recvbuff, MAXLINE); //变长接受服务器返回的消息
        if(flag<=0)
            break;
        
        memset(recvbuff, 0, sizeof(recvbuff));
        memset(sendbuff, 0, sizeof(sendbuff));
    }

    return 0;
}
