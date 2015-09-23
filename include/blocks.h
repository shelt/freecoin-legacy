#ifndef BLOCK_H
#define BLOCK_H

void generate_block(unshort version, unint time, unchar *prev_block_hash, // Header
                    unchar *merkle_root, unint target, unint nonce,       // Header
                    unchar **txs, unshort tx_count);                      // Body

void block_inc_nonce(unchar *block);
#endif
