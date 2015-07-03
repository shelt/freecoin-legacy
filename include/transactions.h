#ifndef TRANS_H
#define TRANS_H

#include "shared.h"

typedef struct
{
    unsigned short size;
    unsigned short version;
    unsigned int time;
} Header_tx;

void generate_transaction(Header_tx* header, unchar ins[], unchar outs[], unint in_count, unint out_count, unchar* tx);

void generate_merkle_root(unchar* txs[], unsigned int tx_count, unchar hash[]);


#endif