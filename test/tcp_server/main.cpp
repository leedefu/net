#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <netinet/tcp.h>
#include <signal.h>

#define CLIENT_MAX_NUM 100

int client_socketIds[CLIENT_MAX_NUM];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    memset(client_socketIds, 0, sizeof(client_socketIds));
    // printf("sizeof(client_socketIds):%d\n", sizeof(client_socketIds));

    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    /*************************************************************************/
    /* Create an AF_INET stream socket to receive incoming connections       */
    /*************************************************************************/
    int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (master_socket < 0) {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    char buffer[1025];  //data buffer of 1K

    /*************************************************************
      set master socket to allow multiple connections ,
      this is just a good habit, it will work without this.
      Allow socket descriptor to be reuseable
    *************************************************************/
    int opt = 1;
    int result = setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    if(result < 0) {
        perror("setsockopt failed");
        close(master_socket);
        exit(EXIT_FAILURE);
    }

    /*************************************************************/
    /* Bind the socket                                           */
    /* bind the socket to localhost port 7000                    */
    /*************************************************************/
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(::atoi(argv[1]));
    result = bind(master_socket, (struct sockaddr *)&address, sizeof(address));
    if (result < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // try to specify maximum of 32 pending connections for the master socket
    printf("Listener on port %d \n", ::atoi(argv[1]));
    result = listen(master_socket, 32);
    if (result < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    puts("Waiting for connections ...");

    //accept the incoming connection
    int addrlen = sizeof(address);

    // set of socket descriptors
    fd_set readfds;

    int max_sd;

    // run loop
    while(1) {

        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(master_socket, &readfds);

        max_sd = master_socket;

        // add child sockets to set
        for (int i = 0 ; i < CLIENT_MAX_NUM; ++i) {
            // socket descriptor
            int sd = client_socketIds[i];

            // if valid socket descriptor then add to read list
            if(sd > 0) {
                FD_SET(sd, &readfds);
            }

            // highest file descriptor number, need it for the select function
            if(sd > max_sd) {
                max_sd = sd;
            }
        }

        // wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
        printf("waiting on select\n");
        int activity = select((max_sd + 1), &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {

            printf("FD_ISSET master socket, new socket connected\n");

            int new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            // int new_socket = accept(master_socket, NULL, NULL);
            if (new_socket < 0) {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            int opt = 1;
            int result = setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt));
            if(result < 0) {
                perror("setsockopt failed");
                close(new_socket);
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection, socketfd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            // send new connection greeting message
            const char* welcomeMessage = "welcome";
            ssize_t res = send(new_socket, welcomeMessage, strlen(welcomeMessage), 0);
            if(res != strlen(welcomeMessage)) {
                perror("send failed");
            }

            puts("Welcome message sent successfully");

            // TODO: read message from client and check whether webclient or ppapi client
            // if ppapi client, add socket to list
            // if web client, read message only, message contains data table information?
            // add new socket to array of sockets
            for (int i = 0; i < CLIENT_MAX_NUM; ++i) {
                // if position is empty
                if(client_socketIds[i] == 0) {
                    client_socketIds[i] = new_socket;
                    printf("Adding %d to list of sockets index:%d\n", new_socket, i);
                    break;
                }
            }
        }
        else { // else its some IO operation on some other socket

            printf("FD_ISSET other socket, IO operation on socket\n");

            for (int i = 0; i < CLIENT_MAX_NUM; ++i) {

                int sd = client_socketIds[i];

                if (FD_ISSET(sd , &readfds)) {
                    // Check if it was for closing, and also read the incoming message
                    int valread = read(sd, buffer, 1024);
                    if (valread == 0) {
                        // Somebody disconnected, get his details and print
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        printf("Host disconnected, ip: %s, port:%d sd:%d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), sd);
                        // Close the socket and mark as 0 in list for reuse
                        // TODO
                        sleep(5);
                        close(sd);
                        client_socketIds[i] = 0;
                    }
                    else { // Echo back the message that came in
                        //set the string terminating NULL byte on the end of the data read
                        buffer[valread] = '\0';
                        printf("read message:[%s] from sd[%d]\n", buffer, sd);

                        int opt = 1;
                        setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt));
                        // int n = write(sd, buffer, strlen(buffer));
                        // int n = send(sd, buffer, strlen(buffer), 0);
                        // if (n < 0) {
                        //     perror("ERROR writing to socket");
                        // }
                    }
                }
            }
        }
    }

    return 0;
}
