#ifndef BLOCK_H
#define BLOCK_H

#include "shared.h"

void block_add_header(
                      uint time,
                      uint height,
                      uchar *prev_hash,
                      uchar target,
                      uchar *dest
                      );
void block_inc_nonce(uchar *block);
void block_update_root(uchar *block);
void block_add_tx(uchar *block, M_tx *tx);
uint block_compute_size(uchar *block);



void target_to_etarget(uint target, uchar *etarget);
uint etarget_to_target(uchar *etarget);
#endif
