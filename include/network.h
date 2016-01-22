#ifndef NETWORK_H
#define NETWORK_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "queue.h"
#include "data.h"

struct _s_Peer //TODO does this struct need to keep track of if its a server or client connection?
{
    uint port;
    uint addr_len;
    char addr[MAX_ADDR_LENGTH];
    int connfd;
    Network *network;
};

struct _s_Network
{
    uint server_ready;
    Peer **peers;
    uint peers_count;
    Dbs *dbs;
    pthread_mutex_t peers_mutex;
};

Network *join_network(Dbs *dbs);
void *handle_connection(void *vpeer); //internal
void *server_listener(void *network); //internal
int start_client_conn(Peer *peer);    //internal
void sendto_peer(Peer *peer, char CTYPE, uchar *msg, uint msg_length);



#endif
