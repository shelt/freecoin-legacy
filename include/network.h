#ifndef NETWORK_H
#define NETWORK_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
// TODO fix
// Externs
//extern unchar **mempool;
//extern pthread_mutex_t mempool_mtx;

//extern unchar *latest_block;
//extern pthread_mutex_t latest_block_mtx;

//extern unchar *
//extern pthread_mutex_t mempool_mtx;

void join_network();

typedef struct conn_thread_params
{
    int connfd;
    struct sockaddr_in peeraddr; //unused
    socklen_t peerlen;           //unused
} conn_thread_params;

int start_client_conn(char *addr, unint port);

unint get_net_time();

#endif
