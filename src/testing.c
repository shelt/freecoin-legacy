#include "shared.h"
#include "transactions.h"
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


unsigned char hexc_to_int(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return c - 'a' + 10;
    else return 255;
}

// TODO THIS SHOULD NOT ALLOCATE MEMORY. A POINTER TO ALLOCATED MEMORY SHOULD BE PASSED TO IT.
void hexstr_to_bytes(unchar* string, size_t bytes, unchar* bytearr)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0;   byte_pos<bytes;   byte_pos++ ,str_pos+=2)
        bytearr[byte_pos] = (hexc_to_int(string[str_pos]) << 4) | (hexc_to_int(string[str_pos+1]));
}

int main()
{
    // At the moment, these test values are arbitrary and are not real hashes of previous txs or blocks
    ///*
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
    

    unchar* tx;
    generate_transaction(header, ins, outs, 1, 1, tx);
    //*/
    
    /*
    unint* a = malloc(32);
    *a = hexstr_to_bytes("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 32);
    unint* hash = malloc(32);
    hash_transactions(a,a,hash);
    
    int i;
    for(i=0; i<32; i++)
        printf("%x", hash[i]);
        */
    
};

    //char* k = hexstr_to_bytes("1bff", 2);
//char* p = hexstr_to_bytes("1bff", 2);
//char** ins = malloc(2);
//ins[0] = k;
//ins[1] = p;
//printf("%x",ins[0]);
