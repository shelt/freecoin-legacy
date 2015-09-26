#include "shared.h"
#include "printing.h"
#include "network.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <arpa/inet.h>

#define FILE_PEERS "data/peers"
#define SERVER_PORT 30330

//struct net_info //?
//int time //median
//**mempool

unchar **socks;

FILE *addrfile;


void join_network()
{
    //TODO revamp this so while loop reads into array, which is used to connect to peers, and fails are removed from array, and array is written back to file (replacing old one).
    printf("Attempting to join network...\n");
    // Load from file
    printf("Loading peerlist...\n");
    char addr[100];
    unint port;
    unint peer_count = 0;
    addrfile = fopen(FILE_PEERS, "w+");
    while (fscanf(addrfile, "%s %d", addr, &port) == 2)
    {
        start_client_conn(addr, port);
        addr[0] = '\0';
        printf("Connected to %s:%d\n", addr, port);
        peer_count++;
    }
    if(peer_count == 0)
    {
        printf("No peers found in peerfile.\nEnter manually: \n");
        scanf("%s %d", addr, &port);
        start_client_conn(addr, port);
    }
    fclose(addrfile);

    // Connect to peer servers
    int client_peer = start_client_conn(addr, port);
    if (client_peer < 0)
    {
        // TODO peer is not alive, remove_from_peerfile() (but revamp as specified above)
    }
    else
    {
        // TODO add_addr_if_not_exist_to_peerfile() (but revamp as specified above)
    }
    
    // Create own server
    printf("Creating socket listener...\n");
    pthread_t server;
    unint port = SERVER_PORT;
    pthread_create(&server, NULL, server_listener, &port);
    sleep(5); // TODO We can do better than this!
    
}

///////////////////////
// NETWORK FUNCTIONS //
///////////////////////

// Threaded (any amount)
void *handle_connection(void *params)
{
    // Dereferenced stack variables
    int connfd                 = ((conn_thread_params *)params)->connfd;

    int n;
    char mesg[1000];
    for(;;)
    {
        n = recvfrom(connfd,mesg,1000, 0, NULL, 0);
        if (n == 0)
            die("Peer has shutdown"); // TODO we shouldn't die because of this
        else if (n < 0)
            die("Error recieving from peer. ERRNO %d\n",errno);
        //if ( sendto(connfd,mesg,n,0, NULL, 0) < 0)
        //    die("Respond to peer failed. ERRNO %d\n",errno);
        printf("-------------------------------------------------------\n");
        mesg[n] = 0;//TODO parsing message and responding
        printf("Received the following:\n");
        printf("%s",mesg);
        printf("-------------------------------------------------------\n");
    }
    close(connfd);
};

// Threaded (once)
void *server_listener(void *port)
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t clilen;
    //pid_t     childpid; TODO unused


    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
        die("Server socket creation failed. ERRNO %d\n",errno);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(*(unint *)port);
    if ( bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        die("Server failed to bind to address/port. ERRNO %d\n",errno);

    if ( listen(sockfd,1024) < 0)
        die("Server listen failed. ERRNO %d\n",errno);

    for(;;)
    {
        clilen=sizeof(cliaddr);
        int connfd = accept(sockfd,(struct sockaddr *)&cliaddr,&clilen);
        if (connfd < 0)
            die("Server failed to accept a connection. ERRNO %d\n",errno);

        pthread_t handler_thread;

        // Argument struct
        conn_thread_params params;
        params.connfd = connfd;

        pthread_create(&handler_thread, NULL, handle_connection, &params);

    }
}


int start_client_conn(char *addr, unint port)
{
    int connfd,n;
    struct sockaddr_in servaddr,ddcliaddr;
    char sendline[1000];
    char recvline[1000];

    connfd = socket(AF_INET,SOCK_STREAM,0);
    if (connfd < 0)
        die("Client socket creation failed. ERRNO %d\n", errno);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(addr);
    servaddr.sin_port=htons(port);

    if ( connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Connection to server failed: %s:%d. ERRNO %d\n", addr,port,errno);
        return -1;
    }

    pthread_t conn_thread;

    // Argument struct
    conn_thread_params params;
    params.connfd = connfd;

    pthread_create(&conn_thread, NULL, handle_connection, &params);
    
    return connfd;
}

//////////
// MISC //
//////////

unint get_net_time()
{
    
};

