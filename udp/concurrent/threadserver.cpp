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
#include <fcntl.h>
#include <pthread.h>
#include <iostream>

#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>

#define MAXLINE 4096
#define PORT 9999

typedef struct thread_argv
{
    int port;
    char buff[MAXLINE];
    struct sockaddr_in sock_address;
} thread_argv;

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

int RecvAndEcho(int sock_conn, char *recvbuff, char *sendbuff, sockaddr_in *sock_address)
{
    struct sockaddr_in client_addr = *(sock_address);
    socklen_t addr_len = sizeof(client_addr);
    char echo[6] = {'E', 'C', 'H', 'O', ':', ' '};
    int flag = 0;

    int result = 1;
    do
    {
        if (flag)
            result = recvfrom(sock_conn, recvbuff, MAXLINE - 6, 0, (struct sockaddr *)&client_addr, &addr_len);
        flag = 1;
        if (result <= 0)
        {
            //错误处理
            perror("error");
            fprintf(stderr, "Recv failed \n");
            result = -1;
        }
        else
        {
            printf("Receive From %s\n", inet_ntoa(client_addr.sin_addr));
            std::cout << "Recv data from client: " << recvbuff << std::endl;
            strncpy(sendbuff, echo, 6);
            strncpy(sendbuff + 6, recvbuff, strlen(recvbuff));
            result = sendto(sock_conn, sendbuff, strlen(sendbuff), 0, (sockaddr *)&client_addr, addr_len);
            if (result > 0)
            {
                std::cout << "Send data to client: " << sendbuff << std::endl;
            }
            else
            {
                //错误处理
                perror("error");
                fprintf(stderr, "Send failed \n");
                result = -1;
            }
        }
        memset(sendbuff, 0, MAXLINE);
        memset(recvbuff, 0, MAXLINE);
    } while (result > 0);
    return result;
}

void *SubThread(void *argv)
{
    thread_argv temp = *((thread_argv *)argv);
    int udp_sock, port;
    char recvbuff[MAXLINE];
    char sendbuff[MAXLINE];
    struct sockaddr_in udp_address;
    memset(recvbuff, 0, MAXLINE);
    memset(sendbuff, 0, MAXLINE);

    port = temp.port;
    strncpy(recvbuff, temp.buff, strlen(temp.buff));
    udp_address = temp.sock_address;
    udp_sock = udpServerInit(port);
    printf("Port: %d\n", port);
    int result = RecvAndEcho(udp_sock, recvbuff, sendbuff, &udp_address);
    if (result <= 0)
        printf("Udp Client Error.\n");

    close(udp_sock);
    return 0;
}

int main()
{
    pthread_t id;
    int udp_server, result, pid, port;
    char recvbuff[MAXLINE];
    char sendbuff[MAXLINE];

    udp_server = udpServerInit(PORT);

    struct sockaddr_in udp_client;
    memset(&udp_client, 0, sizeof(udp_client));
    socklen_t len;

    thread_argv temp;
    port = 20000;

    while (true)
    {
        memset(recvbuff, 0, MAXLINE);
        result = recvfrom(udp_server, recvbuff, MAXLINE, 0, (struct sockaddr *)&udp_client, &len);
        if (result < 0)
        {
            printf("UDP Client Error.\n");
            continue;
        }
        memset(temp.buff, 0, MAXLINE);
        strncpy(temp.buff, recvbuff, strlen(recvbuff));
        temp.sock_address = udp_client;
        temp.port = port++;

        result = pthread_create(&id, NULL, SubThread, (void *)&temp);

        if (result < 0)
        {
            printf("Pthread Create Error.\n");
        }
    }

    close(udp_server);
    return 0;
}