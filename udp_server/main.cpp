#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
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

#define DEFAULT_BUFF_SIZE 1536

static void printHex(unsigned char* buff, int len)
{
    for (int i = 0; i < len; ++i) {
        printf("%02x", buff[i]);

    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    {
        int arr[20] = { 0 };
        for (int i = 0; i < 10; i++) {
            arr[i] = i++;
        }
        for (int i = 0; i < 20; i++) {
            printf("arr[%d]=%d\n", i, arr[i]);
        }
    }


    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    int masterSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (masterSocket < 0) {
        printf("socket error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        return 1;
    }
    printf("masterSocket :%d\n", masterSocket);

    int optval = 1;
    int ret = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret < 0) {
        printf("setsockopt error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        return 1;
    }

#if 0
    ret = setsockopt(masterSocket, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    if (ret < 0) {
        printf("setsockopt error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        return 1;
    }
#endif

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(::atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // servaddr.sin_addr.s_addr = inet_addr("192.168.28.191"); 
    
    #if 0
    struct ip_mreq join;
    join.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    join.imr_interface.s_addr = htonl(INADDR_ANY);
    // join.imr_interface.s_addr = inet_addr("192.168.28.191");

    int on = 1;
    if (setsockopt(masterSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join, sizeof(join)) < 0) {
        printf("fail to setsockopt IP_ADD_MEMBERSHIP. (errno:%d, errmsg:%s)\n", errno, strerror(errno));
    }

    if (setsockopt(masterSocket, IPPROTO_IP, IP_MULTICAST_LOOP, &on, sizeof(on)) < 0) {
        printf("fail to setsockopt IP_MULTICAST_LOOP. (errno:%d, errmsg:%s)\n", errno, strerror(errno));
    }
    #endif

    ret = bind(masterSocket, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("bind error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        return 1;
    }

    // set of socket descriptors
    fd_set readfds;

    // loop
    while(1) {

#if 0
        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(masterSocket, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 50000;

        int activity = select((masterSocket + 1), &readfds, NULL, NULL, &tv);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
            printf("errno:%d, %s\n", errno, strerror(errno));
        }

        if (!FD_ISSET(masterSocket, &readfds)) {
            continue;
        }
#endif

        unsigned char* buff = new unsigned char[DEFAULT_BUFF_SIZE];
        memset(buff, 0, DEFAULT_BUFF_SIZE);

        struct sockaddr_in src_addr;
        socklen_t src_addr_len = sizeof(src_addr);

        int recvSize = recvfrom(masterSocket, (void*)buff, DEFAULT_BUFF_SIZE, 0, (struct sockaddr*)&src_addr, &src_addr_len);
        printf("content:");
        printHex(buff, recvSize);

        char string[42];
		inet_ntop(AF_INET, &src_addr.sin_addr.s_addr, string, 42);
		printf("recvfrom sockId:%d addr:%s port:%d %d bytes\n", masterSocket, string, ntohs(src_addr.sin_port), recvSize);

        printf("recv content:%s\n", buff);

        delete[] buff;
        buff = NULL;

        sleep(1);
    }

    return 0;
}
