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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <string>

int main(int argc, char* argv[])
{
    int sock, n;
    char buffer[2048] = { 0 };

    struct ethhdr *eth;
    struct iphdr *iph;

    if (0>(sock=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP)))) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_ll client;
    socklen_t addr_length = sizeof(sockaddr_ll);

    while (1) {
        // printf("=====================================\n");
        n = recvfrom(sock, buffer, 2048, 0, (sockaddr *)&client, &addr_length);
        // printf("%d bytes read\n",n);

        eth=(struct ethhdr*)buffer;

        // printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",
        //         eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
        // printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",
        //         eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);

        iph=(struct iphdr*)(buffer+sizeof(struct ethhdr));
        // if(iph->version ==4 && iph->ihl == 5){
        if(iph->version ==4) {
            if (IPPROTO_UDP == iph->protocol) {
                in_addr src;
                src.s_addr= iph->saddr;

                in_addr des;
                des.s_addr= iph->daddr;

                if (std::string("192.168.28.66") == std::string(inet_ntoa(src))) {

                    printf("%d bytes read\n",n);

                    printf("client.sll_family=%d\n", client.sll_family);
                    printf("client.sll_protocol=0x%04X\n", ntohs(client.sll_protocol));
                    printf("client.sll_ifindex=%d\n", client.sll_ifindex);
                    printf("client.sll_hatype=%d\n", client.sll_hatype);
                    printf("client.sll_pkttype=0x%02X\n", client.sll_pkttype);
                    printf("client.sll_halen=0x%02X\n", client.sll_halen);
                    printf("client.sll_addr=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n", 
                            client.sll_addr[0],
                            client.sll_addr[1],
                            client.sll_addr[2],
                            client.sll_addr[3],
                            client.sll_addr[4],
                            client.sll_addr[5],
                            client.sll_addr[6],
                            client.sll_addr[7]);

                    unsigned short sll_hatype;
                    unsigned char sll_pkttype;
                    unsigned char sll_halen;
                    unsigned char sll_addr[8];


                    printf("Dest MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",
                            eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
                    printf("Source MAC addr:%02x:%02x:%02x:%02x:%02x:%02x\n",
                            eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);

                    printf("Source host:%s\n", inet_ntoa(src));
                    printf("Dest host:%s\n", inet_ntoa(des));
                }
            }
        }
    }

    return 0;
}
