#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blocks.h"
#include "shared.h"
#include "util.h"
#include "crypto.h"
#include "transactions.h"
#include "printing.h"

//TODO rem: bit shifting loads bytes into register, equiv of converting to big-endian

// This file has to do with serialized blocks.

/*
    The act of using a pointer parameter for returning results
    is useful for when the result is serialized into already-allocated
    memory.
    Functions that generate data
*/

// TODO perform checking as to not exceed MAX_BLOCK_SIZE

/*  ********************** BLOCK **********************
    Possible size: 83B to 1MB (max size)

              HEADER 83B                         BODY
    |-----USED IN BLOCKHASH ---------------|
    |--------------------------------------|     |--|
    0000 [4B] [4B] [32B] [32B] 00 [4B] [4B]      [?B]
    |    |    |   |     |     |   |     |        |
    |    |    |   |     |     |   |     tx_count |
    |    |    |   |     |     |   |              |
    |    |    |   |     |     |   nonce          txs
    |    |    |   |     |     |
    |    |    |   |     |     target
    |    |    |   |     |
    |    |    |   |     merkle_root     
    |    |    |   |
    |    |    |   prev_hash           
    |    |    |
    |    |    height
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
void block_init(ushort version,
                uint time,
                uint height,
                uchar *prev_hash,
                uchar target,
                                  uchar *block)
{
    block[0] = (version >> 8) & 0xFF;                 // 2B : version (parameter)
    block[1] =  version       & 0xFF;
    
    block[2] = (time >> 24)   & 0xFF;                 // 4B : time (parameter)
    block[3] = (time >> 16)   & 0xFF;
    block[4] = (time >> 8)    & 0xFF;
    block[5] =  time          & 0xFF;

    block[6] = (height >> 24) & 0xFF;                 // 4B : height (parameter)
    block[7] = (height >> 16) & 0xFF;
    block[8] = (height >> 8)  & 0xFF;
    block[9] =  height        & 0xFF;
    
    memcpy(&block[10], prev_hash, SIZE_SHA256);        // 32B : prev_hash (parameter)
    memset(&block[42],0,32);                           // 32B : merkle_root (set later)

    block[74] =  target;                               // 1B  : target (parameter)

    block[75] = 0x00;                                  // 4B  : nonce (set later)
    block[76] = 0x00;
    block[77] = 0x00;
    block[78] = 0x00;

    block[79] = 0x00;                                  // 4B  : tx_count (set later)
    block[80] = 0x00;
    block[81] = 0x00;
    block[82] = 0x00;

    // Block body (txs) begins here at byte 83
    // If this changes, modify SIZE_BLOCK_HEADER in shared.h
}

/************** Note: It's not important to have getters for non-primitive
** MODIFIERS ** parts of the block; simply index with the POS_ constants.
***************/
void block_inc_nonce(uchar *block) //TODO this needs to be as fast as possible
{
    uint nonce = (block[POS_BLOCK_NONCE  ] << 24) |
                 (block[POS_BLOCK_NONCE+1] << 16) |
                 (block[POS_BLOCK_NONCE+2] << 8)  |
                  block[POS_BLOCK_NONCE+3];
    nonce++;
    block[POS_BLOCK_NONCE  ] = (nonce >> 24)   & 0xFF;
    block[POS_BLOCK_NONCE+1] = (nonce >> 16)   & 0xFF;
    block[POS_BLOCK_NONCE+2] = (nonce >> 8)    & 0xFF;
    block[POS_BLOCK_NONCE+3] =  nonce          & 0xFF;
}

void block_update_root(uchar *block)
{
    compute_merkle_root(&block[SIZE_BLOCK_HEADER], block_get_tx_count(block), &block[POS_BLOCK_MERKLE_ROOT]);
}

void block_add_tx(uchar *block, uchar *tx)
{
    uint size = block_get_size(block);
    if (size >= MAX_BLOCK_SIZE)
    {
        printfv(VERBOSE, "Warning: Transaction could not be added, block too large");
        return;
    }
    memcpy(&block[size], tx, tx_get_size(tx));
    

    // Increment tx count
    uint tx_count = block_get_tx_count(block);
    tx_count++;
    block[POS_BLOCK_TX_COUNT  ] = (tx_count >> 24)   & 0xFF;
    block[POS_BLOCK_TX_COUNT+1] = (tx_count >> 16)   & 0xFF;
    block[POS_BLOCK_TX_COUNT+2] = (tx_count >> 8)    & 0xFF;
    block[POS_BLOCK_TX_COUNT+3] =  tx_count          & 0xFF;
}

uint block_get_tx_count(uchar *block)
{
    return (block[POS_BLOCK_TX_COUNT  ] << 24) |
           (block[POS_BLOCK_TX_COUNT+1] << 16) |
           (block[POS_BLOCK_TX_COUNT+2] << 8)  |
            block[POS_BLOCK_TX_COUNT+3];
}

uint block_get_size(uchar *block)
{
    uchar tx_count = block_get_tx_count(block);
    uint cursor = SIZE_BLOCK_HEADER; // Index of first byte of a current transaction

    for(int i=0; i<tx_count; i++)
        cursor += tx_get_size(&block[cursor]);
    
    return cursor;
}


/*******************
 ** MISCELLANEOUS **
 *******************/

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
void compute_merkle_root(uchar *txs, uint tx_count, uchar *out)
{
    uchar *tree_hashes[tx_count];                // An array of pointers to leaves
    uchar *buffer = malloc(SIZE_SHA256);         // The buffer where SHA256s are generated.
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    
    // Allocate hashspace
    int i;
    for(i=0; i<tx_count; i++)
        tree_hashes[i] = malloc(SIZE_SHA256);
    
    // Generate base leaves to hashspace
    uint body_cursor = 0;
    for(i=0; i<tx_count; i++)
    {
        uchar *tx = &txs[body_cursor];
        uint tx_size = tx_get_size(tx);
        sha256_init(ctx);
        sha256_update(ctx, tx, tx_size);
        sha256_final(ctx, tree_hashes[i]);
        
        body_cursor += tx_size;
    }
    
    uint leaf_count = tx_count;
    uint remainder;
    uint pair_count;
    uint k;
    while(leaf_count > 1)
    {
        remainder = leaf_count % 2;
        pair_count = (leaf_count - remainder) / 2;
        
        for(k=0,i=0; k<pair_count; k++,i+=2)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[i]  , SIZE_SHA256);
            sha256_update(ctx, tree_hashes[i+1], SIZE_SHA256);
            sha256_final(ctx, buffer);
            
            // Swap buffer and destination ptrs
            uchar *temp = tree_hashes[k];
            tree_hashes[k] = buffer;
            buffer = temp;
        }
        if(remainder)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[leaf_count-1], SIZE_SHA256);
            sha256_update(ctx, tree_hashes[leaf_count-1], SIZE_SHA256);
            sha256_final(ctx, buffer);
            
            uchar *temp = tree_hashes[k];
            tree_hashes[k++] = buffer;
            buffer = temp;
        }
        leaf_count = k;
        
    }
    // Copy to merkle_root pointer
    memcpy(out, tree_hashes[0], SIZE_SHA256); 
    
    // Free memory
    free(buffer);
    free(ctx);
    for(i=0; i<tx_count; i++) 
        free(tree_hashes[i]);
}




/* TARGET CONVERSION */


//TODO
void target_to_etarget(uint target, uchar *etarget);


uint etarget_to_target(uchar *etarget);
