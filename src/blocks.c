#include <stdlib.h>
#include "shared.h"

/*
    The act of using a pointer parameter for returning results
    is useful for when the result is serialized into already-allocated
    memory.
    Functions that generate data
*/

// TODO perform checking as to not exceed MAX_BLOCKSIZE
// TODO update time every few seconds of mining
// TODO get rid of memcpys and use loops for endian-safeness

/*  ********************** BLOCK **********************
    Possible size: 78B to 1MB (max size)

              HEADER 78B                     BODY
    |-----------------------------|     |-----------|
    0000 [4B] [32B] [32B] [4B] [4B]     0000     [?B]
    |    |    |     |     |    |        |        |
    |    |    |     |     |    nonce    |        txs
    |    |    |     |     |             |
    |    |    |     |     target        tx_count
    |    |    |     |
    |    |    |     merkle_root     
    |    |    |
    |    |    prev_block_hash           
    |    |
    |    time
    |
    version
*/

/*
    unchar *block: 1MB
*/
void gen_work_block(unshort version, unint time, unchar *prev_block_hash,
                    unchar **txs, unshort tx_count, unchar *block)
{
    unshort version = __VERSION;
    unint time = get_net_time();
    unchar *prev_block_hash
    block[0] = (version >> 8) & 0xFF;
    block[1] =  version       & 0xFF;
    block[2] = (time >> 24)   & 0xFF;
    block[3] = (time >> 16)   & 0xFF;
    block[4] = (time >> 8)    & 0xFF;
    block[5] =  time          & 0xFF;
    
    get_latest_block(&block[6]);                      // 32B : prev_blockhash
    gen_merkle_root(txs, tx_count, &block[37]);       // 32B : merkle_root

    target_to_bits(get_curr_target(target),block[69]) // 4B  : bits

    block[73] = 0x00;                                 // 4B  : nonce
    block[74] = 0x00;
    block[75] = 0x00;
    block[76] = 0x00;

    // Block body begins here at byte 77
};

void set_block_nonce(unchar *block, unint nonce)
{
    block[73] = (nonce >> 24)   & 0xFF;
    block[74] = (nonce >> 16)   & 0xFF;
    block[75] = (nonce >> 8)    & 0xFF;
    block[76] =  nonce          & 0xFF;
};
