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

#define MAXLINE 4096     
#define LISTENQ 1024     
#define SEVER_PORT 13131
#define TIMEPORT 10000

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //创建套接字
    {
        perror("error");
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(-1);
    }

    //完成servaddr结构体
    char server_ip[] = "127.0.0.1";
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TIMEPORT);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) //向服务器发送连接请求
    {
        perror("error");
        fprintf(stderr, "Connect failed\n");
        exit(-1);
    }

    printf("Time is: ");
    while ((n = recv(sockfd, recvline, MAXLINE, 0)) > 0) //循环接受数据
    {
        recvline[n] = 0; /* null termicate */
        if (fputs(recvline, stdout) == EOF)
            printf("fputs error");
    }
    if (n == 0)
        printf("Read Over"); //当读取字符为0时 表示读取完毕
    if (n < 0)
        printf("read error ");

    return 0;
}