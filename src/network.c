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
//1024*1024*5
#define MAX_MSG_SIZE 5242880

#define MAX_PEERS_FILE_LENGTH 1000

//struct net_info //?
//int time //median
//**mempool

int socks[10];
unint socks_count = 0;


void join_network()
{
    printf("Attempting to join network...\n");

    // Storing active peers which should be saved
    peer_info *peers = malloc(MAX_PEERS_FILE_LENGTH);
    unint peers_count = 0;

    // Buffers
    char addr[100];
    unint port;
    unint last_active;

    // FROM FILE
    /*
        All peers specified in the file are loaded into memory (up to 1000).
        We try to connect to peers until we have 8. Any failed peers are
        removed from memory. The peerfile is then overwritten with peers in
        memory.
    */
    FILE *addrfile = fopen(FILE_PEERS, "r");

    // Load entries into memory
    while ((fscanf(addrfile, "%s %d %d", addr, &port, &last_active) == 2) && peers_count < MAX_PEERS_FILE_LENGTH)
    {
        peer_info peer;
        peer.addr = addr;
        peer.port = port;
        peer.last_active = last_active;
        peers[peers_count++] = peer;
        addr[0] = '\0';
    }
    // Connect to up to 8 peers
    for (int i=0; i<peers_count; i++)
    {
        int peerfd = start_client_conn(peers[i].addr, peers[i].port);
        if (peerfd < 0) // Failed
        {
            peers[i] = '\0'; // Remove it
        }
        else
        {
            // Add to socks array
            socks[socks_count++] = peerfd;
            // Check for 8
            if (socks_count >= 8)
                break;
        }
    }
    // FROM INPUT
    if (socks_count == 0)
    {
        printf("No peers found in peerfile.\nEnter manually: \n");
        while(1)
        {
            scanf("%s %d", addr, &port);
            int peerfd = start_client_conn(addr, port));
            if (peerfd < 0)
            {
                addr[0] = '\0'; // Remove it
            else
            {
                // Add to socks array
                socks[socks_count++] = peerfd;
                // Add entry to memory
                peer_info peer;
                peer.addr = addr;
                peer.port = port;
                peer.last_active = get_system_time();
                peers[peers_count++] = peer;
                break;
            }
        } 
    }

    // Initiate client-server handshakes
    printf("Initiating handshakes...\n");
    for(int i=0; i<socks_count; i++)
    {
        // Params 2 and 3 are because the handshake adds addrs
        handshake(socks[i], peers, peers_count); 
    }

    // Overwrite valid peers to file
    ftruncate(addrfile, 0);
    rewind(addrfile);
    for(int i; i<peer_count; i++)
        if(peers[i] != '\0')
            fprintf(addrfile, "%s %d", peers[i].addr, peers[i].port);
    fclose(addrfile);
    free(peers);
    
    
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

////////// Base network functions //////////

///// Initiation /////

// Threaded (any amount)
void *handle_connection(void *params)
{
    // Dereferenced stack variables
    int connfd = ((conn_thread_params *)params)->connfd;

    int n;
    unchar *msg = malloc(MAX_MSG_SIZE);
    for(;;)
    {
        n = recvfrom(connfd, msg, MAX_MSG_SIZE, 0, NULL, 0);
        if (n == 0)
            die("Peer has shutdown"); // TODO we shouldn't die because of this
        else if (n < 0)
            die("Error recieving from peer. ERRNO %d\n",errno);
        //msg[n] = 0; not a string, TODO
        
        // Parsing
        switch (msg[0])
        {
            case METHOD_REJECT:
                printf("Recieved METHOD_REJECT: %d",msg[1]); //TODO parse message
                break;
            case METHOD_VERSION:
                unshort vers = msg[1];
                if (vers == __VERSION)
                {
                    sendto_peer(connfd, METHOD_VERACK, 0, 0);
                    sendto_peer(connfd, METHOD_ADDR,
                }
                else
                    sendto_peer(connfd, METHOD_REJECT, BAD_VERSION, 1);
            case METHOD_VERACK:
                //TODO store that connfd has accepted our version
        }
                
        
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
    else
        printf("Connected to server %s:%d\n", addr, port);

    // Split to own thread
    pthread_t conn_thread;

    // Argument struct
    conn_thread_params params;
    params.connfd = connfd;

    pthread_create(&conn_thread, NULL, handle_connection, &params);
    
    return connfd;
}

///// Misc /////
void sendto_peer(int connfd, char method, char *msg, unint msg_length)
{
    // Create raw msg by appending method byte
    unchar *raw_msg = malloc(msg_length + 1)
    raw_msg[0] = method;
    memcpy(&raw_msg[1], msg);
    if ( sendto(connfd, *raw_msg, (msg_length + 1), 0, NULL, 0) < 0)
        die("Send to peer failed. ERRNO %d\n", errno); //TODO don't die because of this
    free(raw_msg);
}

////////// Network action functions //////////

void handshake(int connfd, peer_info peers, unint peers_count)
{
    sendto_peer(connfd, METHOD_VERSION, METHOD_VERSION_LENGTH);
};

//////////
// MISC //
//////////

unint get_net_time()
{
    
};

