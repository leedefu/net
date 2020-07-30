#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
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
#include <fcntl.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

#if 0
enum {
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT,
    TCP_SYN_RECV,
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_TIME_WAIT,
    TCP_CLOSE,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_LISTEN,
    TCP_CLOSING
};
#endif

static int getTcpState(uint32_t client_addr, uint32_t client_port, 
    uint32_t server_addr, uint32_t server_port)
{
    int retState = -1;

    char server_address[128] = { 0 };
    char client_address[128] = { 0 };
    snprintf(server_address, 127, "%08X", server_addr);
    snprintf(client_address, 127, "%08X", client_addr);
    printf("server_address=%s\n", server_address);
    printf("client_address=%s\n", client_address);

    /* read /proc/net/tcp */
    FILE* fp = fopen("/proc/net/tcp", "r");
    if (NULL != fp) {
        char line[200] = { 0 };
        /* Bypass header -- read until newline */
        if (fgets(line, sizeof(line), fp) != NULL) {

            /* Read the entries. */
            for (; fgets(line, sizeof(line), fp);) {
                // printf("entry: %s", line);

                unsigned long rxq, txq, time_len, retr, inode;
                int num, local_port, rem_port, d, state, uid, timer_run, timeout;
                char rem_addr[128], local_addr[128], timers[64], buffer[1024], more[512];

                num = sscanf(line,
                        "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %512s\n",
                        &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                        &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode, more);
                // printf("local_addr=%s\n", local_addr);
                // printf("local_port=%04X\n", local_port);
                // printf("rem_addr=%s\n", rem_addr);
                // printf("rem_port=%04X\n", rem_port);

                if(local_port != client_port) {
                    continue;
                }
                if(server_port != rem_port) {
                    continue;
                }
                if (strlen(local_addr) != strlen(client_address)) {
                    continue;
                }
                if (strcmp(local_addr, client_address) != 0) {
                    continue;
                }
                if (strlen(rem_addr) != strlen(server_address)) {
                    continue;
                }
                if (strcmp(rem_addr, server_address) != 0) {
                    continue;
                }

                retState = state;
                break;

            }

        }
        else {
            printf("/proc/net/tcp has no header\n");
        }

        fclose(fp);
    }
    else {
        printf("open /proc/net/tcp failed\n");
    }

    switch (retState) {
    case TCP_ESTABLISHED:
        printf("TCP_ESTABLISHED\n");
    break;
    case TCP_SYN_SENT:
        printf("TCP_SYN_SENT\n");
    break;
    case TCP_SYN_RECV:
        printf("TCP_SYN_RECV\n");
    break;
    case TCP_FIN_WAIT1:
        printf("TCP_FIN_WAIT1\n");
    break;
    case TCP_FIN_WAIT2:
        printf("TCP_FIN_WAIT2\n");
    break;
    case TCP_TIME_WAIT:
        printf("TCP_TIME_WAIT\n");
    break;
    case TCP_CLOSE:
        printf("TCP_CLOSE\n");
    break;
    case TCP_CLOSE_WAIT:
        printf("TCP_CLOSE_WAIT\n");
    break;
    case TCP_LAST_ACK:
        printf("TCP_LAST_ACK\n");
    break;
    case TCP_LISTEN:
        printf("TCP_LISTEN\n");
    break;
    case TCP_CLOSING:
        printf("TCP_CLOSING\n");
    break;
    default:
        printf("invalid tcp state\n");
    break;
    }

    return retState;

}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // check parameters
    char buffer[256];
    if (argc < 3) {
        fprintf(stderr,"usage: %s <ip(hostname)> <port>\n", argv[0]);
        exit(0);
    }

    while (1) {
    // port
    portno = atoi(argv[2]);

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    // get server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // TODO Non-Block!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // fcntl(sockfd, F_SETFL,O_NONBLOCK);

    // bind local address and port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(7788);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    {
        char string[42] = { 0 };        
        printf("bind socket %d to: %s:%d\n", sockfd, string, 7788);
    }


    int optVal = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
        printf("fail to setsockopt SO_REUSEADDR . (errno:%d, errmsg:%s)\n", errno, strerror(errno));
    }
    
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optVal, sizeof(optVal)) < 0) {
    //     printf("fail to setsockopt SO_REUSEPORT . (errno:%d, errmsg:%s)\n", errno, strerror(errno));
    // }


    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("fail to bind. (errno:%d)\n", errno);
    }

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        // error("ERROR connecting");
        printf("first fail to connect errno:%d, msg:%s\n", errno, strerror(errno));
    }

#if 0
    // sleep(1);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("second fail to connect errno:%d, msg:%s\n", errno, strerror(errno));
    }
    else {
        printf("second connect ok!!!!!!!!!!!!!!!!!!!!!!\n");
    }
#endif

    struct sockaddr_in client_addr;
    bzero((char *) &client_addr, sizeof(client_addr));
    int i = sizeof(client_addr);
    if (getsockname(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&i) < 0){
        printf("getsockname error\n");
        exit(1);
    }

    char string[42];
    inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, string, 42);
    printf("connected on : %s:%d\n", string, ntohs(client_addr.sin_port));
    printf("connected on : %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    {
        printf("client addr:%08X, port:%04X\n", (client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port));
        printf("server addr:%08X, port:%04X\n", (serv_addr.sin_addr.s_addr), ntohs(serv_addr.sin_port));

        int tcpstate = getTcpState(client_addr.sin_addr.s_addr, ntohs(client_addr.sin_port),
                                    serv_addr.sin_addr.s_addr, ntohs(serv_addr.sin_port));
        printf("state=%02X\n", tcpstate);
    }

    printf("Please enter the message: ");

    bzero(buffer,256);
    fgets(buffer,255,stdin);

    // write
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)  {
        error("ERROR writing to socket");
    }

    bzero(buffer,256);

    // read
    n = read(sockfd,buffer,255);
    if (n < 0)  {
        error("ERROR reading from socket");
    }
    else {
        printf("%s\n",buffer);
    }

    printf("shutdown \n");
    // if (shutdown(sockfd, SHUT_RD) == -1) { established
    // if (shutdown(sockfd, SHUT_WR) == -1) { //  TCP_FIN_WAIT1, TCP_FIN_WAIT2,
    if (shutdown(sockfd, SHUT_RDWR) == -1) { //  TCP_FIN_WAIT1, TCP_CLOSE,
        printf("fail to shutdown socket(%d) (errno:%d, errmsg:%s)\n", sockfd, errno, strerror(errno));
    }

#if 0
    sleep(2);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("third fail to connect errno:%d, msg:%s\n", errno, strerror(errno));
    }
    else {
        printf("third connect ok!!!!!!!!!!!!!!!!!!!!!!\n");
    }
#endif

#if 0
    int times = 50*120;
    while(--times > 0) {
        struct tcp_info info;
        int len = sizeof(info);

        int ret = getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
        if (ret < 0) {
            printf("fail to getsockopt(%d) TCP_INFO errno:%d strerror:%s\n", sockfd, errno, strerror(errno));
        }

        printf("info.tcpi_state:%d, info.tcpi_ca_state:%d\n", info.tcpi_state, info.tcpi_ca_state);

        int tcpstate = getTcpState(client_addr.sin_addr.s_addr, ntohs(client_addr.sin_port),
                serv_addr.sin_addr.s_addr, ntohs(serv_addr.sin_port));
        printf("state=%02X\n", tcpstate);

        usleep(1000*20);
    }

    printf("sleep 3s\n");
    sleep(3);
#endif

#if 0
    printf("close\n");
    if (close(sockfd) == -1) {
        printf("fail to close socket(%d) (errno:%d, errmsg:%s)\n", sockfd, errno, strerror(errno));
    }

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("fourth fail to connect errno:%d, msg:%s\n", errno, strerror(errno));
    }
    else {
        printf("fourth connect ok!!!!!!!!!!!!!!!!!!!!!!\n");
    }
#endif

#if 0
    // force to close socket for skiping time_wait state
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
        printf("fail to setsockopt SO_LINGER. (errno:%d, errmsg:%s)\n", errno, strerror(errno));
    }
#endif

    if (close(sockfd) == -1) {
        printf("fail to close socket(%d) (errno:%d, errmsg:%s)\n", sockfd, errno, strerror(errno));
    }
    else {
        printf("success to close socket(%d)\n", sockfd);
    }

    sleep(10);

    }
    return 0;
}
