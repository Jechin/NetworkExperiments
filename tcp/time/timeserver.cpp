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

#include <sys/time.h>
#include <utime.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>

#define MAXLINE 4096
#define LINSTEN 1024
#define TIMEPORT 10000

int main()
{
    int sock_listen, sock_conn;
    struct sockaddr_in servaddr;
    char buff[MAXLINE+1];
    time_t ticks;

    if((sock_listen = socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        fprintf(stderr, "Listen Socket Error is %s\n", strerror(errno));
        exit(-1);
    }

    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(TIMEPORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /*绑定主机所有的地址*/

    if (bind(sock_listen, (struct sockaddr*)(&servaddr), sizeof(struct sockaddr)) < 0)
    {
        perror("error");
        fprintf(stderr, "bind failed \n");
        exit(-1);
    }

    listen(sock_listen, LINSTEN);

    for( ; ; )
    {
        memset(buff, 0 , sizeof(buff));
        sock_conn = accept(sock_listen, (struct sockaddr*) NULL, NULL) ;
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks) );
        printf("%.24s\r\n", ctime(&ticks) );
        write(sock_conn, buff, strlen(buff) );
        close(sock_conn);
    }

    return 0;
}