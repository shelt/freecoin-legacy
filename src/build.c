#include "build.h"

/*
    Contains various functions that collect data
    and assemble it in a block of memory.
    Most are used for transmission in network.c
/*
    Compiles a list of block hashes from a startblock (non inclusive)
    to `count` blocks forwards.
    Mainly used in limbo_scan() and networking.
*/
uchar build_block_list(Dbs *dbs, uchar *startblock, uchar req_blocks, uchar *dest)
{
    int ret;
    uchar *block_buffer = malloc(SIZE_BLOCK_HEADER);
    uchar *hash_buffer = malloc(SIZE_SHA256);
    
    ret = data_blocks_get_header(dbs, startblock, block_buffer);
    if (ret != 0)
        return 0;
    uint index = btoui(&block_buffer[POS_BLOCK_HEIGHT]) + 1; // + 1 because we don't send startblock
    uint max = data_chain_get_height(dbs) + 1; // + 1 because it's non-inclusive
    uint req_max = index + req_blocks;

    if ((max - index) > 255)
        max = index + 255;
        
    if (req_max < max)
        max = req_max;
    
    int count = 0;
    for (int i=index; i<max; i++)
    {
        ret = data_chain_get(dbs, i, hash_buffer);
        if (ret != 0)
            return count;
        memcpy(&hash_buffer[count * SIZE_SHA256], hash_buffer, SIZE_SHA256);
        count++;
    }
    return count;
}

uchar build_mempool_list(Dbs *dbs, uchar *dest)
{
    pthread_mutex_lock(dbs->mempool->mutex);
    int max = dbs->mempool->count;
    for (int i=0; i<max; i++)
        memcpy(&dest[i * SIZE_SHA256], dbs->mempool->txs[i], SIZE_SHA256); // TODO could use data_mempool_get()
    pthread_mutex_unlock(dbs->mempool->mutex);
}

uchar build_missing_blocks_list(Dbs *dbs, uchar *list, uchar count, uchar *dest);
{
    int missing = 0;
    for (int i=0; i<count; i++)
    {
        if ( ! data_blocks_exists(dbs, &list[i * SIZE_SHA256]))
            memcpy(&dest[SIZE_SHA256 * missing++], &list[i * SIZE_SHA256], SIZE_SHA256); //TODO precompute indices
        if (missing == 255)
            break;
    }
    return missing;
}


//dont add more to the dest if our mempool exeeds MAX_MEMPOOL_SIZE. which should be set to the MAX_BLOCK_SIZE - SIZE_BLOCK_HEADER
uchar build_missing_tx_list(Dbs *dbs, uchar *list, uchar count, uchar *dest)
{
    int missing = 0;
    for (int i=0; i<count; i++)
    {
        if ( ! data_txs_exists(dbs, &list[i * SIZE_SHA256]))
            memcpy(&dest[SIZE_SHA256 * missing++], &list[i * SIZE_SHA256], SIZE_SHA256); //TODO precompute indices
        if (missing == 255)
            break;
    }
    return missing;
}
