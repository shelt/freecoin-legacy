#include "shared.h"
#include "transactions.h"
#include "crypto.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define unchar unsigned char
#define unint unsigned int

// TODO replace length attribs with size_t type
// Replace for loops with declaration outside forloop to be non C99 compliant
// What is the input signature a signature of?
// functions to build inputs and outputs
// input and output counts should be larger than 1 byte. more than 255 should be allowed
// we need to store the tx hash in the block header.
// move all pointer astrixes to prefix form
// fix crypto.c formatting
// find memory leaks with malloc
// unchar should be used everywhere
// remove all functional memory allocation.
// Move #includes that are relevant to the header to the header, and take them out of the source.
// replace arrays of pointers with appropriate name (_ptrs)

// create functions to extract data from transactions


unsigned char hexc_to_int(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return c - 'a' + 10;
    else return 255;
}

void hexstr_to_bytes(unchar* string, size_t bytes, unchar* bytearr)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0;   byte_pos<bytes;   byte_pos++ ,str_pos+=2)
        bytearr[byte_pos] = (hexc_to_int(string[str_pos]) << 4) | (hexc_to_int(string[str_pos+1]));
}

// At the moment, test values are arbitrary and are not real hashes of previous txs or blocks
int main()
{
    //TRANSACTION GENERATION TESTING ////////////////////////////////////////////////////////////
    Header_tx* header = malloc(sizeof(Header_tx*));
    header->in_count = 1;
    header->out_count = 1;
    header->version = 1;
    header->time = 1435969063;
    
    // the following few things could be done in a loop in implementation for all the ins and outs
    // Initialize xput pointer arrays
    unchar* ins[1];
    unchar* outs[1];
    
    // Allocating xput bytes
    ins[0] = malloc(TX_INPUT_BYTESIZE);
    outs[0] = malloc(TX_OUTPUT_BYTESIZE);
    
    hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", TX_INPUT_BYTESIZE, ins[0]);
    
    hexstr_to_bytes("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb00000001", TX_OUTPUT_BYTESIZE, outs[0]);
    

    unchar* tx0 = malloc(TX_HEADER_SIZE + 1*TX_INPUT_BYTESIZE + 1*TX_OUTPUT_BYTESIZE);
    generate_transaction(header, ins, outs, 1, 1, tx0);
    
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

    unchar* txs[6] = {tx0,tx1,tx2,tx3,tx4,tx5};
    
    generate_merkle_root(txs, 6, hash);
    
    int i;
    for(i=0; i<size; i++)
        printf("%02x", tx1[i]);
    return 0;
};
