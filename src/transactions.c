#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "crypto.h"
#include "transactions.h"
#include "shared.h"




void generate_transaction(Header_tx* header, unchar ins[], unchar outs[], unint in_count, unint out_count, unchar* tx)
{
    // These are computed here for use in for-loop end cases.
    unint ins_size = in_count * TX_INPUT_BYTESIZE;    // Side of ins combined
    unint outs_size = out_count * TX_OUTPUT_BYTESIZE; // Size of outs combined
    unint size = TX_HEADER_SIZE + ins_size + outs_size; // Size of transaction
    
    tx = malloc(TX_HEADER_SIZE + ins_size + outs_size);

    tx[0] = (header->size >> 8) & 0xFF;
    tx[1] = header->size & 0xFF;
    
    tx[2] = (header->version >> 8) & 0xFF;
    tx[3] = header->version & 0xFF;
    
    tx[4] = (header->time >> 24) & 0xFF;
    tx[5] = (header->time >> 16) & 0xFF;
    tx[6] = (header->time >> 8) & 0xFF;
    tx[7] = header->time & 0xFF;
    
    {
        int c=0;
        for(int i=8; i<ins_size; i++,c++)
            tx[i] = ins[c];
    }
    {
        int c=0;
        for(int i=8+ins_size; i<size; i++,c++)
            tx[i] = outs[c];
    }
}

void generate_merkle_root(unchar* txs[], unsigned int tx_count, unchar hash[]) //Hash must be of size SHA_BYTESIZE
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    
    for(int i=0; i<tx_count; i++)
    {
        //sha256_update(&ctx, *(txs[i]),  (( *(txs[i]) << 8) | *(txs[i]))); // MAIN TODO HERE (see gcc errors)
    };
    
    sha256_final(&ctx, hash);
    printf(hash);//debuggery
};