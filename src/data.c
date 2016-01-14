

typedef struct
{
    DB *blocks;
    DB *txs;
    DB *nodes;
} Dbs;


void db_init(DB db, static const char *path)
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


Dbs *data_init()
{
    struct stat st = {0};
    if (stat(PATH_DATA_DIR, &st) == -1)
        mkdir(PATH_DATA_DIR, 0700);

    Dbs dbs = malloc(sizeof(Dbs));
    
    db_init(dbs->blocks, PATH_DATA_BLOCKS, first_time);
    db_init(dbs->txs, PATH_DATA_TXS, first_time);
    db_init(dbs->nodes, PATH_DATA_NODES, first_time);
    
    return dbs;
}

void data_die(Dbs *dbs)
{
    if (dbs->blocks != NULL)
        dbs->blocks->close(dbs->blocks, 0); 
    if (dbs->txs != NULL)
        dbs->txs->close(dbs->txs, 0); 
    if (dbs->nodes != NULL)
        dbs->nodes->close(dbs->nodes, 0);

    free(dbs);
}

data_add_block(uchar *block)
{
    //TODO DB stuff
        
    pthread_mutex_lock(&mutex_blocklist);
    FILE *f = fopen(PATH_BLOCKLIST, "ab+");
    //TODO
    close(f);
    pthread_mutex_unlock(&mutex_blocklist);
}

// move block data (starting after start_block_hash) to limbo
// add blocks given
// remove and replace blocklist data
data_revert_chain(uchar *start_block_hash, uchar **blocks, uint count)
{
    //TODO
    
    pthread_mutex_lock(&mutex_blocklist);
    FILE *f = fopen(PATH_BLOCKLIST, "ab+");
    //TODO
    close(f);
    pthread_mutex_unlock(&mutex_blocklist);
}

void data_get_latest_block_hash(uchar *dest)
{
    pthread_mutex_lock(&mutex_blocklist);
    
    FILE *f = fopen(PATH_BLOCKLIST, "ab+");

    lseek(f, -SIZE_SHA256, SEEK_END);
    read(f, dest, SIZE_SHA256);

    close(f);
    
    pthread_mutex_unlock(&mutex_blocklist);
}

//mutex_mempool
data_mempool_get_size(Mempool *mempool)
data_mempool_get(Mempool *mempool, uint index, uchar *dest)
data_mempool_add(Mempool *mempool, uchar *tx)
