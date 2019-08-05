#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXLINE 4096

int main(int argc, char* argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket error");
        exit(1);
    }

    struct hostent* server = gethostbyname(argv[1]);
    if (NULL == server) {
        printf("gethostbyname error");
        exit(1);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    bcopy((char*)server->h_addr, (char*)&servaddr.sin_addr.s_addr, server->h_length);

    int ret = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("connect error");
        exit(1);
    }

<<<<<<< HEAD
    int size = 0;
    while (1) {
        char buffer[MAXLINE] = { 0 };
        size = read(sockfd, buffer, MAXLINE);
        if (size > 0) {
            printf(buffer);
        }
        else {
            break;
=======
    while (1) {
        char buffer[MAXLINE] = { 0 };
        int size = read(sockfd, buffer, MAXLINE);
        if (size > 0) {
            printf(buffer);
            printf("\n");
        }
        else {
            // printf("read error\n");
            // break;
            sleep(1);
>>>>>>> abf10d3896ee921d257ceefbc679da5cbaa0c27e
        }
    }

    return 0;
}
