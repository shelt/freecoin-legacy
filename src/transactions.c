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
    |    |    output_count   ref_tx          out_address
    |    |
    |    input_count
    |
    freecoin_version

*/
void generate_transaction(unshort version, unshort in_count, unshort out_count, unint time, // Header variables
                            unchar **ins, unchar **outs, unchar *tx)
{
    // These are computed here for use in for-loop tests. TODO
    //unint ins_size = in_count * TX_INPUT_BYTESIZE;    // Side of ins combined
    //unint outs_size = out_count * TX_OUTPUT_BYTESIZE; // Size of outs combined
    //unint size = TX_HEADER_SIZE + ins_size + outs_size; // Size of transaction
    
    ///////////////////// START HEADER /////////////////////
    
    tx[0] = (version >> 8)  & 0xFF;
    tx[1] =  version        & 0xFF;
    tx[2] = (in_count >> 8) & 0xFF;
    tx[3] =  in_count       & 0xFF;
    tx[4] = (out_count >> 8)& 0xFF;
    tx[5] =  out_count      & 0xFF;
    
    
    tx[6] = (time >> 24) & 0xFF;
    tx[7] = (time >> 16) & 0xFF;
    tx[8] = (time >> 8) & 0xFF;
    tx[9] =  time & 0xFF;
    
    ////////////////////// END HEADER //////////////////////
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

void generate_tx_input(unchar *ref_tx, unchar *sig, unchar *tx_input)
{
    memcpy(&tx_input[0], ref_tx, SHA256_SIZE);
    memcpy(&tx_input[SHA256_SIZE], sig, RSA1024_SIZE);
}
void generate_tx_output(unchar *out_address, unint amount, unchar *tx_output)
{
    memcpy(&tx_output[0], out_address, RSA1024_SIZE);
    // Convert int to byte array
    int i;
    unchar *amount_bytes = malloc(sizeof(unint));
    amount_bytes[0] = (amount >> 24) & 0xFF;
    amount_bytes[1] = (amount >> 16) & 0xFF;
    amount_bytes[2] = (amount >>  8) & 0xFF;
    amount_bytes[3] =  amount & 0xFF;
    memcpy(&tx_output[RSA1024_SIZE], amount_bytes, sizeof(unint));
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
    Hashes those hashes in pairs. The results are TREE HASHES.
    If it's an odd number, the final one is hashed with itself.
    Process is repeated until one hash remains (the MERKLE ROOT).
*/
void generate_merkle_root(unchar **txs, size_t tx_count, unchar *outhash)
{
    unchar *tree_hashes[tx_count];                // An array of pointers to leaves
    unchar *buffer = malloc(SHA256_SIZE);         // The buffer where SHA256s are generated.
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    
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