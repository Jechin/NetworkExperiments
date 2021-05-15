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
#include <pthread.h>
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


int RecvAndSend(int sock_conn, char * buff, int maxlength)
{
    int n, nn;
    while((n = recv(sock_conn, buff, maxlength, 0))>0)
    {
        buff[n]=0;
        std::cout<<buff<<std::endl;
        if((nn = send(sock_conn, buff, strlen(buff), 0))>0)
            printf("Echo Successfully!\n\n");
    }

    if (n == 0)
        printf("Receive Over\n"); //当读取字符为0时 表示读取完毕
    if (n < 0)
    {
        perror("error");
        printf("Receive Error\n");
        return -1;
    }       

    return 0;
}

void * SubThread(void *arg)
{
    char buff[MAXLINE + 1];
    char sendbuff[MAXLINE + 1];

    int result;
    int sock_conn = *((int *) arg);

    result = RecvAndSend(sock_conn, buff, MAXLINE);

    printf("Closed a Connection.\n\n");

    return 0;
}

int main()
{
    int sock_server;
    char recv_buff[MAXLINE + 1];
    pthread_t id;
    int ret;

    sock_server = tcpServerInit(CLIENT_PORT);

    listen(sock_server, LISTENQ);

    for( ; ; )
    {
        memset(recv_buff, 0, sizeof(recv_buff));
        int sock_conn = accept(sock_server, (struct sockaddr*) NULL, NULL) ;
        printf("Accept a Connection!\n");
        
        ret = pthread_create(&id, NULL, SubThread, (void *)&sock_conn);
        if(ret != 0)
        {
            printf("Create Thread Failed!\n");
            return 1;
        }
        
        
        //close(sock_conn);
        
    }

    close(sock_server);
    return 0;
}