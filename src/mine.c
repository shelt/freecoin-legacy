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
#include "blockchain.h"
#include "network.h"

// todo validation.c
// passing a point in an array as a ptr looks like &array[x]
// understand importance of stack vars and pass by value; no memory leaks occur


void succeed(unchar *block, unchar *hash)
{
    printf("Valid block found!\n");
    for(int i=0; i<SHA256_SIZE; i++)
        printf("%02x", hash[i]);
    printf("\n\n");
    // clear mempool
    // TODO
};

/*
    Called whilst already connected to peers
*/
void mine(unchar **outs, unshort out_count, unint lock_time)
{
    unchar curr_target = get_curr_target();
    unint nonce = 0;    // This will probably be incremented
    unint tx_count = 0;       // This will be incremented
    
    // Initialize block
    unchar *workblock = malloc(MAX_BLOCKSIZE);
    init_block(curr_target, workblock);
    unint body_cursor = BLOCK_HEADER_SIZE + 1;

    // Add coinbase transaction
    gen_tx(0, out_count, lock_time, NULL, outs, &workblock[BLOCK_HEADER_SIZE]);
    body_cursor += (out_count * TX_OUT_SIZE) + TX_HEADER_SIZE;
    tx_count++;
    
    // Finally
    update_block_merkle_root(workblock, tx_count);
    update_block_tx_count(workblock, tx_count);

    // Do work
    // TODO state of hash state so only the second chunk needs to be computed
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    unchar *hash = malloc(SHA256_SIZE);
    while(1)
    {
        sha256_init(ctx);
        sha256_update(ctx, workblock, BLOCK_HEADER_SIZE);
        sha256_final(ctx, hash);

        if (hash[0] == 0x00) // Has some zero bits
            if (is_valid_blockhash(hash, curr_target))
            {
                succeed(workblock, hash);
                break;
            }
        
        update_block_nonce(workblock, nonce++);
    }
};





int main(int argc, char **argv)
{
    
    print_greeting();
    if (argc==1)
        die("No flags provided.\nType -h for help.");
    else
    {
        unchar **outs = malloc(1024*1024);      // Array of tx_outs; Arbitrarily large (for now)
        unint out_count = 0;                    // Count for tx_outs
        int param_i = 1;                        // Index of next argv parameter
        // TODO move this to its own parsing file and turn repetitive ifs into functions
        while (param_i < argc)
        {
            if (argv[param_i][0] == '-')
                if (strlen(argv[param_i]) > 1 && argv[param_i][1] == 'o') // Output: -o <addr>
                {
                    if (argc >= param_i && argv[param_i+1][0] != '-') // A next value exists
                    {
                        // Validation
                        if (strlen(argv[param_i+1]) != (RSA1024_SIZE*2)) // 2 hexs per byte
                            die("Address must be RSA1024_SIZE.");

                        //hexstr_to_bytes(&argv[param_i], RSA1024_SIZE, out_address_bytes);
                        param_i += 2;
                        
                        unchar *coinbase_output = malloc(TX_OUT_SIZE);
                        gen_tx_output(NULL, MINING_REWARD, coinbase_output);
                        hexstr_to_bytes(argv[param_i+1], RSA1024_SIZE, coinbase_output);
                        outs[out_count++] = coinbase_output;
                    }
                    else
                    {
                        printf("Usage: %s -o <address>",argv[0]);
                        die("No address provided.");
                    }
                }
                // TODO more flag options, lock
                else
                    die("Unknown flag.\nType -h for help.");
            else
                die("Non-flag argument.\nType -h for help.");
            
        }

        printf("Establishing self on network...\n");
        join_network();
        exit(0);//TODO temp

        while(1)
        {
            mine(outs, out_count, 0);
            // call mine again but with new latest block and such
        }
    }
    return 0;
}


