#include <stdlib.h>
#include "shared.h"

// 1024*1024
#define MAX_BLOCKSIZE 1048576

// TODO perform checking as to not exceed MAX_BLOCKSIZE

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
    freecoin_version
*/

void generate_block(unshort version, unint time, unchar *prev_block_hash,
                    unchar *merkle_root, unint target, unint nonce,
                    unchar **txs, unshort tx_count)
{
    unchar *block = malloc(MAX_BLOCKSIZE);
    //TODO header
    //TODO current cursor for writing body
};
