#ifndef TRANS_H
#define TRANS_H
#include "shared.h"
#include <stdlib.h>



struct _s_M_tx
{
    uchar *hash;
    uchar *data;
    uint size;
    uint max;
};

M_tx m_tx_gen(
              ushort in_count,
              ushort out_count,
              uint lock_time,
              // body
              uchar *ins,
              uchar *outs
              );
void m_tx_die(M_tx tx);
void r_tx_gen(
              ushort in_count,
              ushort out_count,
              uint lock_time,
              // body
              uchar *ins,
              uchar *outs,
              uchar *dest
              );
void r_tx_in_gen(
                 uchar *ref,
                 ushort index,
                 uchar *pubkey,
                 uchar *sig,
                 uchar *dest
                 );
void r_tx_out_gen(uchar *address, uint amount, uchar *dest);
void compute_merkle_root(uchar *txs, uint tx_count, uchar *out);

#endif
