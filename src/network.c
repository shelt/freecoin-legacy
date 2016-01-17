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
//TODO move define
#define SERVER_PORT 30330
#define MAX_INV_COUNT 255

void join_network(Dbs *dbs)
{
    Network *network = malloc(sizeof(Network));
    Network->server_ready = 0;
    Network->mutex = PTHREAD_MUTEX_INITIALIZER;
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
    msg_getnodes(peers[0].connfd);

    printf("Creating socket listener...\n");
    pthread_t server;
    pthread_create(&server, NULL, server_listener, network);
    while(1)
    {
        pthread_mutex_lock(network->mutex);
        if (network->server_ready != 1)
            sleep(2);
        pthread_mutex_unlock(network->mutex);
    }
}

///////////////////////
// NETWORK FUNCTIONS //
///////////////////////

// Threaded (any amount)
void *handle_connection(void *peer)
{
    // Dereferenced stack variables
    int connfd = ((Peer *)peer)->connfd;

    int n;
    uchar *msg = malloc(MAX_MSG_SIZE);
    for(;;)
    {
        n = recvfrom(connfd, msg, MAX_MSG_SIZE, 0, NULL, 0);
        if (n == 0)
            die("Peer has shutdown"); // TODO we shouldn't die because of this, simply return or something
        else if (n < 0)
            die("Error receiving from peer. ERRNO %d\n",errno); //todo don't die, return
        
        // Parsing
        if (ntohs(bytes_to_ushort(msg[0])) != __VERSION)
        {
            uchar resp = ERR_BAD_VERSION;
            sendto_peer(connfd, CTYPE_REJECT, &resp, 1);
            break;
        }
        switch (msg[2])
        {
            case CTYPE_REJECT:
                printf("Receive CTYPE_REJECT: %d", msg[CTYPE_REJECT_POS_ERRORTYPE]); //TODO
            break;

            case CTYPE_GETBLOCKS:
                msg_inv_blocks(peer, &msg[CTYPE_GETBLOCKS_POS_START_BLOCK], &msg[CTYPE_GETBLOCKS_POS_BLOCK_COUNT])
            break;

            case CTYPE_MEMPOOL:
                msg_inv_txs(peer);
            break;

            case CTYPE_INV:
                switch (msg[CTYPE_INV_POS_DATATYPE])
                { // TODO this section should be more DRY
                    case DTYPE_BLOCK:
                        uchar count = msg[CTYPE_INV_POS_DATA_IDS_COUNT];
                        int cursor = CTYPE_INV_POS_DATA_IDS;
                        for (int i=0; i<count; i++)
                        {
                            if (!(data_blocks_exists(peer->network->dbs, &msg[cursor]) ||
                                  data_limbo_exists(peer->network->dbs, &msg[cursor])))
                            {
                                uchar *hash = msg[cursor];
                                msg_getdata(peer, DTYPE_BLOCK, &hash, 1); // TODO should compile all missing ones and send a single getdata
                            }
                            cursor += SIZE_SHA256;
                        }
                    break;
                    case DTYPE_TX:
                        uchar count = msg[CTYPE_INV_POS_DATA_IDS_COUNT];
                        int cursor = CTYPE_INV_POS_DATA_IDS
                        for (int i=0; i<count; i++)
                        {
                            if (!data_mempool_exists(peer->network->dbs, &msg[cursor]))
                            {
                                uchar *hash = msg[cursor];
                                msg_getdata(peer, DTYPE_TX, &hash, 1); // TODO should compile all missing ones and send a single getdata
                            }
                            cursor += SIZE_SHA256;
                        }
                    break;
                }
                break;
            case CTYPE_GETDATA:
                switch (msg[CTYPE_INV_POS_DATATYPE])
                {
                    case DTYPE_BLOCK:
                        uchar count = msg[CTYPE_GETDATA_POS_DATA_IDS_COUNT];
                        int cursor = CTYPE_GETDATA_POS_DATA_IDS;
                        for (int i=0; i<count; i++)
                        {
                            if (data_blocks_exists(peer->network->dbs, &msg[cursor]))
                            {
                                uchar *block = malloc(MAX_BLOCK_SIZE);
                                data_blocks_get(peer->network->dbs, &msg[cursor], block);
                                msg_block(peer, block, block_compute_size(block));
                                free(block);
                            }//TODO do we need to check limbo too?
                            cursor += SIZE_SHA256;
                        }
                    break;
                    case DTYPE_TX:
                        uchar count = msg[CTYPE_GETDATA_POS_DATA_IDS_COUNT];
                        int cursor = CTYPE_GETDATA_POS_DATA_IDS;
                        for (int i=0; i<count; i++)
                        {
                            if (data_mempool_exists(peer->network->dbs, &msg[cursor]))
                            {
                                uchar *tx = malloc(MAX_TX_SIZE);
                                data_mempool_get(peer->network->dbs, &msg[cursor], tx);
                                msg_block(peer, tx, tx_compute_size(tx));
                                free(tx);
                            }
                            cursor += SIZE_SHA256;
                        }
                    break;
                    case DTYPE_PEER:
                        pthread_mutex_lock(peer->network->mutex);
                        uint count = peer->network->peers_count;
                        for (int=0; i<count; i++)
                        {
                            msg_peer(peer, peer->network->peers[i]);
                        }
                        pthread mutex_unlock(peer->network->mutex);
                    break;
                }
                break;
            case CTYPE_BLOCK:
                    //TODO HERE
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
    free((Peer *)peer));
};

// Threaded (once)
void *server_listener(void *network)
{
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t clilen;


    sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if (sockfd < 0)
        die("Server socket creation failed. ERRNO %d\n",errno);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(NET_SERVER_PORT);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        die("Server failed to bind to address/port. ERRNO %d\n",errno);

    if (listen(sockfd, 1024) < 0)
        die("Server listen failed. ERRNO %d\n",errno);

    pthread_mutex_lock(network->mutex);
    ((Network *)network)->server_ready = 1;
    pthread_mutex_unlock(network->mutex);
    
    while(1)
    {
        clilen=sizeof(cliaddr);
        int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
            die("Server failed to accept a connection. ERRNO %d\n",errno); //TODO don't die because of this
    
        pthread_t handler_thread;

        // Create peer
        Peer *peer = malloc(sizeof(Peer));
        peer->port = cliaddr.sin_port;
        peer->addr = inet_ntoa(sin_addr.s_addr);
        peer->connfd = connfd;

        pthread_create(&handler_thread, NULL, handle_connection, &peer);

    }
}


int start_client_conn(Peer *peer)
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
    inet_aton(peer->addr, servaddr.sin_addr.s_addr);
    servaddr.sin_port=htons(peer->port);

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

    pthread_create(&conn_thread, NULL, handle_connection, &peer);
    
    return connfd;
}

///// Misc /////
// This should only be called from message functions (TODO until changed)
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




////////// Network message functions //////////

void msg_reject(int c, uchar error, uchar *about, uchar size)
{
    int msg_size = 1 + size;
    uchar *msg = malloc(msg_size);
    
    msg[0] = error;
    memcpy(&msg[1], about, size);
    sendto_peer(c, CTYPE_REJECT, msg, msg_size);

    free(msg);
}

void msg_getnodes(int c)
{
    sendto_peer(c, CTYPE_GETNODES, NULL, 0);
}

void msg_getblocks(int c, uchar *start_block, uchar count)
{
    // Allocation
    int msg_size = SIZE_SHA256 + 1;
    uchar *msg = malloc(msg_size);
    
    memcpy(msg, start_block, SIZE_SHA256);
    msg[SIZE_SHA256] = count;
    sendto_peer(c, CTYPE_GETBLOCKS, msg, msg_size);
    
    free(msg);
}

void msg_mempool(int c)
{
    sendto_peer(c, CTYPE_MEMPOOL, NULL, 0);
}

void msg_inv_blocks(Peer *p, uchar *start_block, uchar block_count)
{
    uchar *start_header = malloc(SIZE_BLOCK_HEADER);
    int ret = data_blocks_get_header(peer->network->dbs, start_block, start_header);
    if (ret != 0)
        return;
    uint start = bytes_to_uint(start_header[POS_BLOCK_HEIGHT]) + 1;
    uint end = start + block_count;
    uint max = data_chain_get_height();

    if (end > max)
        end = max;
    if ((end - start) > MAX_INV_COUNT)
        end = start + MAX_INV_COUNT;
    
    // Allocation
    int msg_size = 2 + ((end-start) * SIZE_SHA256);
    uchar *msg = malloc(msg_size);

    msg[0] = DTYPE_BLOCK;
    msg[1] = end - start;
    int cursor = 2;
    for (int i=start; i<end; i++)
    {
        data_chain_get(p->network->dbs, i, &msg[cursor]);
        cursor += SIZE_SHA256;
    }
    sendto_peer(p->connfd, CTYPE_INV, msg, msg_size);

    free(msg);
}

void msg_inv_txs(Peer *p)
{
    uint count = data_mempool_get_size(p->network->dbs);
    if (count > MAX_INV_COUNT)
        count = MAX_INV_COUNT;

    int msg_size = 2 + (count * SIZE_SHA256);
    uchar *msg = malloc(msg_size);

    msg[0] = DTYPE_TX;
    msg[1] = count;
    int cursor = 2;
    for (int i=0; i<count; i++)
    {
        data_mempool_get(p->network->dbs, i, msg[cursor]);
        cursor += SIZE_SHA256;
    }
}

void msg_getdata(int c, uchar type, uchar **ids, uchar count)
{
    int msg_size = 2 + (SIZE_SHA256 * count);
    uchar *msg = malloc(msg_size);
    msg[0] = type;
    msg[1] = count;
    int cursor = 2;
    for(int i=0; i<count; i++)
    {
        memcpy(&msg[cursor], ids[i], SIZE_SHA256);
        cursor += SIZE_SHA256;
    }
    sendto_peer(c, CTYPE_GETDATA, msg, msg_size);

    free(msg);
}






////   OLD    /////

void msg_block(int c, uchar *block, uint size)
{
    int msg_size = 4 + size;
    uchar *msg = malloc(msg_size);
    uint_to_bytes(size, msg[0]);
    memcpy(&msg[4], block, size);
    sendto_peer(c, CTYPE_BLOCK, msg, msg_size);
    
    free(msg);
}
// Same as above
void msg_tx(int c, uchar *tx, uint size)
{
    int msg_size = 4 + size;
    uchar *msg = malloc(msg_size);
    uint_to_bytes(size, msg[0]);
    memcpy(&msg[4], block, size);
    sendto_peer(c, CTYPE_TX, msg, msg_size);
    
    free(msg);
}

void msg_peer(int c, Peer *peer)
{
    int msg_size = 4 + peer->addr_len;
    uchar *msg = malloc(msg_size);
    ushort_to_bytes(peer->port, &msg[0]);
    ushort_to_bytes(peer->addr_len, &msg[2]);
    memcpy(&msg[4], peer->addr, peer->addr_len);
    sendto_peer(c, CTYPE_PEER, msg, msg_size);
    
    free(msg);    
}

void alert(int c, uchar type, uchar cmd, uint time, uchar *about, uchar about_len, uchar *sig)
{
    int msg_size = 7 + about_len + SIZE_RSA1024;
    uchar *msg = malloc(msg_size);
    msg[0] = type;
    msg[1] = cmd;
    uint_to_bytes(time, &msg[2]);
    msg[6] = about_len;
    memcpy(msg[7], about, about_len);
    memcpy(msg[7 + about_len], sig, SIZE_RSA1024);
    sendto_peer(c, CTYPE_ALERT, msg, msg_size);

    free(msg);
}

void ping(int c)
{
    sendto_peer(c, CTYPE_PING, NULL, 0);
}

void pong(int c)
{
    sendto_peer(c, CTYPE_PONG, NULL, 0);
}
