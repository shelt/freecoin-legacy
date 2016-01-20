#ifndef NETWORK_H
#define NETWORK_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "queue.h"
#include "data.h"

typedef struct _s_network Network;

typedef struct //TODO does this struct need to keep track of if its a server or client connection?
{
    uint port;
    uint addr_len;
    char addr[MAX_ADDR_LENGTH];
    int connfd;
    Network *network;
} Peer;

typedef struct _s_network
{
    uint server_ready;
    Peer **peers;
    uint peers_count;
    Dbs *dbs;
    pthread_mutex_t mutex;
} Network;



#define CTYPE_REJECT 0
#define CTYPE_GETBLOCKS 1
#define CTYPE_MEMPOOL 2
#define CTYPE_INV 3
#define CTYPE_GETDATA 4
#define CTYPE_BLOCK 5
#define CTYPE_TX 6
#define CTYPE_PEER 7
#define CTYPE_ALERT 8
#define CTYPE_PING 9
#define CTYPE_PONG 10

#define DTYPE_BLOCK 0
#define DTYPE_TX 1
#define DTYPE_PEER 2

#define ERR_UNUSED 12

#define ERR_BAD_VERSION 1
#define ERR_MESSAGE_MALFORMED 2
#define ERR_UNKNOWN_CTYPE 3
#define ERR_BLOCK_CONFLICT 4
#define ERR_BLOCK_TIME_INVALID 5
#define ERR_BLOCK_ROOT_INVALID 6
#define ERR_BLOCK_TARGET_INVALID 7
#define ERR_BLOCK_BAD_HASH 8
#define ERR_BLOCK_BAD_BODY 9
#define ERR_BLOCK_OVERFLOW 10
#define ERR_BLOCK_EXISTS 11
#define ERR_TX_BAD_SIG 13
#define ERR_TX_BAD_REF 14
#define ERR_TX_REF_OVERSPEND 15

#endif
