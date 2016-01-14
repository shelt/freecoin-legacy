#ifndef DATA_H
#define DATA_H

typedef struct
{
    uchar **txs;
    uint count;
} Mempool;


// Mutexes
pthread_mutex_t mutex_blocklist = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_mempool = PTHREAD_MUTEX_INITIALIZER;


#endif