#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "crypto.h"
#include "transactions.h"

/*  ******************* TRANSACTION *******************

          HEADER 10B                  INPUTS 290B             OUTPUTS 36B
    |----------------------| |----------------------------|  |-----------|
    0000 0000 0000 00000000  [32B] 0000      [128]  [128]..  [32B][4B]..
    |    |    |    |         |     |         |      |        |     |
    |    |    |    |         |     out_index |      |        |     amount
    |    |    |    |         |               pubkey sig      |
    |    |    |    lock_time |                               out_address
    |    |    |              |
    |    |    out_count      ref_tx
    |    |
    |    in_count
    |
    version

*/

/* 
    Creates a serialized transaction beginning at pointer tx
    (which is already allocated).
*/
void gen_tx(ushort in_count,
            ushort out_count,
            uint lock_time,
            // body
            uchar **ins,
            uchar **outs,
            uchar *tx)
{
    // These are computed here for use in for-loop tests. TODO
    //uint ins_size = in_count * SIZE_TX_INPUT;    // Side of ins combined
    //uint outs_size = out_count * SIZE_TX_OUTPUT; // Size of outs combined
    //uint size = SIZE_TX_HEADER + ins_size + outs_size; // Size of transaction
    
    ///////////////////// START HEADER /////////////////////
    ushort version = __VERSION;
    tx[0] = (version >> 8)  & 0xFF;
    tx[1] =  version        & 0xFF;
    tx[2] = (in_count >> 8) & 0xFF;
    tx[3] =  in_count       & 0xFF;
    tx[4] = (out_count >> 8)& 0xFF;
    tx[5] =  out_count      & 0xFF;
    
    tx[6] = (lock_time >> 24) & 0xFF;
    tx[7] = (lock_time >> 16) & 0xFF;
    tx[8] = (lock_time >> 8)  & 0xFF;
    tx[9] =  lock_time        & 0xFF;
    
    ////////////////////// END HEADER //////////////////////
    int tx_index = 10;
    int i; // Pointer index for this loop
    for(i=0; i<in_count; i++)
    {
         memcpy(&tx[tx_index], ins[i], SIZE_TX_INPUT);
         tx_index += SIZE_TX_INPUT;
    }

    for(i=0; i<out_count; i++)
    {
        memcpy(&tx[tx_index], outs[i], SIZE_TX_OUTPUT);
        tx_index += SIZE_TX_OUTPUT;
    }
}

/**************
** MODIFIERS **
***************/

ushort tx_get_size(uchar *tx)
{
    return ((tx[2] << 8) | tx[3]) * SIZE_TX_INPUT  + 
           ((tx[4] << 8) | tx[5]) * SIZE_TX_OUTPUT + SIZE_TX_HEADER;
};

/****************
 ** INS / OUTS **
 ****************/

void gen_tx_input(uchar *ref_tx, uint out_index, uchar *pubkey, uchar *sig, uchar *tx_input)
{
    memcpy(&tx_input[0], ref_tx, SIZE_SHA256);
    tx_input[SIZE_SHA256  ] = (out_index >> 24) & 0xFF;
    tx_input[SIZE_SHA256+1] = (out_index >> 16) & 0xFF;
    tx_input[SIZE_SHA256+2] = (out_index >> 8)  & 0xFF;
    tx_input[SIZE_SHA256+3] =  out_index        & 0xFF;
    
    memcpy(&tx_input[  SIZE_SHA256+4], sig, SIZE_RSA1024);
    memcpy(&tx_input[2*SIZE_SHA256+4], sig, SIZE_RSA1024);

}
void gen_tx_output(uchar *out_address, uint amount, uchar *tx_output)
{
    memcpy(&tx_output[0], out_address, SIZE_SHA256);
    
    tx_output[SIZE_SHA256  ] = (amount >> 24) & 0xFF;
    tx_output[SIZE_SHA256+1] = (amount >> 16) & 0xFF;
    tx_output[SIZE_SHA256+2] = (amount >>  8) & 0xFF;
    tx_output[SIZE_SHA256+3] =  amount & 0xFF;
}

/*******************
 ** MISCELLANEOUS **
 *******************/

