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
#include "validation.h"

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
    int ret;
    
    print_greeting();
    if (argc == 1)
        die("Usage: %s <miner address>\n", argv[0]);
    if (strlen(argv[1]) != 128)
        die("Miner address must be 128 hexadecimal digits\n");
    
    // Store miner address
    uchar miner_addr[SIZE_SHA256];
    hexstr_to_bytes(argv[1], miner_addr, SIZE_SHA256);
    
    printf("Opening databases...");
    //TODO
    
    printf("Establishing self on network...\n");
    Network *network = join_network(dbs);
    
    // Latest block info allocations
    uchar *latest_block_hash = malloc(SHA256);
    uint latest_block_height;
    
    v_printf("Getting latest block info (pre-update)");
    latest_block_height = data_chain_get_height(dbs);
    ret = data_chain_get(dbs, latest_block_height, latest_block_hash);
    if (ret != 0)
        die("data_chain_get failed: %d", ret);

    v_printf("Updating blockchain and mempool...");
    msgall_getblocks(network, latest_block_hash, MAX_GETBLOCKS);
    msgall_mempool();
    sleep(10) // TODO may be excessive
    
    v_printf("Getting latest block info (post-update)");
    latest_block_height = data_chain_get_height(dbs);
    data_chain_get(dbs, latest_block_height, latest_block_hash);
    if (ret != 0)
        die("data_chain_get failed: %d", ret);

    // PRE-LOOP ALLOCATIONS
    uchar *workblock = malloc(MAX_BLOCK_SIZE);
    uchar *tx_buffer = malloc(MAX_TX_SIZE);                    // dequeue recieved transactions
    uchar *latest_block_hash_cmp_buffer = malloc(SIZE_SHA256); // compare latest block hash with block's prev_hash
    while(1)
    {
        printf("Starting a new block...\n");
        
        v_printf("Updating block info...");
        latest_block_height = data_chain_get_height();
        ret = data_chain_get(dbs, latest_block_height, latest_block_hash);
        if (ret != 0)
            die("data_chain_get failed: %d", ret);
        
        uint target = compute_next_target(dbs, latest_block_hash);

        v_printf("WORKBLOCK: Adding header...");
        void block_set_header(
                              __VERSION,
                              get_curr_time(),
                              (latest_block_height + 1),
                              latest_block_hash,
                              target,
                              workblock
                              );
         
        v_printf("WORKBLOCK: Adding coinbase transaction...");
        uchar *coinbase_output = malloc(SIZE_TX_OUT);
        r_tx_out_gen(miner_addr, MINING_REWARD, coinbase_output);
        Tx *coinbase_tx = m_tx_gen(0, 1, 0, NULL, &coinbase_output);
        block_add_tx(workblock, coinbase_tx);
        free(coinbase_output);
        
        while(1)
        {
            // Check if workblock is stale
            data_chain_get(dbs, data_chain_get_height(), latest_block_hash_cmp_buffer);
            if (memcmp(latest_block_hash_cmp_buffer, latest_block_hash, SIZE_SHA256) != 0) // TODO not very fast
                break;

            // Check for new mempool txs
            int diff = data_mempool_get_size(mempool) - (btoui(&workblock[POS_BLOCK_TX_COUNT]) - 1); // -1 for coinbase tx
            for(int i=0; i<diff; i++)
                block_add_tx(workblock, data_mempool_get(mempool, i));

            uitob(get_curr_time(), &workblock[POS_BLOCK_TIME]); // TODO We could also modify the coinbase output based on surplus of txs
            mine(workblock, target, 10000);
        }
    }
    free(latest_block_hash);
    free(workblock);
    free(tx_buffer);
    free(latest_block_hash_cmp_buffer);
    return 0;
}
