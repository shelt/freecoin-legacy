#ifndef DATA_H
#define DATA_H

typedef struct
{
    uchar *hash;
    uchar *data;
} Mempool_tx;

typedef struct
{
    Mempool-tx *txs;
    uint count;
} Mempool;


// Mutexes
pthread_mutex_t mutex_mempool = PTHREAD_MUTEX_INITIALIZER;


#endif
