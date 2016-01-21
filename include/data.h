#ifndef DATA_H
#define DATA_H

#include <db.h>
#include "transactions.h"
#include "array.h"
#include "network.h"
#include "build.h"


struct _s_Mempool
{
    pthread_mutex_t mutex;
    M_tx **txs;
    uint count;
};

struct _s_Dbs
{
    DB *blocks;
    DB *chain;
    DB *limbo;
    DB *txs;
    DB *nodes;
    Mempool *mempool;
};


DBT new_dbt(uchar *data, uint size);                //internal
DBT new_pdbt(uchar *data, uint psize, uint pstart); //internal
int db_put(DB *db, DBT *key, DBT *dat);             //internal
int db_get(DB *db, DBT *key, DBT *dat);             //internal
DB m_db_init(const char *path, uint type);
void m_db_die(DB *db);
int dbs_validate(Dbs *dbs);
Dbs *m_dbs_init();
void dbs_die(Dbs *dbs);

int data_blocks_revert(
                       Dbs *dbs,
                       uchar *common_block_hash,
                       uchar *new_latest_block_hash
                       );
void data_blocks_add(Dbs *dbs, uchar *block);
int data_blocks_get(Dbs *dbs, uchar* hash, uchar *dest);
int data_blocks_get_header(Dbs *dbs, uchar *hash, uchar *dest);

void data_txs_add(Dbs *dbs, uchar *tx_hash, uchar *block_hash);

void data_chain_safe_set(Dbs *dbs, uchar *block_hash);
void data_chain_set(Dbs *dbs, uint height, uchar *hash);
int data_chain_get(Dbs *dbs, uint height, uchar *dest);
uint data_chain_get_height(Dbs *dbs);

uint _data_VAR_get_height(DBT dbt);

uint data_limbo_get_height(Dbs *dbs);
void data_limbo_safe_add(Dbs *dbs, uchar *block);
void data_limbo_del(Dbs *dbs, uchar *block);
void data_limbo_scan(Dbs *dbs, Network *network);
int data_limbo_scan_can_trace_back(
                                   Dbs *dbs,
                                   uchar *start_hash,
                                   uchar *end_hash
                                   );
int _data_limbo_scan_can_trace_back(
                                    Dbs *dbs,
                                    uchar *start_hash,
                                    uchar *end_hash,
                                    uchar *buffer
                                    );

int data_nodes_get_initial(Peer *peer);

uint data_mempool_get_size(Dbs *dbs);
Tx *data_mempool_get(Dbs *dbs, uint index);
void data_mempool_add(Dbs *dbs, Tx *tx);
int data_mempool_exists(Dbs *dbs, uchar *hash);
Tx *data_mempool_del(Dbs *dbs, uchar *hash);


#endif
