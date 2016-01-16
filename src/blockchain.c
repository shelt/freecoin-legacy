#include <stdio.h>
#include "printing.h"
#include "string.h"
#include "blocks.h"
#include "util.h"
#include "bignum.h"
#include "shared.h"
#include "data.h"

/* blockchain.c
    Contains utility functions dealing with the blockchain
    such as validation, target computation, etc.
*/



/***************
** VALIDATION **
****************/

int is_valid_blockhash(uchar *hash, uchar target)
{
    int retval;
    
    uchar *etarget = malloc(SIZE_SHA256);
    target_to_etarget(target, etarget);

    retval = big_compare(hash, etarget, SIZE_SHA256) <= 0;

    free(etarget);
    return retval;
}

// TODO misc is_coinbase_tx ?


/******************
** MISCELLANEOUS **
*******************/

uint compute_next_target(Dbs *dbs, uchar *block_hash)
{
    uint retval;
    
    // Get block
    uchar *block = malloc(SIZE_BLOCK_HEADER);
    data_blocks_get_header(dbs, block_hash, block);

    // Get block info
    uint block_time = block_get_uint(block, POS_BLOCK_TIME);
    uint block_height = block_get_uint(block, POS_BLOCK_HEIGHT);
    uint block_target = block_get_uint(block, POS_BLOCK_TARGET);

    // Note that the current time is used only for testnet purposes and this function is
    // otherwise time-independent.
    uint curr_time = get_curr_time();
    if (curr_time < TESTNET_EPOCH && ((curr_time - block_time) >= SECONDS_IN_20_MINUTES))
        retval = 0;
    else if ((block_height > 0) && (block_height % RECALC_TARGET_INTERVAL) == 0)
    {
        uchar *recalc_block_hash = malloc(SIZE_SHA256);
        data_chain_get(dbs, (block_height - RECALC_TARGET_INTERVAL), recalc_block_hash);

        // Get recalc_block
        uchar *recalc_block = malloc(SIZE_BLOCK_HEADER);
        data_blocks_get_header(dbs, recalc_block_hash, recalc_block);

        // Get recalc_block info
        uint recalc_block_time = block_get_uint(recalc_block, POS_BLOCK_TIME);

        uint diff = block_time - recalc_block_time;
        
        // upper bound
        if (diff > SECONDS_IN_8_WEEKS)
            diff = SECONDS_IN_8_WEEKS;
        // lower bound
        else if (diff < (SECONDS_IN_HALF_WEEK))
            diff = SECONDS_IN_HALF_WEEK;

        // Actual target computation
        uchar *etarget = malloc(SIZE_SHA256);
        target_to_etarget(block_target, etarget);

        uchar *big_diff = malloc(SIZE_SHA256);
        uint_to_bytes(diff, big_diff);

        uchar divisor[4];
        uint_to_bytes(2, divisor); //TODO make 2 a preproc constant

        uchar *buffer1 = malloc(SIZE_SHA256 + sizeof(diff));
        uchar *buffer2 = malloc(SIZE_SHA256 + sizeof(diff));
        
        big_mult(etarget, SIZE_SHA256, big_diff, SIZE_SHA256, buffer1);
        big_div(buffer1, divisor, buffer2);
        
        memset(buffer1, 0xff, SIZE_SHA256);
        if (big_compare(buffer2, buffer1, SIZE_SHA256) > 0)
            retval = etarget_to_target(buffer1);
        else
            retval = etarget_to_target(buffer2);

        free(recalc_block_hash);
        free(recalc_block);
        free(etarget);
        free(big_diff);
        free(buffer1);
        free(buffer2);
    }
    else
    {
        retval = block_target;
    }
    free(block);

    return retval;
}


