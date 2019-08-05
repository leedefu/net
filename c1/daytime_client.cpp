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
    while (1)  {
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

        int size = 0;
        while (1) {
            char buffer[MAXLINE] = { 0 };
            size = read(sockfd, buffer, MAXLINE);
            if (size > 0) {
                printf("%s", buffer);
            }
            else {
                break;
            }
        }

        if (0 == size) {
            // refer to README.md about server close connection
            printf("server has closed connection\n");
            printf("client retry to connect server\n");
        }

        if (size < 0) {
            printf("read error\n");
        }

        close(sockfd);
        sockfd = -1;

        /*All users should ensure that their software NEVER queries a server more frequently than once every 4 seconds. Systems that exceed this rate will be refused service. In extreme cases, systems that exceed this limit may be considered as attempting a denial-of-service attack.*/ 
        // sleep 5 seconds
        sleep(5);
    }

    return 0;
}
