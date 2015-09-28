#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include "shared.h"
#include "blockchain.h"

#define DIR_BLOCKCHAIN data/blockchain

#define INDEX_BLOCK_PREV_HASH 7

#define TESTNET_EPOCH 1451606400

typedef struct
{
    unint num;
    unchar hash[SHA256_SIZE];
} blockspecs_t;

typedef struct
{
    unshort version;
    unint time;
    unchar *prev_hash; //32B
    unchar *merkle_root; //32B
    unchar target;
    unint nonce;
    unshort tx_count;
    unchar *body;
} block_t;


void __SPECIAL__init_genesis_block()

void add_block(unchar *block)
{
    FILE *addrfile = fopen(FILE_PEERS, "r+");
};

unint is_valid_blockhash(unchar *hash, unchar target)
{
    for (int i=0; i<target; i++)
        if (!(hash[i] == 0x00))
            return 0;
    return 1;
};
unint check_block(blockdata_t blockdata)
{
    unchar *hash = malloc(SHA256_SIZE);
    compute_block_hash(blockdata, hash);
        
    unchar curr_target = get_curr_target();
    
    if (get_block_num(hash) > 0)
        return BLOCK_EXISTS;
    if (is_valid_blockhash(hash, curr_target))
        return BLOCK_HASH_INVALID;
    if (blockdata->version != __VERSION)
        return BLOCK_VERSION_INVALID;
    if (blockdata->target != curr_target)
        return BLOCK_TARGET_INVALID;

    unchar *computed_merkle = malloc(SHA256_SIZE);
    compute_merkle_root(blockdata->body, blockdata->tx_count, computed_merkle);
    if (memcmp(computed_merkle, blockdata->merkle_root) != 0)
        return BLOCK_MRKROOT_INVALID;
    

    // Latest blockspecs
    blockspecs_t *latest_blockspecs = malloc(sizeof(blockspecs_t));
    get_latest_blockspecs(latest_blockspecs);
    // Latest block data
    blockdata_t latest_blockdata = malloc(sizeof(blockdata_t));
    get_block_data(latest_blockspecs,latest_blockdata);
    if (block->time < latest_blockdata->time)
        return BLOCK_TIME_INVALID;
    
    
    free(latest_blockspecs);
    free(latest_blockdata);
    free(computed_merkle);
}


blockspecs_t *get_latest_blockspecs(blockspecs_t *blockspecs)
{
    unchar *latest = NULL;
    unint latest_num = 0;

    unchar *current = malloc(SHA256_SIZE);
    unint current_num;

    DIR *d = opendir(DIR_BLOCKCHAIN);
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            sscanf(dir->dirname, "%d-%s", current_num,current);
            if (current_num > latest_num)
                latest = current;
        }
    }
    blockspecs->num = latest_num;
    blockspecs->hash = latest;

    free(current);
    closedir(d);
    return block;
};
blockdata_t *get_block_data(blockspecs_t blockspecs)
{
    
}
blockdata_t *get_block_bytes(blockspecs_t blockspecs)
{
    
}


unint get_curr_target()
{   
    unint retval;
    // Latest blockspecs
    blockspecs_t *latest_blockspecs = malloc(sizeof(blockspecs_t));
    get_latest_blockspecs(latest_blockspecs);
    // Latest block data
    blockdata_t latest_blockdata = malloc(sizeof(blockdata_t));
    get_block_data(latest_blockspecs,latest_blockdata);

    unint curr_time = get_curr_time();
    if (curr_time < TESTNET_EPOCH && ((curr_time - latest_blockdata->time) >= SECONDS_IN_20_MINUTES))
    {
        retval = 0;
    }
    else if (latest_blockspecs->num == RECALC_TARGET_INTERVAL)
    {
        // Last recalc blockspecs
        blockspecs_t *last_recalc_blockspecs = malloc(sizeof(blockspecs_t));
        last_recalc_blockspecs->num = latest_block_num - RECALC_TARGET_INTERVAL + 1; // 2015 blocks ago

        // Last recalc block data
        blockdata_t last_recalc_blockdata = malloc(sizeof(blockdata_t));
        get_block_data(last_recalc_blockspecs,last_recalc_blockdata);

        unint timediff = latest_blockdata->time - last_recalc_blockdata->time;
        // Set upper bound on timediff
        if (timediff > SECONDS_IN_8_WEEKS)
            timediff = SECONDS_IN_8_WEEKS;
        if (timediff < (SECONDS_IN_HALF_WEEK))
        // Set lower bound on timediff
            timediff = SECONDS_IN_HALF_WEEK;
        retval = bytes_to_unint(latest_blockspecs->target) * (2 / timediff);

        free(last_recalc_blockspecs);
        free(last_recalc_blockdata);
    }
    else
        retval = bytes_to_unint(latest_blockspecs->target); // Stays same

    free(latest_blockdata);
    free(latest_blockspecs);

    if (retval > 63)
        retval = 63;
    return retval;
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
unchar *compute_merkle_root(unchar *txs, unint tx_count, unchar *out)
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
        unchar *tx = &txs[body_cursor];
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
    memcpy(out, tree_hashes[0], SHA256_SIZE); 
    
    // Free memory
    free(buffer);
    free(ctx);
    for(i=0; i<tx_count; i++) 
        free(tree_hashes[i]);
    return out;
};


