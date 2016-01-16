/* data.c
    Contains functions which directly modify databases using
    DBTs. Also contains functions which perform complex database
    operations using other functions in this file.
*/

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

Db m_db_init(static const char *path)
{
    Db *db = malloc(sizeof(Db));
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
void m_db_die(Db *db)
{
    if (db != NULL)
    {
        db->close(db, 0);
        free(db);
        db = NULL;
    }
}

/////////////////////////////////////////////////////////////

int dbs_validate(Dbs *dbs)
{
    die("todo dbs_validate");
    // chain has no gaps
    // chain height == blocks count
    // all block heights in chain point to blocks listing identical block heights
    // all txs are found in specified block heights
}

Dbs *m_dbs_init()
{
    Dbs *dbs = malloc(sizeof(Dbs));

    struct stat st = {0};
    if (stat(PATH_DATA_DIR, &st) == -1)
        mkdir(PATH_DATA_DIR, 0700);
    
    db_init(dbs->blocks, PATH_DATA_BLOCKS);
    db_init(dbs->chain, PATH_DATA_CHAIN);
    db_init(dbs->limbo, PATH_DATA_LIMBO);
    db_init(dbs->txs, PATH_DATA_TXS);
    db_init(dbs->nodes, PATH_DATA_NODES);

    dbs->mempool = malloc(sizeof(Mempool));
    dbs->mempool->mutex = PTHREAD_MUTEX_INITIALIZER;
    dbs->mempool->count = 0;
    dbs->mempool->txs = malloc(sizeof(Tx *) * TX_COUNT_SOFT_LIMIT);

    return dbs;
}

void dbs_die(Dbs *dbs)
{
    free(dbs->mempool->txs);
    free(dbs->mempool);

    m_db_die(dbs->blocks);
    m_db_die(dbs->chain);
    m_db_die(dbs->limbo);
    m_db_die(dbs->txs);
    m_db_die(dbs->nodes);

    free(dbs);
    dbs = NULL;
}

/**********************
** COMPLEX FUNCTIONS **
**********************/
// Functions which don't *directly* modify one specific database.
// These functions perform more complex operations than others in
// this file.

// - Remove tx db references
// - swap with limbo
// - update chain db
// -date tx db references
void data_blocks_revert(Dbs *dbs, uchar *common_block_hash, uchar *new_latest_block_hash)
{
    uint initial_height = data_chain_get_height(dbs);
    // common_block_height
    uchar *common_block = malloc(SIZE_BLOCK_HEADER);
    data_blocks_get_header(dbs, common_block_hash, common_block);
    uint common_block_height = block_get_height(common_block);

    uchar *curr_block = malloc(MAX_BLOCK_SIZE);
    uchar *curr_block_hash = malloc(SIZE_SHA256);
    uchar *tx_hash = malloc(SIZE_SHA256);
    // Old block processing
    for (int i=initial_height; i>common_block_height; i--)
    {
        data_chain_get(i, curr_block_hash);
        data_blocks_get(dbs, curr_block_hash, curr_block);
        
        // Transaction removal
        uint tx_count = block_get_tx_count(curr_block);
        uint cursor = POS_BLOCK_BODY;
        for (int i=0; i<tx_count; i++)
        {
            // Deletion from tx db
            tx_compute_hash(&curr_block[cursor], tx_hash);
            data_txs_del(dbs, tx_hash);
            // Addition to mempool
            if (dbs->mempool != NULL)
            {
                data_mempool_add(dbs, &curr_block[cursor]);
            }
            cursor += tx_get_size(&curr_block[cursor]);
        }
        
        // Move block to limbo
        data_blocks_del(dbs, curr_block_hash);
        data_limbo_add(dbs, curr_block);

        // Remove chain entry
        data_chain_del(dbs, i);
    }
    // New block processing
    memcpy(curr_block_hash, new_latest_block_hash, SIZE_SHA256);
    uint curr_block_height;
    while (1)
    {
        data_limbo_get(dbs, curr_block_hash, curr_block);
        curr_block_height = block_get_height(curr_block);
        if (curr_block_height == common_block_height)
            break;

        // Transaction addition
        uint tx_count = block_get_tx_count(curr_block);
        uint cursor = POS_BLOCK_BODY;
        for (int i=0; i<tx_count; i++)
        {
            // Addition to tx db
            tx_compute_hash(&curr_block[cursor], tx_hash);
            data_txs_add(dbs, tx_hash);
            // Deletion from mempool
            if (dbs->mempool != NULL)
            {
                data_mempool_del(tx_hash);
            }

            cursor += tx_get_size(&curr_block[cursor]);
        }

        // Move block to blockchain
        data_limbo_del(dbs, curr_block_hash);
        data_blocks_add(dbs, curr_block);

        // Set chain entry
        data_chain_safe_set(dbs, curr_block_hash); //TODO are we sure the changes from the previous function was committed? if error, see here

        block_get_prev_hash(curr_block, curr_block_hash);
    }

    free(common_block);
    free(curr_block);
    free(curr_block_hash);
    free(tx_hash);
}

/*****************************
** BLOCK-SPECIFIC FUNCTIONS **
*****************************/
void data_blocks_add(Dbs *dbs, uchar *block)
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
        tx_compute_hash(&block[cursor], tx_hash);

        // Insertion
        data_txs_add(dbs, tx_hash, block_height);

        cursor += tx_get_size(&block[cursor]);
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

int data_blocks_get(Dbs *dbs, uchar* hash, uchar *dest)
{
    DBT key = new_dbt(hash, SIZE_SHA256);
    DBT dat = new_dbt(dest, MAX_BLOCK_SIZE);
    int ret = db_get(dbs->blocks, &key, &dat);
    
    return ret;
}

int data_blocks_get_header(Dbs *dbs, uchar *hash, uchar *dest)
{
    DBT key = new_dbt(hash, SIZE_SHA256);
    DBT dat = new_pdbt(dest, SIZE_BLOCK_HEADER, 0);
    int ret = db_get(dbs->blocks, &key, &dat);
    
    return ret;
}

/**************************
** TX-SPECIFIC FUNCTIONS **
**************************/

void data_txs_add(Dbs *dbs, uchar *tx_hash, uint block_height)
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

void data_chain_safe_set(Dbs *dbs, uchar *block_hash)
{
    uchar *block = malloc(SIZE_BLOCK_HEADER);
    int ret = data_blocks_get_header(dbs, block_hash, block);
    if (ret != 0)
        return ret;
    DBT key = (block_hash, SIZE_SHA256);
    DBT dat = (&(block_get_height(block)), sizeof(uint));
    int ret = db_set(dbs->chain, &key, &dat);
    if (ret != 0)
        printf("error %d on chain db", ret);

    free(block);
}

void data_chain_set(Dbs *dbs, uint height, uchar *hash)
{
    die("use data_chain_safe_set"); //TODO optional, debug
    DBT key = new_dbt(&height, sizeof(uint));
    DBT dat = new_dbt(hash, SIZE_SHA256);
}

int data_chain_get(Dbs *dbs, uint height, uchar *dest)
{
    DBT key = new_dbt(&height, sizeof(uint));
    DBT dat = new_dbt(dest, SIZE_SHA256);
    int ret = db_get(dbs->chain, &key, &dat);
    
    return ret;
}

uint data_chain_get_height(Dbs *dbs)
{
    DB_HASH_STAT stats;
    dbs->chain->stat(dbs->chain, &stats, 0);
    
    return stats.hash_nkeys - 1;
}


/**********************
** MEMPOOL FUNCTIONS **
**********************/
uint data_mempool_get_size(Dbs *dbs)
{
    pthread_mutex_lock(dbs->mempool->mutex);
    
    return dbs->mempool->count;
    
    pthread_mutex_unlock(dbs->mempool->mutex);
}

Tx *data_mempool_get(Dbs *dbs, uint index, uchar *dest)
{
    pthread_mutex_lock(dbs->mempool->mutex);
    
    if (index >= dbs->mempool->count)
        return NULL;

    Tx *retval = dbs->mempool->txs[i];
    
    pthread_mutex_unlock(dbs->mempool->mutex);
    return retval;
}

void data_mempool_add(Dbs *dbs, Tx *tx)
{
    pthread_mutex_lock(dbs->mempool->mutex);
    
    dbs->mempool->txs[dbs->mempool->count] = tx;
    dbs->mempool->count++;

    pthread_mutex_unlock(dbs->mempool->mutex);
}

Tx *data_mempool_del(Dbs *dbs, uchar *hash)
{
    pthread_mutex_lock(dbs->mempool->mutex);

    Tx *retval = NULL;
    uint size = dbs->mempool->size;
    for (int i=0; i<size; i++)
    {
        if (memcmp(hash, dbs->mempool->txs[i], SIZE_SHA256) == 0)
        {
            retval = dbs->mempool->txs[i];
            dbs->mempool->size--;
            if (dbs->mempool->size > 0)
                dbs->mempool->txs[i] = dbs->mempool->txs[size];
            
            dbs->mempool->txs[size] = NULL;
        }
    }
    pthread_mutex_unlock(dbs->mempool->mutex);
    return retval;
}
