/*
    Contains various functions that collect data
    and assemble it in a block of memory.
    Most are used for transmission in network.c
/*
    Compiles a list of block hashes from a startblock to `count` blocks backwards.
    Mainly used in limbo_scan().
*/
void build_block_list(Dbs *dbs, uchar *startblock, uint count, uchar *dest)
{
    uchar *buffer = malloc(SIZE_BLOCK_HEADER);
    memcpy(dest, startblock, SIZE_SHA256);
    for(int i=1; i<blocklist_count; i++)
    {
        ret = data_blocks_get_header(dbs, &dest[i-1], buffer);
        if (ret != 0)
            die("Failed to get block header in build_block_list");
        memcpy(&dest[i*SIZE_SHA256], &buffer[POS_BLOCK_PREV_HASH], SIZE_SHA256);
    }
    free(buffer);
}


/****************************
** ASSEMBLY FOR NETWORKING **
****************************/
//TODO