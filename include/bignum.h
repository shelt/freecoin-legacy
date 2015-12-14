#ifndef BIGNUM_H
#define BIGNUM_H

#include "shared.h"

void big_mult(uchar *a, uint a_bytes, uchar *b, uint b_bytes, uchar *res);

void big_div(uchar *a, uchar *b, uchar *res);

int big_compare(uchar *a, uchar *b, uint bytes);


#endif
