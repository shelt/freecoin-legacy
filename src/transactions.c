#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "crypto.h"
#include "transactions.h"
#include "blockchain.h"

/*  ******************* TRANSACTION *******************
    Total size = 10B+(160*input_count)+(132*output_count)

          HEADER 10B          INPUTS 160B     OUTPUTS 132B
    |---------------------|  |------------|  |------------|
    0000 0000 0000 00000000  [32B][128B]...  [128B][4B]...
    |    |    |    |         |    |          |     |
    |    |    |    lock_time |    signature  |     amount
    |    |    |              |               |
    |    |    out_count   ref_tx          out_address
    |    |
    |    in_count
    |
    version

*/

/* 
    Creates a serialized transaction beginning at pointer tx
    (which is already allocated).
*/
void gen_tx(unshort in_count, unshort out_count, unint lock_time, // Header variables
            unchar **ins, unchar **outs, unchar *tx)
{
    // These are computed here for use in for-loop tests. TODO
    //unint ins_size = in_count * TX_IN_SIZE;    // Side of ins combined
    //unint outs_size = out_count * TX_OUT_SIZE; // Size of outs combined
    //unint size = TX_HEADER_SIZE + ins_size + outs_size; // Size of transaction
    
    ///////////////////// START HEADER /////////////////////
    unshort version = __VERSION;
    tx[0] = (version >> 8)  & 0xFF;
    tx[1] =  version        & 0xFF;
    tx[2] = (in_count >> 8) & 0xFF;
    tx[3] =  in_count       & 0xFF;
    tx[4] = (out_count >> 8)& 0xFF;
    tx[5] =  out_count      & 0xFF;
    
    tx[6] = (lock_time >> 24) & 0xFF;
    tx[7] = (lock_time >> 16) & 0xFF;
    tx[8] = (lock_time >> 8)  & 0xFF;
    tx[9] =  lock_time        & 0xFF;
    
    ////////////////////// END HEADER //////////////////////
    int tx_index = 10;
    int i; // Pointer index for this loop
    for(i=0; i<in_count; i++)
    {
         memcpy(&tx[tx_index], ins[i], TX_IN_SIZE);
         tx_index += TX_IN_SIZE;
    }

    for(i=0; i<out_count; i++)
    {
        memcpy(&tx[tx_index], outs[i], TX_OUT_SIZE);
        tx_index += TX_OUT_SIZE;
    }
}

void gen_tx_input(unchar *ref_tx, unchar *sig, unchar *tx_input)
{
    memcpy(&tx_input[0], ref_tx, SHA256_SIZE);
    memcpy(&tx_input[SHA256_SIZE], sig, RSA1024_SIZE);
}
void gen_tx_output(unchar *out_address, unint amount, unchar *tx_output)
{
    if (out_address != NULL)
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


unshort get_tx_size(unchar *tx)
{
    return ((tx[2] << 8) | tx[3]) * TX_IN_SIZE  + 
           ((tx[4] << 8) | tx[5]) * TX_OUT_SIZE + TX_HEADER_SIZE;
};





/*
void gen_merkle_root_hash_OLD(unchar **txs, unshort tx_count, unchar *outhash)
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
*/
