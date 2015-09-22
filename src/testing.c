#include "shared.h"
#include "transactions.h"
#include "crypto.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

unsigned char hexc_to_int(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return c - 'a' + 10;
    else return 255;
}

void hexstr_to_bytes(char *string, size_t bytes, unchar *bytearr)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0;   byte_pos<bytes;   byte_pos++ ,str_pos+=2)
        bytearr[byte_pos] = (hexc_to_int(string[str_pos]) << 4) | (hexc_to_int(string[str_pos+1]));
}

// At the moment, test values are arbitrary and are not real hashes of previous txs or blocks
int main(void)
{
    //TRANSACTION GENERATION TESTING ////////////////////////////////////////////////////////////
    unshort version = __VERSION;
    size_t in_count = 1;
    size_t out_count = 1;
    unint time = 1435969063;
    
    // the following few things could be done in a loop in implementation for all the ins and outs
    unchar *ins[1];
    unchar *outs[1];
    
    
    ins[0] = malloc(TX_INPUT_BYTESIZE);
    outs[0] = malloc(TX_OUTPUT_BYTESIZE);
    
    ///// INPUT GENERATING /////
    unchar *ref_tx = malloc(SHA256_SIZE);
    hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", SHA256_SIZE, ref_tx);
    
    unchar *sig = malloc(RSA1024_SIZE);
    hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", RSA1024_SIZE, sig);
    
    
    generate_tx_input(ref_tx, sig, ins[0]);
    

    
    ///// OUTPUT GENERATING /////
    unchar *out_address = malloc(RSA1024_SIZE);
    hexstr_to_bytes("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", RSA1024_SIZE, out_address);
    unint amount = 0x00000001;
    generate_tx_output(out_address, amount, outs[0]);
    
    unchar *tx0 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    generate_transaction(version, in_count, out_count, time, ins, outs, tx0);

    
    // MERKLE ROOT TESTING ///////////////////////////////////////////////////////////////////////
    size_t size = (TX_HEADER_SIZE + TX_INPUT_BYTESIZE + TX_OUTPUT_BYTESIZE); // This is only computable like this because we know there's only 1 in and 1 out.
    unchar* hash = malloc(SHA256_SIZE);
    
    unchar *tx1 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    unchar *tx2 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    unchar *tx3 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    unchar *tx4 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    unchar *tx5 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    
    tx1=memcpy(tx1, tx0, size);
    tx2=memcpy(tx2, tx1, size);
    tx3=memcpy(tx3, tx2, size);
    tx4=memcpy(tx4, tx3, size);
    tx5=memcpy(tx5, tx4, size);

    unchar *txs[6] = {tx0,tx1,tx2,tx3,tx4,tx5};
    
    generate_merkle_root(txs, 6, hash);
    
    printf("Transaction:            ");
    int i;
    for(i=0;i<size;i++)
        printf("%02x",tx0[i]);
    printf("\n");
    
    printf("Merkle root: ");
    //int i;
    for(i=0; i<SHA256_SIZE; i++)
        printf("%02x", hash[i]);
    return 0;
};
