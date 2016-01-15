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


/***************
** VALIDATION **
****************/

uint is_valid_blockhash(uchar *hash, uchar target)
{
    die("todo is_valid_blockhash");
}

// TODO misc is_coinbase_tx ?


/******************
** MISCELLANEOUS **
*******************/

uint compute_next_target(uchar *block_hash)
{
    uint retval;
    Block_header block = get_block_header(block_hash); //TODO update with new db method, needs to accept dbs (should it be in data.c?)

    uint block_time = block_get_time(block);
    uint block_height = block_get_height(block);
    uint block_get_target = block_get_target(block);

    // Note that the current time is used only for testnet purposes and this function is
    // otherwise time-independent.
    uint curr_time = get_curr_time();
    if (curr_time < TESTNET_EPOCH && ((curr_time - block_time) >= SECONDS_IN_20_MINUTES))
        retval = 0;
    else if (block_height > 0 && (block_height % RECALC_TARGET_INTERVAL) == 0)
    {
        uchar *recalc_block_hash = malloc(SIZE_SHA256);
        inv_block_of_height((block_height - RECALC_TARGET_INTERVAL), recalc_block_hash);

        uint diff = block_time - get_block_header(recalc_block_hash).time;
        
        // upper bound
        if (diff > SECONDS_IN_8_WEEKS)
            diff = SECONDS_IN_8_WEEKS;
        // lower bound
        else if (diff < (SECONDS_IN_HALF_WEEK))
            diff = SECONDS_IN_HALF_WEEK;

        // target computation
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
        retval = block_target;
    }
    return retval;
}
