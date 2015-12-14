#include "shared.h"
#include <stdlib.h>

#ifndef UTIL_H
#define UTIL_H

void hexstr_to_bytes(uchar *bytearr, char *string, size_t bytes);
void bytes_to_hexstr(char *string, uchar *bytearr, size_t bytes);

uint get_curr_time();

void uint_to_bytes(uint a, uchar *bytes);
uint bytes_to_uint(uchar *bytes);


uint bytes_to_uint(uchar *bytes);
ushort bytes_to_ushort(uchar *bytes);


#endif
