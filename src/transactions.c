#include <string.h>
#include <time.h>
#include <stdio.h>
#include "crypto.h"
#include "transactions.h"



/*  ******************* TRANSACTION *******************
    Total size: 10B+(160*input_count)+(132*output_count)

          HEADER 10B          INPUTS 160B     OUTPUTS 132B
    |---------------------|  |------------|  |------------|
    0000 0000 0000 00000000  [32B][128B]...  [128B][4B]...
    |    |    |    |         |    |          |     |
    |    |    |    time      |    signature  |     amount
    |    |    |              |               |
    |    |    output_count   input_tx        out_address
    |    |
    |    input_count
    |
    freecoin_version

*/
void generate_transaction(Header_tx* header, unchar** ins, unchar** outs, size_t in_count, size_t out_count, unchar* tx)
{
    // These are computed here for use in for-loop end cases. TODO NONE OF THESE ARE USED !!!?!?!!?
    unint ins_size = in_count * TX_INPUT_BYTESIZE;    // Side of ins combined
    unint outs_size = out_count * TX_OUTPUT_BYTESIZE; // Size of outs combined
    unint size = TX_HEADER_SIZE + ins_size + outs_size; // Size of transaction
    
    tx[0] = (header->version >> 8) & 0xFF;
    tx[1] =  header->version & 0xFF;
    tx[2] = (header->in_count >> 8) & 0xFF;
    tx[3] =  header->in_count & 0xFF;
    tx[4] = (header->out_count >> 8) & 0xFF;
    tx[5] =  header->out_count & 0xFF;
    
    
    tx[6] = (header->time >> 24) & 0xFF;
    tx[7] = (header->time >> 16) & 0xFF;
    tx[8] = (header->time >> 8) & 0xFF;
    tx[9] =  header->time & 0xFF;
    
    int tx_index = 10;
    int i; // Pointer index for this loop
    for(i=0; i<in_count; i++)
    {
         memcpy(&tx[tx_index], ins[i], TX_INPUT_BYTESIZE);
         tx_index += TX_INPUT_BYTESIZE;
    }

    for(i=0; i<out_count; i++)
    {
        memcpy(&tx[tx_index], outs[i], TX_OUTPUT_BYTESIZE);
        tx_index += TX_OUTPUT_BYTESIZE;
    }
}

size_t get_tx_size(unchar *tx)
{
    
    return ((tx[2] << 8) | tx[3]) * TX_INPUT_BYTESIZE  + 
           ((tx[4] << 8) | tx[5]) * TX_OUTPUT_BYTESIZE + TX_HEADER_SIZE;
}


/////////////////////////
// MERKLE TREE HASHING //
/////////////////////////
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
    Generates hashes of those transactions.
    Hashes those hashes in pairs called TREE HASHES.
    If it's an odd number, the final one is hashed with itself.
    Process is repeated until one hash remains (the MERKLE ROOT).
*/
void generate_merkle_root(unchar** txs, size_t tx_count, unchar* outhash)
{
    unchar *tree_hashes[tx_count];                // An array of pointers to leaves
    unchar *buffer = malloc(SHA256_SIZE);         // The buffer where SHA256s are generated.
    SHA256_CTX* ctx = malloc(sizeof(SHA256_CTX));
    
    // Allocate hashspace
    int i;
    for(i=0; i<tx_count; i++)
        tree_hashes[i] = malloc(SHA256_SIZE);
    
    // Generate base leafs to hashspace
    for(i=0; i<tx_count; i++)
    {
        sha256_init(ctx);
        sha256_update(ctx, txs[i], get_tx_size(txs[i]));
        sha256_final(ctx, tree_hashes[i]);
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
    // Copy to outhash pointer
    memcpy(outhash, tree_hashes[0], SHA256_SIZE);
    
    // Free memory
    free(buffer);
    free(ctx);
    for(i=0; i<tx_count; i++) 
        free(tree_hashes[i]);
};