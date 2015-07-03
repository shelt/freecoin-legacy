#include <time.h>
#include "crypto.h"

#define uchar unsigned char

struct _Header_tx
{
    unsigned short size;
    unsigned short version;
    unsigned int time;
};

void generate_transaction(Header_tx* header, uchar ins[], uchar outs[], uint in_count, uint out_count, uchar* tx)
{
    // These are computed here for use in for-loop end cases.
    uint ins_size = in_count * TX_INPUT_BYTESIZE;    // Side of ins combined
    uint outs_size = out_count * TX_OUTPUT_BYTESIZE; // Size of outs combined
    uint size = TX_HEADER_SIZE + in_size + out_size; // Size of transaction
    
    tx = malloc(TX_HEADER_SIZE + in_size + out_size);

    tx[0] = (header->size >> 8) & 0xFF;
    tx[1] = header->size & 0xFF;
    
    tx[2] = (header->version >> 8) & 0xFF;
    tx[3] = header->version & 0xFF;
    
    tx[4] = (header->time >> 24) & 0xFF;
    tx[5] = (header->time >> 16) & 0xFF;
    tx[6] = (header->time >> 8) & 0xFF;
    tx[7] = header->time & 0xFF;
    
 
    for(int i=8,int c=0; i<ins_size; i++,c++)
        tx[i] = ins[c];
    
    for(int i=8+ins_size,int c=0; i<size; i++,c++)
        tx[i] = outs[c];
}

void generate_merkle_root(uchar txs[], unsigned int tx_count, uchar hash[]) //Hash must be of size SHA_BYTESIZE
{
    SHA256_CTX ctx;
    sha256_init(ctx);
    
    for(int i=0; i<tx_count; i++)
    {
        sha256_update(ctx, txs[i],  (((txs[i][0]) << 8) | txs[i][1])); // MAIN TODO HERE (see gcc errors)
    };
    
    sha256_final(ctx, hash);
    printf(hash);//debuggery
};