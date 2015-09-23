#ifndef TRANS_H
#define TRANS_H
#include "shared.h"
#include <stdlib.h>


void gen_tx(unshort version, unshort in_count, unshort out_count, unint time, // Header variables
                            unchar **ins, unchar **outs, unchar *tx);                       // Body variables
void gen_merkle_root(unchar **txs, unshort tx_count, unchar *hash);

void gen_tx_input(unchar *ref_tx, unchar *sig, unchar *tx_input);

void gen_tx_output(unchar *out_address, unint amount, unchar *tx_output);

unchar get_tx_size(unchar *tx);

#endif
