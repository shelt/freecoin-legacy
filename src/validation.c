#include "printing.h"



int valid_block(uchar *block)
{
    //TODO
}

int valid_connectable_to_chain(Dbs *dbs, uchar *start_hash, uchar check_validity)
{
    int ret = 0;
    uchar *buffer = malloc(MAX_BLOCK_SIZE);
    uchar *start_hash_buffer = malloc(SIZE_SHA256);
    memcpy(start_hash_buffer, start_hash, SIZE_SHA256);

    ret = _internal_valid_connectable_to_chain(dbs, start_hash_buffer, check_validity, buffer);
    
    free(start_hash_buffer);
    free(buffer);
    return ret;
}

int _internal_valid_connectable_to_chain(Dbs *dbs, uchar *start_hash, uchar check_validity, uchar *buffer)
    int ret = 0;
    data_blocks_get(dbs, start_hash, buffer);
    if ( ! valid_block(buffer)
    if (btoui(&buffer[POS_BLOCK_HEIGHT]) > (data_chain_get_height() - REASONABLE_CONFIRMATIONS))
    {
        if (data_chain_exists(dbs, start_hash))
            ret = 1;
        else
        {
            memcpy(start_hash, &buffer[POS_BLOCK_PREV_HASH], SIZE_SHA256);
            
            ret = _data_limbo_scan_can_trace_back(dbs, start_hash, check_validity, buffer);
        }
    }
    return ret;
