#ifndef TRANS_H
#define TRANS_H

#include <stdlib.h>
#include "shared.h"

void generate_transaction(unshort version, unshort in_count, unshort out_count, unint time, // Header variables
                            unchar** ins, unchar** outs, unchar* tx);                       // Body variables
void generate_merkle_root(unchar** txs, size_t tx_count, unchar* hash);

#endif