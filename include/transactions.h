#ifndef TRANS_H
#define TRANS_H
#include "shared.h"
#include <stdlib.h>


void gen_tx(unshort in_count, unshort out_count, unint lock_time, // Header variables
            unchar **ins, unchar **outs, unchar *tx);

void gen_tx_input(unchar *ref_tx, unchar *sig, unchar *tx_input);

void gen_tx_output(unchar *out_address, unint amount, unchar *tx_output);

unshort get_tx_size(unchar *tx);

#endif
