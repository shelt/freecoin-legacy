#include "shared.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>



uchar hexc_to_nibble(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else return 255;
}
char nibble_to_hexc(uchar n)
{
    if (n >= 0 && n <= 9)
        return n + '0';
    else
        return n + 'a' - 10;
}

// bytearr must have size bytes/2 and (bytes mod 2) of 0
void hexstr_to_bytes(uchar *bytearr, char *string, size_t bytes)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0; byte_pos<bytes; byte_pos++, str_pos+=2)
        bytearr[byte_pos] = (hexc_to_nibble(string[str_pos]) << 4) | (hexc_to_nibble(string[str_pos+1]));
}
// string must have size 2*bytes+1
void bytes_to_hexstr(char *string, uchar *bytearr, size_t bytes)
{
    int byte_pos, str_pos;
    for(byte_pos=0, str_pos=0; byte_pos<bytes; byte_pos++, str_pos+=2)
    {
        string[str_pos  ] = nibble_to_hexc(bytearr[byte_pos] >> 4);
        string[str_pos+1] = nibble_to_hexc(bytearr[byte_pos]);
    }
}



//TODO use these more prevelantly
// Big-endian
void uint_to_bytes(uint a, uchar *bytes)
{
    bytes[0] = a >> 24 & 0xFF;
    bytes[1] = a >> 16 & 0xFF;
    bytes[2] = a >> 8  & 0xFF;
    bytes[3] = a       & 0xFF;
}
// Big-endian
uint bytes_to_uint(uchar *bytes)
{
    return ntohl((uint)((bytes[0] << 24) |
                        (bytes[1] << 16) |
                        (bytes[2] << 8 ) |
                         bytes[3]));
}
// Big-endian
ushort bytes_to_ushort(uchar *bytes)
{
    return ntohs((ushort)((bytes[0] << 8) | bytes[1]));
}

uint get_curr_time()
{
    return (uint)time(NULL);
}




/* BYTE ORDER CONVERSION */

void htoleb(uchar *h, uchar *le)
{

}

void letohb(uchar *le, uchar *h)
{

}

void htobeb(uchar *h, uchar *be)
{

}

void betohb(uchar *be, uchar h)
{

}
