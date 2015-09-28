#include "shared.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>


unchar hexc_to_int(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        return c - 'a' + 10;
    else return 255;
};

void hexstr_to_bytes(char *string, size_t bytes, unchar *bytearr)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0;   byte_pos<bytes;   byte_pos++ ,str_pos+=2)
        bytearr[byte_pos] = (hexc_to_int(string[str_pos]) << 4) | (hexc_to_int(string[str_pos+1]));
};

unint bytes_to_unint(char *bytes)
{
    return (bytes[0]<<24) + (bytes[1]<<16) + (bytes[2]<<8) +bytes[3];
}
unshort bytes_to_unshort(char *bytes)
{
    return (bytes[2]<<8) +bytes[3];
}

unint get_curr_time()
{
    return (unint)time(NULL);
}
