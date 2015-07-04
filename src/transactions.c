#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "crypto.h"
#include "transactions.h"
#include "shared.h"

/*  ******************* TRANSACTION *******************

         HEADER 8B          INPUTS 160B    OUTPUTS 132B
    |------------------|  |------------|  |------------|
    00 00 00 00 00000000  [32B][128B]...  [128B][4B]...
    |  |  |  |  |         |    |          |     |
    |  |  |  |  time      |    signature  |     amount
    |  |  |  |            |               |
    |  |  |  version      input_tx        out_address
    |  |  |
    |  |  output_count
    |  |
    |  input_count
    |  
    0x00          

*/
void generate_transaction(Header_tx* header, unchar** ins, unchar** outs, unint in_count, unint out_count, unchar* tx)
{
    // These are computed here for use in for-loop end cases. TODO NONE OF THESE ARE USED !!!?!?!!?
    unint ins_size = in_count * TX_INPUT_BYTESIZE;    // Side of ins combined
    unint outs_size = out_count * TX_OUTPUT_BYTESIZE; // Size of outs combined
    unint size = TX_HEADER_SIZE + ins_size + outs_size; // Size of transaction
    
    tx = malloc(TX_HEADER_SIZE + ins_size + outs_size);
    tx[0] = 0x00;
    tx[1] = header->in_count;
    tx[2] = header->out_count;
    tx[3] = header->version;
    
    tx[4] = (header->time >> 24) & 0xFF;
    tx[5] = (header->time >> 16) & 0xFF;
    tx[6] = (header->time >> 8) & 0xFF;
    tx[7] = header->time & 0xFF;
    
    int tx_index = 8;
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
    
    //debuggery
    
    for(i=0; i<size; i++)
        printf("%02x", tx[i]);

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