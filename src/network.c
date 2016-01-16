#include "shared.h"
#include "printing.h"
#include "queue.h"
#include "network.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define FILE_PEERS "data/peers"
#define SERVER_PORT 30330
//1024*1024*5
#define MAX_MSG_SIZE 5242880
#define MAX_LINE_SIZE 1024
#define MAX_PEERS_FILE_LENGTH 1000

/*int initial_peer_fromfile(peer_info_t *peers, Net_info *net_info)
{
    // Nodes from file
    peer_info_t *nodes_fromfile = malloc(sizeof(peer_info_t)*MAX_PEERS_FILE_LENGTH);
    uint nodes_fromfile_count = 0;

    // Buffers
    char line[MAX_LINE_SIZE];
    char addr[100];
    uint port;

    // FROM FILE
    //    All peers specified in the file are loaded into memory (up to 1000).
    //    We try to connect to an initial peer. Any failed peers are
    //    removed from memory. The peerfile is then overwritten with peers in
    //    memory.
    
    FILE *addrfile = fopen(FILE_PEERS, "r+");
    if (addrfile == NULL)
        die("Failed to access address file");

    // Load entries into memory
    while (fgets(line, MAX_LINE_SIZE, addrfile) && nodes_fromfile_count < MAX_PEERS_FILE_LENGTH)
    {
        if (sscanf(line, "%s %d", addr, &port) != 3)
            break;
        peer_info_t node;
        strcpy(node.addr, addr);
        node.port = port;
        nodes_fromfile[nodes_fromfile_count++] = node;
        addr[0] = '\0';
    }
    int retval = 0;
    // Find initial peer
    for (int i=0; i<nodes_fromfile_count; i++)
    {
        int peerfd = start_client_conn(nodes_fromfile[i].addr, nodes_fromfile[i].port, net_info);
        if (peerfd < 0) // Failed
        {
            nodes_fromfile[i].invalid = 1; // mark for removal
        }
        else
        {
            nodes_fromfile[i].connfd = peerfd;
            memcpy(&(peers[0]), &(nodes_fromfile[i]), sizeof(peer_info_t));
            retval = 1;
            break;
        }
    }

    // Overwrite valid peers to file
    addrfile = freopen(NULL, "w", addrfile);
    for(int i; i<nodes_fromfile_count; i++)
        if(nodes_fromfile[i].invalid != 1)
            fprintf(addrfile, "%s %d", nodes_fromfile[i].addr, nodes_fromfile[i].port);
    fclose(addrfile);
    free(nodes_fromfile);

    return retval;
};
*/

//TODO deal with these
void webify(peer_info_t *peers, uint *peers_count);
void *server_listener(void *net_info);
void sendto_peer(int connfd, char CTYPE, uchar *msg, uint msg_length);

void join_network(Dbs *dbs)
{
    Network *network = malloc(sizeof(Network));
    Network->server_ready = 0;
    Network->peers = malloc(sizeof(Peer) * MAX_CONNECTED_PEERS);

    Dbs *dbs = m_dbs_init();

    uint ret;
    network->peers[0] = malloc(sizeof(Peer));
    while (1)
    {
        pthread_mutex_lock(network->mutex);
        if (network->peers_count > 0)
            break;
        ret = data_peers_get_top(network->peers[0]);
        if (ret != 0)
        {
            printf("Failed to find any working nodes. \n \
                    Enter manually as \"<addr> <port>\": ");
            scanf("%s %d", network->peers[0]->addr, &(network->peers[0]->port));
        }
        start_client_conn(network->peers[0]);
        if (network->peers[0]->connfd > 0)
            network->peers_count++;
        pthread_mutex_unlock(network->mutex);
    }
    // Webification
    sendto_peer(peers[*peers_count-1].connfd, CTYPE_GETNODES, 0, 0);

//TODO redone up to here




////////// OLD // OLD // OLD // OLD // OLD // OLD // OLD // OLD // OLD  /////////////////

    // Create own server
    printf("Creating socket listener...\n");
    pthread_t server;
    net_info->server_port = NET_SERVER_PORT;
    
    pthread_create(&server, NULL, server_listener, net_info);
    while(net_info->server_active != 1)
        sleep(2);
    return net_info;
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
    Net_info *net_info = ((conn_thread_params *)params)->net_info;

    int n;
    uchar *msg = malloc(MAX_MSG_SIZE);
    for(;;)
    {
        n = recvfrom(connfd, msg, MAX_MSG_SIZE, 0, NULL, 0);
        if (n == 0)
            die("Peer has shutdown"); // TODO we shouldn't die because of this
        else if (n < 0)
            die("Error recieving from peer. ERRNO %d\n",errno);
        //msg[n] = 0; not a string, TODO
        
        // Parsing
        if (msg[1] != __VERSION)
        {
            uchar resp = ERR_BAD_VERSION;
            sendto_peer(connfd, CTYPE_REJECT, &resp, 1);
            close(connfd);
        }
        switch (msg[0])
        {
            case CTYPE_REJECT:
                printf("Recieved CTYPE_REJECT: %d",msg[1]); //TODO parse message
                break;
           /* case CTYPE_VERACK: // TODO only send getblocks once per session?
                //sendto_peer(connfd, CTYPE_ADDR,  // Send addresses TODO revamp above
                uchar *body = malloc(CTYPE_GETBLOCKS_SIZE);
                // Start hash param
                get_latest_block(body);
                // Size param
                ushort count = MAX_GETBLOCKS_COUNT;
                body[SHA256_SIZE]   = count << 8 & 0xFF;
                body[SHA256_SIZE+1] = count      & 0xFF;
                sendto_peer(connfd, CTYPE_GETBLOCKS, body, SHA256_SIZE);
                break;  */
            case CTYPE_GETNODES:
                //TODO revamp above
                break;
            case CTYPE_GETBLOCKS:
                
                break;
            case CTYPE_MEMPOOL:
                break;
            case CTYPE_INV:
                if (msg[2] == DTYPE_BLOCK)
                {
                    // TODO if block we dont have, increment foreign blocks found
                }
                        
                break;
            case CTYPE_GETDATA:
                break;
            case CTYPE_BLOCK:
                break;
            case CTYPE_TX:
                break;
            case CTYPE_ALERT:
                break;
            case CTYPE_PING:
                break;
            case CTYPE_PONG:
                break;
        }
                
        
    }
    close(connfd);
};

// Threaded (once)
void *server_listener(void *net_info)
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t clilen;
    //pid_t     childpid; TODO unused


    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
        die("Server socket creation failed. ERRNO %d\n",errno);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(((Net_info *)net_info)->server_port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        die("Server failed to bind to address/port. ERRNO %d\n",errno);

    if (listen(sockfd,1024) < 0)
        die("Server listen failed. ERRNO %d\n",errno);

    
    ((Net_info *)net_info)->server_active = 1;

    for(;;)
    {
        clilen=sizeof(cliaddr);
        int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
            die("Server failed to accept a connection. ERRNO %d\n",errno);
    
        pthread_t handler_thread;

        // Argument struct
        conn_thread_params params;
        params.connfd = connfd;
        params.net_info = net_info;
        params.acting_server = 1;

        pthread_create(&handler_thread, NULL, handle_connection, &params);

    }
}


int start_client_conn(char *addr, uint port, Net_info *net_info)
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
    params.net_info = net_info;
    params.acting_server = 0;

    pthread_create(&conn_thread, NULL, handle_connection, &params);
    
    return connfd;
}

///// Misc /////
void sendto_peer(int connfd, char CTYPE, uchar *msg, uint msg_length)
{
    // Create raw msg by appending version and CTYPE bytes
    uchar *raw_msg = malloc(msg_length + 3);
    raw_msg[0] = __VERSION << 8 & 0xFF;
    raw_msg[1] = __VERSION      & 0xFF;
    raw_msg[2] = CTYPE;
    if (msg_length > 0)
        memcpy(&raw_msg[1], msg, msg_length);
    if ( sendto(connfd, raw_msg, (msg_length + 1), 0, NULL, 0) < 0)
        die("Send to peer failed. ERRNO %d\n", errno); //TODO don't die because of this
    free(raw_msg);
}

////////// Network action functions //////////
//none

