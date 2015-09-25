#include <stdlib.h>
#include "shared.h"
#include "blockchain.h"

//struct_of_blockinfo get_block_info //?? TODO

//struct get_tx_info{} //?? TODO

/*
    Finds the latest block we have and stores its
    hash in *blockhash.
*/

unchar *get_latest_block(unchar *blockhash)
{
    // todo
    return blockhash;
};

unint get_block_num(unchar *blockhash)
{
    return 0;//TODO
};

unint get_curr_target()
{
    unchar *latest_block = malloc(SHA256_SIZE);
    if (get_block_num(get_latest_block(latest_block)) == RECALC_TARGET_INTERVAL)
    {
        // TODO time diffs
    }
    free(latest_block);
    return 0;//TODO
};

