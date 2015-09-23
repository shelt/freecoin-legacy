#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "shared.h"
#include "printing.h"
// todo validation.c
// passing a point in an array as a ptr looks like &array[x]
// understand importance of stack vars and pass by value; no memory leaks occur

int main(int argc, char **argv)
{
    print_greeting();

    if (argc==1)
        printf("\nType -h for help.\n");
    else
    {
        
    }
    return 0;
}

/*
    Called whilst already connected to peers
*/
void mine(unchar *outs, unshort out_count, unint lock_time)
{
    //unchar *coinbase_tx = malloc((out_count * TX_OUT_SIZE) + TX_HEADER_SIZE);
    //   gen_tx(__VERSION, 0, out_count, lock_time, NULL, outs, coinbase_tx);
    
    unshort coinbase_tx_size = (out_count * TX_OUT_SIZE) + TX_HEADER_SIZE;
    unshort mempool_count = get_curr_mempool_count();
    unshort txs_size = coinbase_tx_size; // Will be incremented
    for (int i; i<mempool_count
    unchar *txs = malloc(get_curr_mempool_size() + coinbase_tx_size);
    get_curr_mempool(&txs[coinbase_tx_size]);


    unchar *block = malloc(MAX_BLOCKSIZE);
    gen_work_block(__VERSION, get_net_time(), get_latest_block(), txs,
        unshort tx_count, unchar *block)
    gen_tx(__VERSION, 0, out_count, lock_time, NULL, outs, &block[77]); // Block body starts at 77th byte
    
    // Do work
    // TODO state of hash state so only the second chunk needs to be computed
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    unchar *hash = malloc(SHA256_SIZE);
    while(1)
    {
        sha256_init(ctx);
        sha256_update(ctx, block, BLOCK_HEADER_SIZE);
        sha256_final(ctx, hash);

        if (hash[0] == 0x00) // Has some zero bits
        for i 
            
    }
};
