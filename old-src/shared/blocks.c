#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"
#include "crypto.h"
#include "transactions.h"
#include "blockchain.h"


// This file has to do with serialized blocks.

/*
    The act of using a pointer parameter for returning results
    is useful for when the result is serialized into already-allocated
    memory.
    Functions that generate data
*/

// TODO perform checking as to not exceed MAX_BLOCKSIZE

/*  ********************** BLOCK **********************
    Possible size: 80B to 1MB (max size)

              HEADER 80B                    BODY
    |-----USED IN BLOCKHASH ------|
    |---------------------------------|     |--|
    0000 [4B] [32B] [32B] 00  [4B] 0000     [?B]
    |    |    |     |     |   |    |        |
    |    |    |     |     |   |    tx_count |
    |    |    |     |     |   |             |
    |    |    |     |     |   nonce         txs
    |    |    |     |     |
    |    |    |     |     target
    |    |    |     |
    |    |    |     merkle_root     
    |    |    |
    |    |    prev_hash           
    |    |
    |    time
    |
    version
*/

/*
    Essentially, this function adds a header to a 1MB
    block allocated for block generation. 
    The following header fields must be modified after
    this function is used: merkle_root, nonce (most likely),
    tx_count.
    
*/
void init_block(unchar target, unchar *block)
{
    unshort version = __VERSION;
    unint time = get_curr_time();
    block[0] = (version >> 8) & 0xFF;
    block[1] =  version       & 0xFF;
    block[2] = (time >> 24)   & 0xFF;
    block[3] = (time >> 16)   & 0xFF;
    block[4] = (time >> 8)    & 0xFF;
    block[5] =  time          & 0xFF;
    
    get_latest_block(&block[6]);                      // 32B : prev_blockhash
    memset(&block[38],0,32);                          // 32B : merkle_root (set later)

    block[70] =  target          & 0xFF;              // 1B  : target

    block[71] = 0x00;                                 // 4B  : nonce (set later)
    block[72] = 0x00;
    block[73] = 0x00;
    block[74] = 0x00;

    block[75] = 0x00;                                 // 4B  : tx_count (set later)
    block[76] = 0x00;
    block[77] = 0x00;
    block[78] = 0x00;

    // Block body (txs) begins here at byte 79
    // If this changes, modify BLOCK_HEADER_SIZE in shared.h
};


/************
** SETTERS ** TODO add more
************/
void update_block_nonce(unchar *block, unint nonce)
{
    block[71] = (nonce >> 24)   & 0xFF;
    block[72] = (nonce >> 16)   & 0xFF;
    block[73] = (nonce >> 8)    & 0xFF;
    block[74] =  nonce          & 0xFF;
};

void update_block_merkle_root(unchar *block, unint tx_count)
{
    compute_merkle_root(&block[BLOCK_HEADER_SIZE], tx_count, &block[38]); // 38 is first byte of merkle root
};

void update_block_tx_count(unchar *block, unint tx_count)
{
    block[75] = (tx_count >> 24)   & 0xFF;
    block[76] = (tx_count >> 16)   & 0xFF;
    block[77] = (tx_count >> 8)    & 0xFF;
    block[78] =  tx_count          & 0xFF;
};
