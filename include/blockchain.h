#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "shared.h"

uint get_next_target(uchar *block_hash);
uint is_valid_blockhash(uchar *hash, uchar target);


#endif
