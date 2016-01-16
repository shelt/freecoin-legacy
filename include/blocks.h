#ifndef BLOCK_H
#define BLOCK_H

#include "shared.h"


#define POS_BLOCK_VERSION 0
#define POS_BLOCK_TIME 2
#define POS_BLOCK_HEIGHT 6
#define POS_BLOCK_PREV_HASH 10
#define POS_BLOCK_MERKLE_ROOT 42
#define POS_BLOCK_TARGET 74
#define POS_BLOCK_NONCE 75
#define POS_BLOCK_TX_COUNT 79
#define POS_BLOCK_BODY 83

#define SIZE_BLOCK_VERSION 2
#define SIZE_BLOCK_TIME 4
#define SIZE_BLOCK_HEIGHT 4
#define SIZE_BLOCK_PREV_HASH 32
#define SIZE_BLOCK_MERKLE_ROOT 32
#define SIZE_BLOCK_TARGET 1
#define SIZE_BLOCK_NONCE 4
#define SIZE_BLOCK_TX_COUNT 4

#define SIZE_BLOCK_HEADER 83



void block_init(ushort version,
                uint time,
                uint height,
                uchar *prev_hash,
                uchar target,
                                  uchar *block);

void block_inc_nonce(uchar *block);

void block_update_root(uchar *block);

uint block_get_uint(uchar *block, uint pos);

void block_add_tx(uchar *block, uchar *tx);

void compute_merkle_root(uchar *txs, uint tx_count, uchar *out);



void target_to_etarget(uint target, uchar *etarget);
uint etarget_to_target(uchar *etarget);
#endif
