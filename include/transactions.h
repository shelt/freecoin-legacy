#ifndef TRANS_H
#define TRANS_H

#include <stdlib.h>
#include "shared.h"

typedef struct
{
    unsigned short size;
    unsigned short in_count;
    unsigned short out_count;
    unsigned short version;
    unsigned int time;
} Header_tx;

void generate_transaction(Header_tx* header, unchar** ins, unchar** outs, unint in_count, unint out_count, unchar* tx);

void generate_merkle_root(unchar** txs, size_t tx_count, unchar* hash);

void hash_objects(unchar* obj1, unchar* obj2, size_t size, unchar* hash);


#endif