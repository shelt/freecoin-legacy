#include <stdlib.h>
#include <stdio.h> //debuggery
#include <time.h>
#include "shared.h"
#include "coins.h"
#include "transactions.h"
// IN TERMS OF DATA STRUCTURES, BLOCKS DO NOT CONTAIN OR EVEN REFERENCE TRANSACTIONS. Only merkle roots.
// Typically, functions that generate blocks will "return" them via the last parameter.

// This file is seriously out of date.

struct _Block
{
    unsigned char hash[SHA_SIZE]; // SHA(SHA(version..prev..merkle)..time..nonce) (where merkle is SHA(txs))
    
    unsigned char  version;         // Freecoin version
    unsigned char* prev;            // SHA256 of previous block
    unsigned char* merkle;
    
    unsigned int   time;            // Start time
    unsigned long  nonce;           // Nonce used to generate hash
    
    unsigned int tx_count; // unused? todo
};

void block_init(Block* b, unsigned char* prev, Transaction* txs, unsigned int tx_count)
{
    b = malloc(sizeof(Block));
    b->version = __version;
    b->nonce = 0x00000000;
    b->prev = prev;
    b->tx_count = tx_count;
    // Compute total txs size UNUSED and if it will be move it to tx.c
    //int c = 0;
    //int i;
    //for(i=0; i<tx_count; i++)
    //    c += (INPUT_BYTESIZE * txs[i]->in_tx_count) + (OUTPUT_BYTESIZE * txs[i]->out_count);
    b->merkle = generate_merkle_root(txs);
    
};




