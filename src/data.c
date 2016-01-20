#include "array.h"
#include "build.h"


/* data.c
    Contains functions which directly modify databases using
    DBTs. Also contains functions which perform complex database
    operations using other functions in this file.
*/

/*********************************************
** Database commission functions (internal) **
*********************************************/

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

Db m_db_init(static const char *path, uint type)
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
                  type,
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

/*******************************
** Database control functions **
*******************************/

//TODO purge
//remove txs which reference nonexistant blocks
//remove old limbo blocks and respective limbo references


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
    
    db_init(dbs->blocks, PATH_DATA_BLOCKS, DB_HASH);
    db_init(dbs->chain, PATH_DATA_CHAIN, DB_BTREE);
    db_init(dbs->limbo, PATH_DATA_LIMBO, DB_BTREE);
    db_init(dbs->txs, PATH_DATA_TXS, DB_HASH);
    db_init(dbs->nodes, PATH_DATA_NODES, DB_HASH);

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

/**************************
** Multiple-db functions **
**************************/
// Functions which don't *directly* modify one specific database.
// These functions perform more complex operations than others in
// this file.

// - Remove old txs
// - Move old hashes from chain to limbo
// - Move new hashes from limbo to chain
// - Add new txs
int data_blocks_revert(Dbs *dbs, uchar *common_block_hash, uchar *new_latest_block_hash)
{
    int ret;
    
    // initial_height
    uint initial_height = data_chain_get_height(dbs);
    // common_block_height
    uchar *common_block = malloc(SIZE_BLOCK_HEADER);
    ret = data_blocks_get_header(dbs, common_block_hash, common_block);
    if (ret != 0)
        return ret;
    uint common_block_height = block_get_height(common_block);

    uchar *curr_block = malloc(MAX_BLOCK_SIZE);
    uchar *curr_block_hash = malloc(SIZE_SHA256);
    uchar *tx_hash = malloc(SIZE_SHA256);
    
    // Old block processing
    for (int i=initial_height; i>common_block_height; i--)
    {
        data_chain_get(i, curr_block_hash);
        ret = data_blocks_get(dbs, curr_block_hash, curr_block);
        if (ret != 0)
            return ret;
        
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
        data_limbo_safe_add(dbs, curr_block);
    }
    
    // New block processing
    memcpy(curr_block_hash, new_latest_block_hash, SIZE_SHA256);
    uint curr_block_height;
    while (1)
    {
        data_blocks_get(dbs, curr_block_hash, curr_block);
        if (ret != 0)
            return ret;
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
                data_mempool_del(tx_hash);

            cursor += tx_get_size(&curr_block[cursor]);
        }

        // Move block to to chain
        data_limbo_del(dbs, curr_block);
        data_chain_set(dbs, curr_block); //assumes new highest block is higher than old one TODO

        block_get_prev_hash(curr_block, curr_block_hash);
    }

    free(common_block);
    free(curr_block);
    free(curr_block_hash);
    free(tx_hash);
}

/********************************
** "blocks" database functions **
********************************/

//TODO all true blocks need to add to chain as well
// data_chain_set(dbs, block_height, block_hash);
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
        data_txs_add(dbs, tx_hash, block_hash);

        cursor += tx_get_size(&block[cursor]);
    }

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

/*****************************
** "txs" database functions **
*****************************/

void data_txs_add(Dbs *dbs, uchar *tx_hash, uchar *block_hash)
{
    DBT key = new_dbt(tx_hash, SIZE_SHA256);
    DBT dat = new_dbt(block_hash, SIZE_SHA256);
    int ret = db_put(dbs->txs, &key, &dat);
    if (ret != 0)
        printf("error %d on txs db", ret);
}


/*******************************
** "chain" database functions **
*******************************/

void data_chain_safe_set(Dbs *dbs, uchar *block_hash)
{
    uchar *block = malloc(SIZE_BLOCK_HEADER);
    int ret = data_blocks_get_header(dbs, block_hash, block);
    if (ret != 0)
        return ret;
    DBT key = (block_hash, SIZE_SHA256);
    DBT dat = (&(block_get_height(block)), sizeof(uint));
    int ret = db_set(dbs->chain, &key, &dat); //TODO db_set isn't a function. check bdb api to make sure `get` will `set` an existing entry
    if (ret != 0)
        printf("error %d on chain db", ret);

    free(block);
}

void data_chain_set(Dbs *dbs, uint height, uchar *hash)
{
    die("use data_chain_safe_set"); //TODO optional
    DBT key = new_dbt(&height, sizeof(uint));
    DBT dat = new_dbt(hash, SIZE_SHA256);
    //not finished anyway
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
    return _data_VAR_get_height(dbs->chain);
}

//TODO this function can just get DB_LAST
// Also used by similar limbo function
uint _data_VAR_get_height(DBT dbt)
{
    DBC *dbc;
    DBT key, dat;

    uchar *block_hash = malloc(SIZE_SHA256);
    uchar *block_header = malloc(BLOCK_HEADER_SIZE);

    int ret = dbt->cursor(dbt, NULL, &dbc, 0);
    if (ret != 0)
        die("failed to init cursor in _data_VAR_get_height");

    key = new_dbt(block_hash, SIZE_SHA256);
    dat = new_dbt(block, block_header);

    uint highest;
    uint curr;
    while (dbt->c_get(dbt, &key, &dat, DB_NEXT) == 0)
    {
        curr = block_get_height(dat);
        if (curr > highest)
        highest = curr;
    }
    free(block_hash);
    free(block_header);
    dbc->close(dbc);
    return highest;
}

/*******************************
** "limbo" database functions **
*******************************/

uint data_limbo_get_height(Dbs *dbs)
{
    return _data_VAR_get_height(dbs->limbo);
}

// Safe add: extract height from given block
void data_limbo_safe_add(Dbs *dbs, uchar *block)
{
    // Compute block hash
    uchar *hash = malloc(SIZE_SHA256);
    block_compute_hash(block, hash);

    // Get height
    uint height = block_get_height(block);

    // Initialize buffer
    uint buffer_size = 4 + (SIZE_SHA256 * LIMBO_MAX_LEVEL_SIZE);
    uchar *buffer = malloc(buffer_size);
    
    DBT key = new_dbt(height, sizeof(uint));
    DBT dat = new_dbt(buffer, buffer_size);
    ret = db_get(dbs->limbo, &key, &dat);
    if (ret != 0)
        printf("error %d on limbo db add get", ret);
    
    Array arr = array_init(&buffer[4], SIZE_SHA256, bytes_to_uint(buffer), LIMBO_MAX_LEVEL_SIZE);
    array_add(arr, hash);

    ret = db_put(dbs->limbo, &key, &dat);
    if (ret != 0)
        printf("error %d on limbo db add put", ret);
}

//TODO same as above notice
void data_limbo_del(Dbs *dbs, uchar *block)
{
    // Compute block hash
    uchar *hash = malloc(SIZE_SHA256);
    block_compute_hash(block, hash);

    // Get height
    uint height = block_get_height(block);

    // Initialize buffer
    uint buffer_size = 4 + (SIZE_SHA256 * LIMBO_MAX_LEVEL_SIZE);
    uchar *buffer = malloc(buffer_size);
    
    DBT key = new_dbt(height, sizeof(uint));
    DBT dat = new_dbt(buffer, buffer_size);
    ret = db_get(dbs->limbo, &key, &dat);
    if (ret != 0)
        printf("error %d on limbo db add get", ret);
    
    Array arr = array_init(&buffer[4], SIZE_SHA256, bytes_to_uint(buffer), LIMBO_MAX_LEVEL_SIZE);
    array_del(arr, hash);

    ret = db_put(dbs->limbo, &key, &dat);
    if (ret != 0)
        printf("error %d on limbo db add put", ret);
}

/******************************
** "limbo" database scanning **
******************************/

void data_limbo_scan(Dbs *dbs, Network *network)
{
    // Current block data
    uint curr_height = data_chain_get_height();

    // Init cursor
    DBC *dbc;
    int ret = dbt->cursor(dbt, NULL, &dbc, DB_LAST);
    if (ret != 0)
        die("failed to init cursor in limbo_scan");

    // Cursor dbt`s
    uint index; // Represents the current height in limbo
    DBT key = new_dbt(&index, sizeof(uint));
    uint buffer_size = 4 + (SIZE_SHA256 * LIMBO_MAX_LEVEL_SIZE);
    uchar *buffer = malloc(buffer_size);
    DBT dat = new_dbt(buffer, buffer_size);

    int already_swapped = 0;
    // Loop buffer
    uchar *end_hash_buffer = malloc(SIZE_SHA256);
    while (dbs->limbo->c_get(dbs->limbo, &key, &dat, DB_PREV) == 0)
    {
        // Only attempt blocks which would make our chain higher
        if (index <= curr_height)
            break;
        // Process each block hash listed at this level
        Array arr = array_init(&buffer[4], SIZE_SHA256, bytes_to_uint(buffer), LIMBO_MAX_LEVEL_SIZE);
        for (int i=0; i<arr.length; i++)
        {
            uchar *hash = array_get(arr, i);
            if (data_limbo_scan_can_trace_back(dbs, hash, end_hash_buffer))
                if ( !already_swapped && is_valid_chain(dbs, hash, end_hash_buffer))
                {
                    already_swapped = 1;
                    data_blocks_revert(dbs, hash, end_hash_buffer);

                    // Send block to others in <inv>
                    uint block_list_count = index - curr_height;
                    uchar *block_list = malloc(block_list_count * SIZE_SHA256);
                    data_build_block_list(dbs, hash, block_list_count, block_list);
                    msgall_inv(network, DTYPE_BLOCK, blocklist, blocklist_count);
                    free(block_list);
                }
            else
                msgall_getdata(network, DTYPE_BLOCK, end_hash_buffer, 1);
        }

    }
    free(end_hash_buffer);
    free(inv_hash_buffer);
}

/*
    Returns 1 if a block can be traced back to a block in the chain
    using blocks in limbo. Returns 0 if the chain formed from start_hash
    never encounters the blockchain (or after REASONABLE_CONFIRMATIONS is passed,
    at which point it becomes infeasible that we will find a shared block anyway).
*/
int data_limbo_scan_can_trace_back(Dbs *dbs, uchar *start_hash, uchar *end_hash)
{
    int ret = 0;
    uchar *buffer = malloc(BLOCK_HEADER_SIZE);
    uchar *start_hash_buffer = malloc(SIZE_SHA256);
    memcpy(start_hash_buffer, start_hash, SIZE_SHA256);

    ret = _data_limbo_scan_can_trace_back(dbs, start_hash_buffer, end_hash, buffer);
    
    free(start_hash_buffer);
    free(block_buffer);
    return ret;
}

/*
    Intermediate recursive function used above.
    start_hash is allowed to be mutated.
*/
int _data_limbo_scan_can_trace_back(Dbs *dbs, uchar *start_hash, uchar *end_hash, uchar *buffer)
{
    int ret = 0;
    data_blocks_get_header(dbs, start_hash, buffer);
    if (btoui(&buffer[POS_BLOCK_HEIGHT]) > (data_chain_get_height() - REASONABLE_CONFIRMATIONS))
    {
        if (data_chain_exists(dbs, start_hash))
            ret = 1;
        else
        {
            memcpy(start_hash, &buffer[POS_BLOCK_PREV_HASH], SIZE_SHA256);
            
            ret = _data_limbo_scan_can_trace_back(dbs, start_hash, end_hash, buffer);
        }
    }
    return ret;
}



/*******************************
** "mempool" struct functions **
*******************************/

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
