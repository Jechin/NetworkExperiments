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
#define PORT 9999

typedef struct set //设置的数据结构
{
    int times;
    int datasize;
    int buffsize;
    int recvspeed;
} set;

/*udp客户端初始化*/
int udpClientInit()
{
    int sock_conn;

    sock_conn = socket(AF_INET, SOCK_DGRAM, 0); //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }
    return sock_conn;
}

set buff2set(char *buff) //将输入缓冲区的数据拆解放入设置中
{
    int flag = 0;
    set setting;
    memset(&setting, 0, sizeof(setting));
    int temp;
    for (int i = 0; i < strlen(buff); i++)
    {
        if (buff[i] == ' ')
            flag++; //由空格分割
        if (buff[i] >= '0' && buff[i] <= '9')
        {
            temp = buff[i] - 48;
            if (flag == 0)
                setting.times = setting.times * 10 + temp;
            else if (flag == 1)
                setting.datasize = setting.datasize * 10 + temp;
            else if (flag == 2)
                setting.buffsize = setting.buffsize * 10 + temp;
            else if (flag == 3)
                setting.recvspeed = setting.recvspeed * 10 + temp;
        }
    }
    if (setting.datasize > MAXLINE)
    {
        std::cout << "package is too big!!" << std::endl;
        exit(0);
    }
    printf("setting:\n  times: %d\n  datasize: %d\n  buffsize: %d\n  recvspeed: %d.\n", setting.times, setting.datasize, setting.buffsize, setting.recvspeed);
    return setting;
}

int SendAndRecv(int sock_conn, char *server_ip, char *sendbuff, char *recvbuff, set setting)
{
    int result;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    socklen_t add_len;
    //发送设置的数据
    result = sendto(sock_conn, sendbuff, strlen(sendbuff), 0, (sockaddr *)&server_addr, sizeof(sockaddr));
    if (result <= 0)
    {
        //错误处理
        perror("error");
        printf("Send Setting Error.\n");
        return -1;
    }
    //设置发送数据报内容
    memset(sendbuff, 0, MAXLINE);
    for (int i = 0; i < setting.datasize; i++)
        sendbuff[i] = 'A';
    sendbuff[setting.datasize] = '\0';

    for (int i = 0; i < setting.times; i++) //循环发送times个数据报
    {
        result = sendto(sock_conn, sendbuff, setting.datasize, 0, (sockaddr *)&server_addr, sizeof(sockaddr));
        if (result <= 0)
        {
            //发送失败时重新发送
            printf("Send data error. And resend.\n");
            i--;
        }
    }
    printf("Send %d data successfully.\n", setting.times);
    return 1;
}

int main()
{
    int udp_client, result;
    char recvbuff[MAXLINE];
    char sendbuff[MAXLINE];
    char server_ip[] = "127.0.0.1";

    //输入设置参数
    printf("Input times, datasize, buffsize, recvspeed (separated by space):\n");
    std::cin.getline(sendbuff, MAXLINE);
    set setting = buff2set(sendbuff);   //将缓冲区内数据放入设置的数据结构中

    udp_client = udpClientInit();
    result = SendAndRecv(udp_client, server_ip, sendbuff, recvbuff, setting);
    sleep(5);
    close(udp_client);
    return 0;
}