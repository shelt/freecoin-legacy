#ifndef NETWORK_H
#define NETWORK_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "queue.h"

typedef struct
{
    uint server_port;         // set by network thread
    uint server_active;       // set by network thread
} Net_info;
typedef struct
{
    int connfd;
    int acting_server; // 0 for client, 1 for server
    Net_info *net_info;
    struct sockaddr_in peeraddr; //unused
    socklen_t peerlen;           //unused
} conn_thread_params;
typedef struct
{
    char addr[80];
    uint port;
    uint invalid;
    int connfd;
} peer_info_t;

Net_info *join_network();

int start_client_conn(char *addr, uint port, Net_info *net_info);

uint get_net_time();

#define CTYPE_REJECT 0
#define CTYPE_GETNODES 1
#define CTYPE_GETBLOCKS 2
#define CTYPE_MEMPOOL 3
#define CTYPE_INV 4
#define CTYPE_GETDATA 5
#define CTYPE_BLOCK 6
#define CTYPE_TX 7
#define CTYPE_ALERT 8
#define CTYPE_PING 9
#define CTYPE_PONG 10

#define DTYPE_BLOCK 0
#define DTYPE_TRANSACTION 1
#define DTYPE_PEERS 2

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
