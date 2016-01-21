#include "shared.h"
#include "printing.h"
#include "queue.h"
#include "network.h"
#include "data.h"
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

Network *join_network(Dbs *dbs)
{
    Network *network = malloc(sizeof(Network));
    network->server_ready = 0;
    network->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    network->dbs = dbs;
    network->peers = malloc(sizeof(Peer) * MAX_CONNECTED_PEERS);

    uint ret;
    network->peers[0] = malloc(sizeof(Peer));
    while (1)
    {
        pthread_mutex_lock(&network->mutex);
        if (network->peers_count > 0)
            break;
        ret = data_nodes_get_initial(network->peers[0]);
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
    return network;
}

///////////////////////
// NETWORK FUNCTIONS //
///////////////////////

// Threaded (any amount)
void *handle_connection(void *vpeer)
{
    Peer *peer = (Peer *) vpeer;

    int n;
    uchar *msg = malloc(MAX_MSG_SIZE);
    uchar *resp = malloc(MAX_MSG_SIZE);
    if (msg == NULL)
        die("Failed to allocate msg buffer");
    if (resp == NULL);
        die("Failed to allocate response buffer");
    for(;;)
    {
        n = recvfrom(peer->connfd, msg, MAX_MSG_SIZE, 0, NULL, 0);
        if (n == 0)
        {
            v_printf("Peer has shutdown");
            break;
        }
        else if (n < 0)
        {
            v_printf("Error receiving from peer. ERRNO %d\n",errno);
            break;
        }
        
        // Parsing
        if (ntohs(bytes_to_ushort(msg[0])) != __VERSION) //TODO constant
        {
            resp[0] = ERR_BAD_VERSION;
            sendto_peer(peer, CTYPE_REJECT, resp, 1);
            break;
        }
        switch (msg[2])//TODO constant
        {
            case CTYPE_REJECT:
                printf("Received CTYPE_REJECT: %d", msg[CTYPE_REJECT_POS_ERRORTYPE]); //TODO
            break;

            case CTYPE_GETBLOCKS:
                int count = build_block_list(peer->network->dbs, &msg[CTYPE_GETBLOCKS_POS_START_BLOCK], &msg[CTYPE_GETBLOCKS_POS_BLOCK_COUNT], &resp[2]);
                resp[0] = DTYPE_BLOCK;
                resp[1] = count;
                sendto_peer(peer, CTYPE_INV, resp, 2 + (count * SIZE_SHA256));
            break;

            case CTYPE_MEMPOOL:
                int count = build_mempool_list(peer->network->dbs, &resp[2]);
                resp[0] = DTYPE_TX;
                resp[1] = count;
                sendto_peer(peer, CTYPE_INV, resp, 2 + (count * SIZE_SHA256));
            break;

            case CTYPE_INV:
                switch (msg[CTYPE_INV_POS_DATATYPE])
                {
                    case DTYPE_BLOCK:
                        int count = build_missing_blocks_list(peer->network->dbs, &msg[CTYPE_INV_POS_DATA_IDS], msg[CTYPE_INV_POS_DATA_IDS_COUNT], &resp[2]);
                        resp[0] = DTYPE_BLOCK;
                        resp[1] = count;
                        sendto_peer(peer, CTYPE_GETDATA, resp, 2 + (count * SIZE_SHA256));
                    break;
                    case DTYPE_TX:
                        int count = build_missing_tx_list(peer->network->dbs, &msg[CTYPE_INV_POS_DATA_IDS], msg[CTYPE_INV_POS_DATA_IDS_COUNT], &resp[2]);
                        resp[0] = DTYPE_TX;
                        resp[1] = count;
                        sendto_peer(peer, CTYPE_GETDATA, resp, 2 + (count * SIZE_SHA256));
                    break;
                }
            break;
            case CTYPE_GETDATA:
                switch (msg[CTYPE_INV_POS_DATATYPE])
                {
                    case DTYPE_BLOCK:
                        uchar count = msg[CTYPE_GETDATA_POS_DATA_IDS_COUNT];
                        uchar max = count * SIZE_SHA256;
                        for (int i=CTYPE_GETDATA_POS_DATA_IDS; i<max; i+=SIZE_SHA256)
                            if (data_blocks_exists(peer->network->dbs, &msg[i])
                            {
                                data_blocks_get(peer->network->dbs, &msg[i], &resp[4]);
                                uint size = block_get_size(&resp[i+4]);
                                uitob(size, resp);
                                sendto_peer(peer, CTYPE_BLOCK, resp, 4 + size);
                            }
                    break;
                    case DTYPE_TX:
                        uchar count = msg[CTYPE_GETDATA_POS_DATA_IDS_COUNT];
                        for (int i=CTYPE_GETDATA_POS_DATA_IDS; i<max; i+=SIZE_SHA256)
                            if (data_mempool_exists(peer->network->dbs, &msg[i])
                            {
                                data_mempool_get(peer->network->dbs, &msg[i], &resp[4]);
                                uint size = tx_get_size(&resp[i+4]);
                                uitob(size, resp);
                                sendto_peer(peer, CTYPE_TX, resp, 4 + size);
                            }
                    break;
                    case DTYPE_PEER:
                        pthread_mutex_lock(peer->network->mutex);
                        uchar count = peer->network->peers_count;
                        for (int i=0; i<count; i++)
                        {
                            int len = peer->network->peers[i].addrlen;
                            ustob(peer->network->peers[i].port, resp);
                            ustob(len, &resp[2]);
                            memcpy(&resp[4], peer->network->peers[i].addr, len);
                            sendto_peer(peer, CTYPE_PEER, resp, 4 + len);
                        }
                        pthread_mutex_unlock(peer->network->mutex);
                    break;
                }
            break;
            case CTYPE_BLOCK:
                    pthread_mutex_lock(peer->network->mutex);//TODO why is this required here TODO are all of these used?
                    uint size = btoui(&msg[CTYPE_BLOCK_POS_SIZE]);
                    uchar *block = &msg[CTYPE_BLOCK_POS_BLOCK];
                    uint height = btoui(&block[POS_BLOCK_HEIGHT]);
                    uint curr_height = data_chain_get_height(peer->network->dbs);
                    uchar *curr_hash = malloc(SIZE_SHA256);
                    data_chain_get(peer->network->dbs, curr_height, curr_hash);
                    
                    if (valid_block(block)) //TODO and isn't too old using REASONABLE_CONFIRMATIONS
                        data_blocks_add(peer->network->dbs, block);
                    // TODO else notify peer of rejection and drop them
                    
                    if ((height <= curr_height) ||                     // less than or equal to, or
                       ( height > 2 && ((height - 2) >= curr_height))) // 2 or more greater than
                    {
                        data_limbo_add(peer->network->dbs, block);
                        // Notify peers
                        block_compute_hash(block, &resp[2]);
                        resp[0] = DTYPE_BLOCK;
                        resp[1] = 1;
                        sendto_peer(peer, CTYPE_GETDATA, resp, 2 + SIZE_SHA256);
                    }
                    else if ((height-1) == curr_height) // 1 greater than
                    {
                        if (memcmp(&block[prev_hash],
                                   curr_hash,
                                   SIZE_SHA256) == 0)
                            data_chain_add(peer->network->dbs, block);
                        else
                            data_limbo_add(peer->network->dbs, block);
                    }
                    data_limbo_scan(peer->network->dbs, peer->network);
                    pthread_mutex_unlock(peer->network->mutex);
                break;
            case CTYPE_TX:
                pthread_mutex_lock(peer->network->mutex);
                if (peer-network->mempool->count < MAX_MEMPOOL_SIZE)
                {
                    ushort in_count = btous(&msg[CTYPE_TX_POS_BODY + POS_TX_IN_COUNT]);
                    ushort out_count = btous(&msg[CTYPE_TX_POS_BODY + POS_TX_OUT_COUNT]);
                    uint lock_time = btoui(&msg[CTYPE_TX_POS_BODY + POS_TX_LOCK_TIME]);
                    M_tx tx = m_tx_gen(
                                       in_count,
                                       out_count,
                                       lock_time,
                                       &msg[CTYPE_TX_POS_BODY + POS_TX_BODY],
                                       &msg[CTYPE_TX_POS_BODY + POS_TX_BODY + (in_count * SIZE_TX_INPUT)]
                                       );
                    data_mempool_add(peer->network->dbs, tx);
                }
                pthread_mutex_unlock(peer->network->mutex);
            break;
            case CTYPE_ALERT:
                //TODO
            break;
            case CTYPE_PING:
                    sendto_peer(c, CTYPE_PONG, NULL, 0);
            break;
            case CTYPE_PONG: //TODO
                    print("Received pong from peer %d", peer->connfd);
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
        peer->network = network;
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
void sendto_peer(Peer *peer, char CTYPE, uchar *msg, uint msg_length)
{
    // Create raw msg by appending version and CTYPE bytes
    uchar *raw_msg = malloc(msg_length + 3);
    raw_msg[0] = __VERSION << 8 & 0xFF;
    raw_msg[1] = __VERSION      & 0xFF;
    raw_msg[2] = CTYPE;
    if (msg_length > 0)f
        memcpy(&raw_msg[1], msg, msg_length);
    if ( sendto(peer->connfd, raw_msg, (msg_length + 1), 0, NULL, 0) < 0)
        die("Send to peer failed. ERRNO %d\n", errno); //TODO don't die because of this
    free(raw_msg);
}



/*************************
**Message-all functions **
*************************/
//TODO

