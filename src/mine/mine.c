#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "shared.h"
#include "printing.h"
#include "transactions.h"
#include "blocks.h"
#include "util.h"
#include "crypto.h"
#include "network.h"
#include "queue.h"
#include "blockchain.h"

// todo validation.c
// passing a point in an array as a ptr looks like &array[x]
// understand importance of stack vars and pass by value; no memory leaks occur


void succeed(uchar *block, uchar *hash)
{
    //TODO this function doesn't need the hash and does need the nonce.
    printf("Valid block found!\n");
    for(int i=0; i<SIZE_SHA256; i++)
        printf("%02x", hash[i]);
    printf("\n\n");
    // TODO finalize nonce
    // TODO adjust data block/tx/chain, which should adjust mempool accrd
    // TODO tell peers
};


// TODO this entire function
void mine(uchar *workblock, uchar target, uint mtime)
{
    // TODO state of hash state so only the second chunk needs to be computed
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX)); //TODO malloc'ing here is neither fast nor freed
    uchar *hash = malloc(SIZE_SHA256);            //TODO malloc'ing here is neither fast not freed
    uint time_end = get_curr_time() + mtime;
    while(get_curr_time() < time_end)
    {
        sha256_init(ctx);
        sha256_update(ctx, workblock, SIZE_BLOCK_HEADER);
        sha256_final(ctx, hash);

        if (hash[0] == 0x00) // Has some zero bits
            if (is_valid_blockhash(hash, target))
            {
                succeed(workblock, hash);
                break;
            }
        block_inc_nonce(workblock);
    }
}
   
/*
    PROCESS:
    while mining
        generate block header (target included)
        generate block body
        while a new block hasn't been added
            Populate txs from queue
            Mine for n seconds (during this we are modifying header nonce)
*/
int main(int argc, char **argv)
{
    print_greeting();
    if (argc==1)
        die("Usage: %s <miner address>\n", argv[0]);
    if (strlen(argv[1]) != 128)
        die("Miner address must be 128 hexadecimal digits\n");
    
    uchar miner_addr[SIZE_SHA256];
    hexstr_to_bytes(miner_addr, argv[1], SIZE_SHA256);

    // TODO validation

    printf("Preparing to begin mining...\n");

    printf("Establishing self on network...\n");
    join_network(dbs);
    
    // Latest block info allocations
    uchar *latest_block_hash = malloc(SHA256);
    uint latest_block_height;
    
    // Get latest block info (pre-update)
    latest_block_height = data_chain_get_height();
    data_chain_get(dbs, latest_block_height, latest_block_hash);

    // Update our blockchain and mempool
    netall_getblocks(latest_block_hash, MAX_GETBLOCKS);
    netall_mempool();

    sleep(10) // TODO may be excessive
    // Get latest block info (post-update)
    latest_block_height = data_chain_get_height();
    data_chain_get(dbs, latest_block_height, latest_block_hash);
    

    printf("Initializing workblock...\n");
    uchar *workblock = malloc(MAX_BLOCK_SIZE);

    // Used to dequeue recieved transactions
    uchar *tx_buffer = malloc(MAX_TX_SIZE);
    // Used to compare latest block hash with block's prev_hash
    uchar *latest_block_hash_cmp_buffer = malloc(SIZE_SHA256);

    uint target;
    while(1)
    {
        printf("Starting a new block...\n");
        
        // Update info
        latest_block_height = data_chain_get_height();
        data_chain_get(dbs, latest_block_height, latest_block_hash);
        target = compute_next_target(dbs, latest_block_hash);

        void block_init(__VERSION,
                        get_curr_time(),
                        (latest_block_height + 1),
                        latest_block_hash,
                        get_next_target(latest_block_hash),
                        workblock);

        // Coinbase transaction
        uchar *output = malloc(SIZE_TX_OUTPUT);
        gen_tx_output(miner_addr, MINING_REWARD, output)
        Tx *coinbase_tx = m_tx_gen(0, 1, 0, NULL, &output);
        block_add_tx(workblock, coinbase_tx);
        free(output);
        
        
        while(1)
        {
            // Check if workblock is stale
            data_chain_get(dbs, data_chain_get_height(), latest_block_hash_cmp_buffer);
            if (memcmp(latest_block_hash_cmp_buffer, latest_block_hash, SIZE_SHA256) != 0) // TODO not very fast
                break;

            // Check for new mempool txs
            int diff = data_mempool_get_size(mempool) - (block_get_tx_count(workblock) - 1); // -1 for coinbase tx
            for(int i=0; i<diff; i++)
            {
                data_mempool_get(mempool, i, tx_buffer);
                block_add_tx(workblock, tx_buffer);
            }

            block_set_time(workblock, get_curr_time()); // TODO We could also modify the coinbase output based on surplus of txs
            mine(workblock, target, 10000);
        }
    }
    free(latest_block_hash);
    free(workblock);
    free(tx_buffer);
    free(latest_block_hash_cmp_buffer);
    return 0;
}
