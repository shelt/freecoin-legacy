

typedef struct
{
    DB *blocks;
    DB *chain;
    DB *limbo;
    DB *txs;
    DB *nodes;
} Dbs;


//////// Database commission functions (internal) //////////

DBT new_dbt(uchar *data, uint size)
{
    DBT dbt;
    memset(&dbt, 0, sizeof(DBT));
    
    dbt.flags = DB_DBT_USERMEM;
    dbt.data = data;
    dbt.ulen = size;

    return dbt;
}
DBT new_pdbt(uchar *data, uint psize, uint pstart)
{
    DBT dbt = new_dbt(data, 0);
    
    dbt.flags |= DB_DBT_PARTIAL;
    dbt.doff = pstart;
    dbt.dlen = psize;

    return dbt;
}

int db_put(DB *db, DBT *key, DBT *dat)
{
    return db->put(db, NULL, key, dat, 0);
}
int db_get(DB *db, DBT *key, DBT *dat)
{
    return db->get(db, NULL, key, dat, 0);
}

void db_init(DB *db, static const char *path)
{
    int ret;
    ret = db_create(db, NULL, 0);
    if (ret != 0)
        die("Failed to create database handler: %s", path);
    ret = db_open(db,
                  NULL,
                  path,
                  NULL,
                  DB_HASH,
                  DB_CREATE | DB_THREAD,
                  0);
    if (ret != 0)
        die("Failed to open database: %s", path);
}

/////////////////////////////////////////////////////////////


void dbs_init(Dbs *dbs)
{
    struct stat st = {0};
    if (stat(PATH_DATA_DIR, &st) == -1)
        mkdir(PATH_DATA_DIR, 0700);
    
    db_init(dbs->blocks, PATH_DATA_BLOCKS);
    db_init(dbs->chain, PATH_DATA_CHAIN);
    db_init(dbs->limbo, PATH_DATA_LIMBO);
    db_init(dbs->txs, PATH_DATA_TXS);
    db_init(dbs->nodes, PATH_DATA_NODES);
}

void data_die(Dbs *dbs)
{
    if (dbs->blocks != NULL)
        dbs->blocks->close(dbs->blocks, 0);
    
    if (dbs->chain != NULL)
        dbs->chain->close(dbs->chain, 0);
    
    if (dbs->limbo != NULL)
        dbs->limbo->close(dbs->limbo, 0);
    
    if (dbs->txs != NULL)
        dbs->txs->close(dbs->txs, 0); 
    
    if (dbs->nodes != NULL)
        dbs->nodes->close(dbs->nodes, 0);
}

/*********************
** SHARED FUNCTIONS **
*********************/

// move block data (starting after common_block_hash) to limbo
// add blocks given
// remove and replace blocklist data
data_block_revert(uchar *common_block_hash, uchar **blocks, uint count)
{
    //TODO
}

/*****************************
** BLOCK-SPECIFIC FUNCTIONS **
*****************************/

data_block_add(Dbs dbs, uchar *block)
{
    // Block data
    uchar *block_hash = malloc(SIZE_SHA256);
    block_compute_hash(block, hash);
    uint block_height = block_get_height(block);

    // Transaction processing
    uchar *tx_hash = malloc(SIZE_SHA256);
    uint tx_count = block_get_tx_count(block);
    uint cursor = POS_BLOCK_BODY;
    for (int i=0; i<tx_count; i++)
    {
        tx_compute_hash(block[cursor], tx_hash);

        // Insertion
        data_tx_add(dbs, tx_hash, block_height);

        cursor += tx_get_size(block[cursor]);
    }

    // Chain processing
    data_chain_set(dbs, block_height, block_hash);

    // Block processing
    DBT key = new_dbt(block_hash, SIZE_SHA256);
    DBT dat = new_dbt(block, block_compute_size(block));
    int ret = db_put(dbs->blocks, &key, &dat);
    if (ret != 0)
        printf("error %d on blocks db", ret);

    free(block_hash);
    free(tx_hash);
}

void data_block_get(Dbt *dbt, uchar *dest)
{
    die("TODO block data_block_get");
}

void data_block_get_header(Dbt *dbt, uchar *hash, uchar *dest)
{
    DBT key = new_dbt(hash, SIZE_SHA256);
    DBT dat = new_pdbt(dest, SIZE_BLOCK_HEADER, 0);
    int ret = db_get(dbs->blocks, &key, &dat);
    if (ret != 0)
        printf("error %d on blocks db", ret);
}

/**************************
** TX-SPECIFIC FUNCTIONS **
**************************/

void data_tx_add(Dbs *dbs, uchar *tx_hash, uint block_height)
{
    DBT key = new_dbt(tx_hash, SIZE_SHA256);
    DBT dat = new_dbt(block_height, sizeof(uint));
    int ret = db_put(dbs->txs, &key, &dat);
    if (ret != 0)
        printf("error %d on txs db", ret);
}


/*****************************
** CHAIN-SPECIFIC FUNCTIONS **
*****************************/

void data_chain_safe_set(Dbs *dbs, uchar *hash)
{
    uchar *block = malloc(SIZE_BLOCK_HEADER);
    data_block_get_header(dbs, hash, block);

    free(block);
}

void data_chain_set(Dbs *dbs, uint height, uchar *hash)
{
    DBT key = new_dbt(&height, sizeof(uint));
    DBT dat = new_dbt(hash, SIZE_SHA256);
}

void data_chain_get(Dbs *dbs, uint height, uchar *dest)
{
    DBT key = new_dbt(&height, sizeof(uint));
    DBT dat = new_dbt(dest, SIZE_SHA256);
    int ret = db_get(dbs->chain, &key, &dat);
    if (ret != 0)
        printf("error %d on chain db", ret);
}

uint data_chain_get_height(Dbs *dbs)
{
    DB_HASH_STAT stats;
    dbs->chain->stat(dbs->chain, &stats, 0);
    return stats.hash_nkeys;
}


//mutex_mempool
data_mempool_get_size(Mempool *mempool)
data_mempool_get(Mempool *mempool, uint index, uchar *dest)
data_mempool_add(Mempool *mempool, uchar *tx)
