#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"
#include "crypto.h"
#include "transactions.h"
#include "blockchain.h"


/*
    The act of using a pointer parameter for returning results
    is useful for when the result is serialized into already-allocated
    memory.
    Functions that generate data
*/

// TODO perform checking as to not exceed MAX_BLOCKSIZE

/*  ********************** BLOCK **********************
    Possible size: 80B to 1MB (max size)

              HEADER 80B                     BODY
    |-----USED IN BLOCKHASH ------|
    |----------------------------------|     |--|
    0000 [4B] [32B] [32B] [4B] [4B] 0000     [?B]
    |    |    |     |     |    |    |        |
    |    |    |     |     |    |    tx_count |
    |    |    |     |     |    |             |
    |    |    |     |     |    nonce         txs
    |    |    |     |     |
    |    |    |     |     target
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
    Essentially, this function adds a header to a 1MB
    block allocated for block generation. 
    The following header fields must be modified after
    this function is used: merkle_root, nonce (most likely),
    tx_count.
    
*/
void init_block(unchar target, unchar *block)
{
    unshort version = __VERSION;
    unint time = 0xDEADBEEF;// TODO net time should be provided to this function from the mine function
    block[0] = (version >> 8) & 0xFF;
    block[1] =  version       & 0xFF;
    block[2] = (time >> 24)   & 0xFF;
    block[3] = (time >> 16)   & 0xFF;
    block[4] = (time >> 8)    & 0xFF;
    block[5] =  time          & 0xFF;
    
    get_latest_block(&block[6]);                      // 32B : prev_blockhash
    memset(&block[37],0,32);                          // 32B : merkle_root (set later)

    block[69] = (target >> 24)   & 0xFF;              // 4B  : target
    block[70] = (target >> 16)   & 0xFF;
    block[71] = (target >> 8)    & 0xFF;
    block[72] =  target          & 0xFF;

    block[73] = 0x00;                                 // 4B  : nonce (set later)
    block[74] = 0x00;
    block[75] = 0x00;
    block[76] = 0x00;

    block[77] = 0x00;                                 // 4B  : tx_count (set later)
    block[78] = 0x00;
    block[79] = 0x00;
    block[80] = 0x00;

    // Block body (txs) begins here at byte 81
    // If this changes, modify BLOCK_HEADER_SIZE in shared.h
};

void update_block_nonce(unchar *block, unint nonce)
{
    block[73] = (nonce >> 24)   & 0xFF;
    block[74] = (nonce >> 16)   & 0xFF;
    block[75] = (nonce >> 8)    & 0xFF;
    block[76] =  nonce          & 0xFF;
};

/*
                   ------ e81287 -----
                   |                 |
           ----d1074c----          70a4e6
           |            |             |
       -8ebc6a-     -d5e414-      -89b77c-
       |      |     |      |      |      |
    2d7f4d 3407a8 5edf5a 65c356 89aa32 e3e69c
       |      |      |      |      |      |
    sometx sometx sometx sometx sometx sometx

    Accepts an array of pointers to transactions.
    Generates hashes of those transactions. TODO this is all invalid now
    Hashes those hashes in pairs. The results are TREE HASHES.
    If it's an odd number, the final one is hashed with itself.
    Process is repeated until one hash remains (the MERKLE ROOT).
*/
void update_block_merkle_root(unchar *block, unint tx_count)
{
    unchar *tree_hashes[tx_count];                // An array of pointers to leaves
    unchar *buffer = malloc(SHA256_SIZE);         // The buffer where SHA256s are generated.
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    
    // Allocate hashspace
    int i;
    for(i=0; i<tx_count; i++)
        tree_hashes[i] = malloc(SHA256_SIZE);
    
    // Generate base leaves to hashspace
    unint body_cursor = 0;
    for(i=0; i<tx_count; i++)
    {
        unchar *tx = &block[BLOCK_HEADER_SIZE + 1 + body_cursor];
        unint tx_size = get_tx_size(tx);
        sha256_init(ctx);
        sha256_update(ctx, tx, tx_size);
        sha256_final(ctx, tree_hashes[i]);
        
        body_cursor += tx_size;
    }
    
    unint leaf_count = tx_count;
    unint remainder;
    unint pair_count;
    unint k;
    while(leaf_count > 1)
    {
        remainder = leaf_count % 2;
        pair_count = (leaf_count - remainder) / 2;
        
        for(k=0,i=0; k<pair_count; k++,i+=2)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[i]  , SHA256_SIZE);
            sha256_update(ctx, tree_hashes[i+1], SHA256_SIZE);
            sha256_final(ctx, buffer);
            
            // Swap buffer and destination ptrs
            unchar *temp = tree_hashes[k];
            tree_hashes[k] = buffer;
            buffer = temp;
        }
        if(remainder)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[leaf_count-1], SHA256_SIZE);
            sha256_update(ctx, tree_hashes[leaf_count-1], SHA256_SIZE);
            sha256_final(ctx, buffer);
            
            unchar *temp = tree_hashes[k];
            tree_hashes[k++] = buffer;
            buffer = temp;
        }
        leaf_count = k;
        
    }
    // Copy to merkle_root pointer
    memcpy(&block[37], tree_hashes[0], SHA256_SIZE); // 37 is first byte of merkle root
    
    // Free memory
    free(buffer);
    free(ctx);
    for(i=0; i<tx_count; i++) 
        free(tree_hashes[i]);
};

void update_block_tx_count(unchar *block, unint tx_count)
{
    block[77] = (tx_count >> 24)   & 0xFF;
    block[78] = (tx_count >> 16)   & 0xFF;
    block[79] = (tx_count >> 8)    & 0xFF;
    block[80] =  tx_count          & 0xFF;
};
