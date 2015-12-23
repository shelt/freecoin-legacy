#include <stdio.h>

#include "printing.h"

#include "string.h"
#include "blocks.h"
#include "util.h"
#include "bignum.h"


uchar MAX_TARGET[32] = { 0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff,
                         0xff, 0xff, 0xff, 0xff };


// Note: intended for internal use only
typedef struct
{
    short version;
    uint time;
    uint height;
    uchar *prev_hash;
    uchar *merkle_root;
    uchar target;
    uint nonce;
    short tx_count;
} Block_header;

// Note: intended for internal use only
Block_header get_block_header(uchar *out_hash)
{
    char filename[SIZE_SHA256*2];
    uchar *block = malloc(SIZE_BLOCK_HEADER);
    Block_header header;

    bytes_to_hexstr(filename, out_hash, SIZE_SHA256);
    FILE *blockf = fopen(filename, "r");
    if (blockf == 0)
        die("Failed to access block. We shouldn't be asking the filesystem for blocks we don't have.");
    
    fread(block, SIZE_BLOCK_HEADER, 1, blockf);
    
    header.version = bytes_to_ushort(block);
    header.time = bytes_to_uint(&block[POS_BLOCK_TIME]);
    memcpy(header.prev_hash, &block[POS_BLOCK_PREV_HASH], SIZE_BLOCK_PREV_HASH);
    memcpy(header.merkle_root, &block[POS_BLOCK_MERKLE_ROOT], SIZE_BLOCK_MERKLE_ROOT);
    header.target = block[POS_BLOCK_TARGET];
    header.nonce = bytes_to_uint(&block[POS_BLOCK_NONCE]);
    header.tx_count = bytes_to_ushort(&block[POS_BLOCK_TX_COUNT]);
    
    free(block);
    fclose(blockf);
    return header;
}

void get_block(); //TODO

/**************************
** BLOCKCHAIN NAVIGATION **
***************************/
//TODO
void inv_next_block(uchar *in_hash, uchar *out_hash);
void inv_prev_block(uchar *in_hash, uchar *out_hash);
void inv_block_of_height(uint height, uchar *out_hash)
{
    
}




/***************
** VALIDATION **
****************/

uint is_valid_blockhash(uchar *hash, uchar target)
{
    for (int i=0; i<target; i++)
        if (!(hash[i] == 0x00))
            return 0;
    return 1;
}


/******************
** MISCELLANEOUS **
*******************/

uint get_next_target(uchar *block_hash)
{
    uint retval;
    Block_header block = get_block_header(block_hash);

    // Note that the current time is used only for testnet purposes and this function is
    // otherwise time-independent.
    uint curr_time = get_curr_time();
    if (curr_time < TESTNET_EPOCH && ((curr_time - block.time) >= SECONDS_IN_20_MINUTES))
        retval = 0;
    else if (block.height > 0 && (block.height % RECALC_TARGET_INTERVAL) == 0)
    {
        uchar *recalc_block_hash = malloc(SIZE_SHA256);
        inv_block_of_height((block.height - RECALC_TARGET_INTERVAL), recalc_block_hash);

        uint diff = block.time - get_block_header(recalc_block_hash).time;
        
        // upper bound
        if (diff > SECONDS_IN_8_WEEKS)
            diff = SECONDS_IN_8_WEEKS;
        // lower bound
        else if (diff < (SECONDS_IN_HALF_WEEK))
            diff = SECONDS_IN_HALF_WEEK;

        // target computation
        uchar *etarget = malloc(SIZE_SHA256);
        target_to_etarget(block.target, etarget);

        uchar *big_diff = malloc(SIZE_SHA256);
        uint_to_bytes(diff, big_diff);

        uchar divisor[4];
        uint_to_bytes(2, divisor); //TODO make 2 a preproc constant

        uchar *buffer1 = malloc(SIZE_SHA256 + sizeof(diff));
        uchar *buffer2 = malloc(SIZE_SHA256 + sizeof(diff));
        
        big_mult(etarget, SIZE_SHA256, big_diff, SIZE_SHA256, buffer1);
        big_div(buffer1, divisor, buffer2);
        if (big_compare(buffer2, MAX_TARGET, SIZE_SHA256) > 0)
            retval = etarget_to_target(MAX_TARGET);
        else
            retval = etarget_to_target(buffer2);

        free(recalc_block_hash);
        free(etarget);
        free(big_diff);
        free(buffer1);
        free(buffer2);
    }
    else
    {
        retval = block.target;
    }
    return retval;
}
