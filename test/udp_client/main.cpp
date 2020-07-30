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

int main(int argc, char* argv[])
{
    int s;
    unsigned short port;
    struct sockaddr_in server;
    char buf[32];

    /* argv[1] is internet address of server argv[2] is port of server.
     * Convert the port from ascii to integer and then from host byte
     * order to network byte order.
     */
    if(argc != 3) {
        printf("Usage: %s <host address> <port> \n",argv[0]);
        exit(1);
    }

    port = htons(atoi(argv[2]));

    /* Create a datagram socket in the internet domain and use the
     * default protocol (UDP).
     */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        exit(1);
    }

    int optval = 1;
    int ret = setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    if (ret < 0) {
        printf("setsockopt error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        exit(1);
    }

    ret = setsockopt(s, SOL_SOCKET, SO_TIMESTAMP, &optval, sizeof(optval));;
    if (ret < 0) {
        printf("setsockopt error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        exit(1);
    }

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // servaddr.sin_port = htons(9999);
    servaddr.sin_port = htons(20000);
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr("192.168.2.99");

    ret = bind(s, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("bind error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        return 1;
    }

    {
        uint8_t ip_options[40] = { 0 };
        // IP Options - Timestamp 
        int ip_opt_len = 0;
        ip_options[0] = 68u; ip_opt_len++;  // Timestamp option
        ip_options[1] = 8u; ip_opt_len++;  // Option length (bytes) - 40 is maximum
        ip_options[2] = 9u; ip_opt_len++;  // Pointer
        ip_options[3] = 0u; ip_opt_len++;  // Overflow and Flag
        ip_options[4] = 0u; ip_opt_len++;  // Time in milliseconds (right-justified) - random here
        ip_options[5] = 0u; ip_opt_len++;  // Time cont'd
        ip_options[6] = 67u; ip_opt_len++;  // Time cont'd
        ip_options[7] = 78u; ip_opt_len++;  // Time cont'd

        // Pad to the next 4-byte boundary.
        while ((ip_opt_len%4) != 0) {
            ip_options[ip_opt_len] = 0;
            ip_opt_len++;
        }

        ret = setsockopt(s, IPPROTO_IP, IP_OPTIONS, ip_options, ip_opt_len);
        if(ret < 0) {
            printf("fail to setsockopt IP_OPTIONS errno:%d errmsg:%s\n", errno, strerror(errno));
            exit(1);
        }
    }

    /* Set up the server name */
    server.sin_family      = AF_INET;            /* Internet Domain    */
    server.sin_port        = port;               /* Server Port        */
    server.sin_addr.s_addr = inet_addr(argv[1]); /* Server's Address   */

    strcpy(buf, "Hello");

    sleep(1);

    /* Send the message in buf to the server */
    if (sendto(s, buf, (strlen(buf)+1), 0,
                (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("sendto error\n");
        printf("errno:%d, %s\n", errno, strerror(errno));
        exit(2);
    }

    /* Deallocate the socket */
    close(s);

    return 0;
}
