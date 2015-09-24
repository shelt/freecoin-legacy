#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "shared.h"
#include "printing.h"
#include "transactions.h"
#include "freecoin.h"
#include "blocks.h"
#include "util.h"
#include "crypto.h"

// todo validation.c
// passing a point in an array as a ptr looks like &array[x]
// understand importance of stack vars and pass by value; no memory leaks occur



int is_valid_blockhash(unchar *hash, unchar target)
{
    for (int i=0; i<target; i++)
        if (!(hash[i] == 0x00))
            return 0;
    return 1;
};

void succeed(unchar *block)
{
    // clear mempool
    // TODO
};

/*
    Called whilst already connected to peers
*/
void mine(unchar **outs, unshort out_count, unint lock_time, unchar **mempool, unint mempool_count)
{
    unchar curr_target = get_curr_target();
    unint nonce = 0;    // This will probably be incremented
    unint tx_count = 1; // The coinbase transaction; This will be incremented
    
    // Initialize block
    unchar *block = malloc(MAX_BLOCKSIZE);
    init_block(curr_target, block);
    unint body_cursor = BLOCK_HEADER_SIZE + 1;

    // Add coinbase transaction
    gen_tx(0, out_count, lock_time, NULL, outs, &block[77]); // Block body starts at 77th byte
    body_cursor += (out_count * TX_OUT_SIZE) + TX_HEADER_SIZE;

    // Add mempool transactions
    for (int i; i<mempool_count; i++)
    {
        unchar *tx = mempool[i];
        unint tx_size = get_tx_size(tx);
        memcpy(block, tx, tx_size);

        body_cursor += tx_size;
        tx_count++;
    }
    
    // Finally
    update_block_merkle_root(block, tx_count);
    update_block_tx_count(block, tx_count);

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
            if (is_valid_blockhash(hash, curr_target))
            {
                succeed(block);
                break;
            }
        update_block_nonce(block, nonce++);
    }
};





int main(int argc, char **argv)
{
    print_greeting();

    if (argc==1)
        printf("\nType -h for help.\n");
    else
    {
        // Out transactions parsing
        unchar *outs[1024*1024]; // Arbitrarily large (for now)
        char out_address_string[RSA1024_SIZE];
        unchar *out_address_bytes = malloc(RSA1024_SIZE);
        unint out_count = 0;
        while(fgets(out_address_string, RSA1024_SIZE, stdin) != NULL)
        {

            fgets(out_address_string, RSA1024_SIZE, stdin);
            hexstr_to_bytes(out_address_string, RSA1024_SIZE, out_address_bytes);

            unchar *coinbase_output = malloc(TX_OUT_SIZE);
            gen_tx_output(out_address_bytes, MINING_REWARD, coinbase_output);
            outs[out_count++] = coinbase_output;
        }//todo handle no input
        // connect to peers
        // mempool
        unchar **mempool;//todo
        unint mempool_count; //todo
        // parse outputs and lock and stuff
        while(1)
        {
            mine(outs, out_count, 0, mempool, mempool_count);
            // call mine again but with new latest block and such
        }
    }
    return 0;
}


