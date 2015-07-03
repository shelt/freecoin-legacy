#include "transactions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int main()
{
    //unsigned char* hashes[2];
    
    //char k = 0xAA;
    //hashes[0] = &k;
    
    //printf("%x",*(hashes[0]));
    //exit(1);
    
    Header_tx header;
    //...todo populate header
    
    unchar* ins[1];
    ins[1] = malloc(256);
    memcpy(ins[1], 0x1bb6509d3a1c5dd2a39c4e48b372b86c95eec9dc083654431aa71ea0bc0767b8, 256);
    generate_transaction(&header, unchar ins[], unchar outs[], unint in_count, unint out_count, unchar* tx);
};