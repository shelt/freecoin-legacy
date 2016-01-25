#include "shared.h"
#include <stdlib.h>

#ifndef UTIL_H
#define UTIL_H

uchar hexc_to_nibble(char c); //internal
char nibble_to_hexc(uchar n); //internal

void hexstr_to_bytes(uchar *bytearr, char *string, size_t bytes);
void bytes_to_hexstr(char *string, uchar *bytearr, size_t bytes);

uint get_curr_time();

uint btoui(uchar *bytes);
ushort btous(uchar *bytes);
void ustob(ushort a, uchar *dest);
void uitob(uint a, uchar *dest);

#endif
