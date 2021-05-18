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
#define PORT    9999


/*udp客户端初始化*/
int udpClientInit()
{
    int sock_conn;

    sock_conn = socket(AF_INET, SOCK_DGRAM, 0);    //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }
    return sock_conn; 
}

int SendAndRecv(int sock_conn, char *server_ip, char *sendbuff, char *recvbuff)
{
    int result;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    socklen_t add_len;

    while (printf("Input: ") && std::cin.getline(sendbuff, MAXLINE)) //循环读入行
    {
        if (*sendbuff == 'Q' || *sendbuff == 'q')   //输入Q时退出
        {
            std::cout << "Input End!" << std::endl;
            result = shutdown(sock_conn, SHUT_WR);
            result = close(sock_conn);
            if (result < 0)
            {
                //错误处理
                perror("Error");
                printf("Quit Error!\n");
            }
            break;
        }

        result = sendto(sock_conn, sendbuff, (int)strlen(sendbuff), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if(result <= 0)
        {
            perror("error");
            printf("Send Data Error.\n");
            continue;
        }

        printf("Send to %s\n", inet_ntoa(server_addr.sin_addr));
        std::cout<<"The data sended to server is: "<<sendbuff<<std::endl;

        result = recvfrom(sock_conn, recvbuff, MAXLINE, 0, (sockaddr *)&server_addr, &add_len);
        if(result <= 0)
        {
            perror("error");
            printf("Recv Data Error.\n");
            continue;
        }
        printf("Receive From %s\n", inet_ntoa(server_addr.sin_addr));
        std::cout << "The Data Received From Server Is:" << recvbuff << std::endl;

        memset(sendbuff, 0, MAXLINE);
        memset(recvbuff, 0, MAXLINE);
    }
    return -1;
}



int main()
{
    int udp_client, result;
    char recvbuff[MAXLINE];
    char sendbuff[MAXLINE];
    char server_ip[] = "127.0.0.1";

    udp_client = udpClientInit();
    result = SendAndRecv(udp_client, server_ip, sendbuff, recvbuff);

    close(udp_client);
    return 0;
}