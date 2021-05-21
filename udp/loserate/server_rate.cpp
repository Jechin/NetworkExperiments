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
#include <sstream>
#include <iostream>

#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>

#define MAXLINE 4096
#define PORT 9999

typedef struct set
{
    int times;
    int datasize;
    int buffsize;
    int recvspeed;
} set;

/*tcp服务器端初始化*/
int udpServerInit(u_short port)
{
    int result;
    int sock_conn;
    const int ON = 1;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_conn = socket(AF_INET, SOCK_DGRAM, 0); //创建套接字
    if (sock_conn < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }

    result = setsockopt(sock_conn, SOL_SOCKET, SO_REUSEADDR, (char *)&ON, sizeof(ON));
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "set sock option failed \n");
        exit(-1);
    }

    result = bind(sock_conn, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)); //绑定端口地址
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "bind failed \n");
        exit(-1);
    }

    printf("Bind Sucessfully. Server Starts!\n");
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

int RecvAndCal(int sock_conn, char *recvbuff, char *sendbuff)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int result = 0;
    struct timeval timeout1 = {100, 0}; //设置超时
    result = setsockopt(sock_conn, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout1, sizeof(struct timeval));
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "set sock option failed \n");
        exit(-1);
    }

    //接受设置的数据
    result = recvfrom(sock_conn, recvbuff, MAXLINE - 6, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "Recv setting error.\n");
        exit(-1);
    }
    set setting = buff2set(recvbuff); //将缓冲区数据放入设置的数据结构中
    int nRecvBuf = (setting.buffsize+1)*8;
    result = setsockopt(sock_conn, SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBuf, sizeof(int));
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "set sock option failed \n");
        exit(-1);
    }
    struct timeval timeout2 = {0, 100000}; //设置超时
    //int timeout = 500;
    result = setsockopt(sock_conn, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout2, sizeof(struct timeval));
    if (result < 0)
    {
        //错误处理
        perror("error");
        fprintf(stderr, "set sock option failed \n");
        exit(-1);
    }
    int recv_num = 0; //接收到的数据报个数
    for (int i = 0; i < setting.times; i++)
    {
        usleep(setting.recvspeed * 1000);
        memset(recvbuff, 0, MAXLINE);
        result = recvfrom(sock_conn, recvbuff, MAXLINE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (result == setting.datasize) //接受成功
            recv_num++;                 //接收到的数据报个数加一
        else if (result > 0)            //接收到不完整的数据报
            printf("Recv Uncomplete data.\n");
        //else
            //printf("Recv Error.\n");
    }

    double rate = (1 - (double)recv_num / setting.times) * 100;
    printf("Lost Rate: %.2lf%%\n", rate);
    return 1;
}

int main()
{
    int udp_server, result;
    char recvbuff[MAXLINE];
    char sendbuff[MAXLINE];

    udp_server = udpServerInit(PORT);

    while (true)
    {
        result = RecvAndCal(udp_server, recvbuff, sendbuff);
        if (result < 0)
        {
            printf("UDP Client Error.\n");
        }
    }

    close(udp_server);
    return 0;
}