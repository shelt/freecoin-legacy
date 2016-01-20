#include "bignum.h"
#include <string.h>
// note that bignums are little endian (TODO change to big)
// TODO the rest of this file

void big_mult(uchar *a, uint a_bytes, uchar *b, uint b_bytes, uchar *res)
{
    int w = a_bytes + b_bytes;
    memset(res, 0x00, w);
    for (int j = 0; j < w; j++)
    {
        uint16_t carry = 0;
        uint16_t n;
        for (int i = 0; (i + j) < w; i++)
        {
            n = carry + res[i + j] + (uint16_t)a[j] * b[i];
            res[i + j] = n & 0xff;
            carry = n >> 8;
        }
    }
}

void big_div(uchar *a, uchar *b, uchar *res)
{
    
}

// Due to the nature of when this function is used,
// the lengths of a and b are assumed to be equal.
int big_compare(uchar *a, uchar *b, uint bytes)
{
    for(int i=0; i<bytes; i++)
    {
        if (a[i] < b[i])
            return -1;
        if (a[i] > b[i])
            return 1;
    }
    return 0;
}
