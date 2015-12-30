#ifndef BLOCK_H
#define BLOCK_H

void init_block(unchar target, unchar *block);

void update_block_nonce(unchar *block, unint nonce);

void update_block_merkle_root(unchar *block, unint tx_count);

void update_block_tx_count(unchar *block, unint tx_count);
#endif
