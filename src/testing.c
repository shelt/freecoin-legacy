#include "transactions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// TODO replace length attribs with size_t type


char hexc_to_int(const char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if(c >= 'a' && c <= 'f')
        
        return c - 'a' + 10;
    else
        return 255;
}

char* hexstr_to_bytes(char* string)
{
    
}

/* Convert a string of characters representing a hex buffer into a series of bytes of that real value */
uint8_t* hexStringToBytes(char* string)
{
	uint8_t *retval;
	uint8_t *p;
	int len, i;
	
    len = strlen(string) / 2;
	retval = malloc(len+1);
	for(i=0, p = (uint8_t *) string; i<len; i++)
    {
		retval[i] = (strtoc(*p) << 4) | strtoc(*(p+1));
		p += 2;
	}
    retval[len] = 0;
	return retval;
}








int main()
{
    //unsigned char* hashes[2];
    
    //char k = 0xAA;
    //hashes[0] = &k;
    
    //printf("%x",*(hashes[0]));
    //exit(1);
    
    Header_tx header;
    //...todo populate header
    
    
    uint8_t* a = hexStringToBytes("1bb6509d3a1c5dd2a39c4e48b372b86c95eec9dc083654431aa71ea0bc0767b8");
    //char* a = strtob("1bb6509d3a1c5dd2a39c4e48b372b86c95eec9dc083654431aa71ea0bc0767b8", 32);
    
    printf("%x %x %x %x", a[0],a[1],a[2],a[3]);
    
    /*unchar* ins[1];
    ins[0] = malloc(256);
    memcpy(ins[1], 0x1bb6509d3a1c5dd2a39c4e48b372b86c95eec9dc083654431aa71ea0bc0767b8, 256);
    generate_transaction(&header, unchar ins[], unchar outs[], unint in_count, unint out_count, unchar* tx);
    */
    };
    
    
