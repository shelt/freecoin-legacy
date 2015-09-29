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

int initial_peer_fromfile(peer_info_t *peers)
{
    // Nodes from file
    peer_info_t *nodes_fromfile = malloc(sizeof(peer_info_t)*MAX_PEERS_FILE_LENGTH);
    nodes_fromfile_count = 0;

    // Buffers
    char addr[100];
    unint port;

    // FROM FILE
    //    All peers specified in the file are loaded into memory (up to 1000).
    //    We try to connect to an initial peer. Any failed peers are
    //    removed from memory. The peerfile is then overwritten with peers in
    //    memory.
    
    FILE *addrfile = fopen(FILE_PEERS, "r+");

    // Load entries into memory
    while ((fscanf(addrfile, "%s %d %d", addr, &port, &last_active) == 2) && nodes_fromfile_count < MAX_PEERS_FILE_LENGTH)
    {
        peer_info_t node;
        node.addr = addr;
        node.port = port;
        node.last_active = last_active;
        nodes_fromfile[nodes_fromfile_count++] = node;
        addr[0] = '\0';
    }
    int retval = 0;
    // Find initial peer
    for (int i=0; i<nodes_fromfile_count; i++)
    {
        int peerfd = start_client_conn(nodes_fromfile[i].addr, nodes_fromfile[i].port);
        if (peerfd < 0) // Failed
        {
            nodes_fromfile[i] = '\0'; // Set to 0 to remove it from file
        }
        else
        {
            peer_info_t peer;
            memcpy(peer, nodes_fromfile[i], sizeof(peer_info_t));
            peer.connfd = peerfd;
            peers[0] = peer;
            retval = 1;
            break;
        }
    }

    // Overwrite valid peers to file
    ftruncate(addrfile, 0);
    rewind(addrfile);
    for(int i; i<nodes_fromfile_count; i++)
        if(nodes_fromfile[i] != '\0')
            fprintf(addrfile, "%s %d", nodes_fromfile[i].addr, nodes_fromfile[i].port);
    fclose(addrfile);
    free(nodes_fromfile);

    return retval;
};

void join_network()
{
    printf("Attempting to join network...\n");

    // Peers AKA connected nodes
    peer_info_t *peers = malloc(sizeof(peers)*50);
    unint peers_count = 0;

    int success = initial_peer_fromfile(peers);
    if (success)
        peers_count++;
    else
    // FROM INPUT
    {
        // Buffers
        char addr[100];
        unint port;
        printf("Failed to connect to nodes from peerfile. \nEnter manually: \n");
        while(1)
        {
            scanf("%s %d", addr, &port);
            int peerfd = start_client_conn(addr, port));
            if (peerfd < 0)
            {
                addr[0] = '\0'; // Remove it
                printf("Failed to connect to node. \nEnter another: \n");
            else
            {
                peer_info_t peer; 
                peer.addr = addr;
                peer.port = port;
                peers[0] = peer;
                peers_count++;
                break;
            }
        } 
    }

    // Initiate client-server handshakes
    webify(peers, &peers_count);
    
    // Create own server
    printf("Creating socket listener...\n");
    pthread_t server;
    unint port = SERVER_PORT;
    pthread_create(&server, NULL, server_listener, &port);
    sleep(5); // TODO We can do better than this!
};

void webify(peer_info_t *peers, unint *peers_count)
{
    if (peers_count == 0)
        die("Webify: need initial peer");
    sendto_peer(peers[*peers_count-1], METHOD_GETPEER, peers, sizeof(peer_info_t)*peers_count);
};
//TODO mutex peers and peers_count

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
    int acting_server = ((conn_thread_params *)params)->acting_server;

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
                if (msg[1] == __VERSION)
                {
                    if (acting_server)
                    {
                        // Send verack  
                        sendto_peer(connfd, METHOD_VERACK, 0, 0);
                        // Send version
                        unchar body[METHOD_VERSION_SIZE];
                        body[0] = __version << 8 & 0xFF;
                        body[1] = __version      & 0xFF;
                        sendto_peer(connfd, METHOD_VERSION, body, METHOD_VERSION_SIZE);
                    }
                    //sendto_peer(connfd, METHOD_ADDR,  // Send addresses TODO revamp above
                }
                else
                    sendto_peer(connfd, METHOD_REJECT, &BAD_VERSION, 1);
                    close(connfd);
                break;
            case METHOD_VERACK: // TODO only send getblocks once per session?
                //sendto_peer(connfd, METHOD_ADDR,  // Send addresses TODO revamp above
                unchar *body = malloc(METHOD_GETBLOCKS_SIZE);
                // Start hash param
                get_latest_block(body);
                // Size param
                unshort count = MAX_GETBLOCKS_COUNT;
                body[SHA256_SIZE]   = count << 8 & 0xFF;
                body[SHA256_SIZE+1] = count      & 0xFF;
                sendto_peer(connfd, METHOD_GETBLOCKS, body, SHA256_SIZE);
                break;
            case METHOD_ADDR:
                //TODO revamp above
                break;
            case METHOD_GETBLOCKS:
                
                break;
            case METHOD_MEMPOOL:
                break;
            case METHOD_INV:
                break;
            case METHOD_GETDATA:
                break;
            case METHOD_BLOCK:
                break;
            case METHOD_TX:
                break;
            case METHOD_ALERT:
                break;
            case METHOD_PING:
                break;
            case METHOD_PONG:
                break;
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
        params.acting_server = 1;

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
    params.acting_server = 0;

    pthread_create(&conn_thread, NULL, handle_connection, &params);
    
    return connfd;
}

///// Misc /////
void sendto_peer(int connfd, char method, char *msg, unint msg_length)
{
    // Create raw msg by appending method byte
    unchar *raw_msg = malloc(msg_length + 1)
    raw_msg[0] = method;
    if (msg_length > 0)
        memcpy(&raw_msg[1], msg);
    if ( sendto(connfd, *raw_msg, (msg_length + 1), 0, NULL, 0) < 0)
        die("Send to peer failed. ERRNO %d\n", errno); //TODO don't die because of this
    free(raw_msg);
}

////////// Network action functions //////////
//none

