#ifndef TRANS_H
#define TRANS_H
#include "shared.h"
#include <stdlib.h>

#define POS_TX_VERSION 0
#define POS_TX_IN_COUNT 2
#define POS_TX_OUT_COUNT 4
#define POS_TX_LOCK_TIME 6
#define POS_TX_BODY 10

#define SIZE_TX_VERSION 2
#define SIZE_TX_IN_COUNT 2
#define SIZE_TX_OUT_COUNT 2
#define SIZE_TX_LOCK_TIME 4

#define SIZE_TX_HEADER 10
#define SIZE_TX_INPUT 290
#define SIZE_TX_OUTPUT 36

#define POS_TX_INPUT_REF_TX 0
#define POS_TX_INPUT_OUT_INDEX 32
#define POS_TX_INPUT_PUBKEY 34
#define POS_TX_INPUT_SIG 162

#define SIZE_TX_INPUT_REF_TX 32
#define SIZE_TX_INPUT_OUT_INDEX 2
#define SIZE_TX_INPUT_PUBKEY 128
#define SIZE_TX_INPUT_SIG 128

#define POS_TX_OUTPUT_OUT_ADDRESS 0
#define POS_TX_OUTPUT_AMOUNT 32

#define SIZE_TX_OUTPUT_OUT_ADDRESS 32
#define SIZE_TX_OUTPUT_AMOUNT 4



void gen_tx(ushort in_count, ushort out_count, uint lock_time, // Header variables
            uchar **ins, uchar **outs, uchar *tx);

void gen_tx_input(uchar *ref_tx, uint out_index, uchar *pubkey, uchar *sig, uchar *tx_input);

void gen_tx_output(uchar *out_address, uint amount, uchar *tx_output);

ushort tx_get_size(uchar *tx);

#endif
