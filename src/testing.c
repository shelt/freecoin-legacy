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


unsigned char hexc_to_int(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return c - 'a' + 10;
    else return 255;
}

unsigned char* hexstr_to_bytes(char* string, size_t bytes)
{
    unsigned char* bytearr = malloc(bytes * sizeof(char*));
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0;   byte_pos<bytes;   byte_pos++ ,str_pos+=2)
        bytearr[byte_pos] = (hexc_to_int(string[str_pos]) << 4) | (hexc_to_int(string[str_pos+1]));
    return bytearr;
}

int main()
{
    // At the moment, these test values are arbitrary and are not real hashes of previous txs or blocks
    
    Header_tx* header = malloc(sizeof(Header_tx*));
    header->in_count = 1;
    header->out_count = 1;
    header->version = 1;
    header->time = 1435969063;
    
    unsigned char* ins[1];
    ins[0] = hexstr_to_bytes("4b252922a1581c8a6dfd4bba25dd43f3f6bac043b6707d82f471d259f47d1d618cb824e97ab63083e21876d9041b01008d70ad3e0a0c8b9a4fca5e4ffc26caf492a3bf1a85b8eff86ff0d7422d8eecdfdf9b008eac868ab34b9527e8059f2c018cb824e97ab63083e21876d9041b01008d70ad3e0a0c8b9a4fca5e4ffc26caf492a3bf1a85b8eff86ff0d7422d8eecdfdf9b008eac868ab34b9527e8059f2c01", TX_INPUT_BYTESIZE);
    
    unsigned char* outs[1];
    outs[0] = hexstr_to_bytes("8cb824e97ab63083e21876d9041b01008d70ad3e0a0c8b9a4fca5e4ffc26caf492a3bf1a85b8eff86ff0d7422d8eecdfdf9b008eac868ab34b9527e8059f2c018cb824e97ab63083e21876d9041b01008d70ad3e0a0c8b9a4fca5e4ffc26caf492a3bf1a85b8eff86ff0d7422d8eecdfdf9b008eac868ab34b9527e8059f2c0100000001", TX_OUTPUT_BYTESIZE);
    

    unchar* tx;
    generate_transaction(header, ins, outs, 1, 1, tx);
};

    //char* k = hexstr_to_bytes("1bff", 2);
//char* p = hexstr_to_bytes("1bff", 2);
//char** ins = malloc(2);
//ins[0] = k;
//ins[1] = p;
//printf("%x",ins[0]);
