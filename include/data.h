#ifndef DATA_H
#define DATA_H

#include <db.h>
#include "transactions.h"

typedef struct
{
    pthread_mutex_t mutex;
    Tx **txs;
    uint count;
} Mempool;

typedef struct
{
    DB *blocks;
    DB *chain;
    DB *limbo;
    DB *txs;
    DB *nodes;
} Dbs;


int dbs_validate(Dbs *dbs);
Dbs m_dbs_init();
void dbs_die(Dbs *dbs);
void data_blocks_revert(Dbs *dbs, uchar *common_block_hash, uchar *new_latest_block_hash);
void data_blocks_add(Dbs *dbs, uchar *block);
int data_blocks_get(Dbs *dbs, uchar* hash, uchar *dest);
int data_blocks_get_header(Dbs *dbs, uchar *hash, uchar *dest);
void data_txs_add(Dbs *dbs, uchar *tx_hash, uint block_height);
void data_chain_safe_set(Dbs *dbs, uchar *block_hash);
void data_chain_set(Dbs *dbs, uint height, uchar *hash);
int data_chain_get(Dbs *dbs, uint height, uchar *dest);
uint data_chain_get_height(Dbs *dbs);
uint data_mempool_get_size(Dbs *dbs);
Tx *data_mempool_get(Dbs *dbs, uint index, uchar *dest);
void data_mempool_add(Dbs *dbs, Tx *tx);
Tx *data_mempool_del(Dbs *dbs, uchar *hash);

#endif
