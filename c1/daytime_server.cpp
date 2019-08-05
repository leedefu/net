#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>

#define MAXLINE 4096

int main(int argc, char* argv[])
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("socket error");
        exit(1);
    }

    int opt = 1;
    // if not set SO_REUSEADDR
    // errno 98 Address already in use
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if(ret < 0) {
        printf("setsockopt failed");
        close(listenfd);
        exit(1);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);

    ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("bind error");
        printf("errno:%d, %s\n", errno, strerror(errno));
        exit(1);
    }

    ret = listen(listenfd, 1024);
    if (ret < 0) {
        printf("listen error");
        exit(1);
    }

    while (1) {
        int connectfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        time_t ticks = time(NULL);
        char buffer[MAXLINE] = { 0 };
        snprintf(buffer, (sizeof(buffer) - 1), "%.24s\r\n", ctime(&ticks));
        write(connectfd, buffer, strlen(buffer));
        close(connectfd);
    }

    close(listenfd);
    return 0;
}
