#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "crypto.h"
#include "transactions.h"

/* transactions.c
    Contains functions dealing with transaction generation, reading and modification.
    Unlike blocks, entire transactions need to be passed around in large numbers and
    thus use a struct to retain information about their size. Once a block's body is
    verified, only the header (the size of which is fixed) needs to be passed around.
    //TODO convert all instances of transactions being passed around as raw bytes.
    // TODO allow inputs and outputs to be added ipso factor and the size and hash
    // recomputed. The entire tx would need to be re-allocated though.

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
    Creates a serialized transaction.
*/
Tx *m_tx_gen(ushort in_count,
             ushort out_count,
             uint lock_time,
             // body
             uchar **ins,
             uchar **outs)
{
    Tx *tx = malloc(sizeof(Tx));
    tx_buffer = malloc(MAX_TX_SIZE);

    ///////////////////// START HEADER /////////////////////
    ushort version = __VERSION;
    tx_buffer[0] = (version >> 8)  & 0xFF;
    tx_buffer[1] =  version        & 0xFF;
    tx_buffer[2] = (in_count >> 8) & 0xFF;
    tx_buffer[3] =  in_count       & 0xFF;
    tx_buffer[4] = (out_count >> 8)& 0xFF;
    tx_buffer[5] =  out_count      & 0xFF;
    
    tx_buffer[6] = (lock_time >> 24) & 0xFF;
    tx_buffer[7] = (lock_time >> 16) & 0xFF;
    tx_buffer[8] = (lock_time >> 8)  & 0xFF;
    tx_buffer[9] =  lock_time        & 0xFF;
    
    ////////////////////// END HEADER //////////////////////
    int tx_index = POS_TX_BODY;
    int i; // Pointer index for this loop
    for(i=0; i<in_count; i++)
    {
         memcpy(&tx_buffer[tx_index], ins[i], SIZE_TX_INPUT); //TODO check for MAX_TX_SIZE overflow
         tx_index += SIZE_TX_INPUT;
    }

    for(i=0; i<out_count; i++)
    {
        memcpy(&tx_buffer[tx_index], outs[i], SIZE_TX_OUTPUT); //TODO check for MAX_TX_SIZE overflow
        tx_index += SIZE_TX_OUTPUT;
    }

    // Size
    tx->size = tx_index;
    // Data
    tx->data = malloc(tx->size);
    memcpy(tx->data, tx_buffer, tx->size);
    
    // Hash
    tx->hash = malloc(SIZE_SHA256);
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    sha256_init(ctx);
    sha256_update(ctx, tx->data, tx->size);
    sha256_final(ctx, tx->hash);
    
    free(ctx);
    free(tx_buffer);
    
    return tx;
}

void m_tx_die(Tx *tx)
{
    free(tx->data);
    free(tx->hash);
    tx = NULL;
}

/**************
** MODIFIERS **
**************/

// This is now called tx_raw because typically a tx should be
// inside a Tx which retains size information.
ushort tx_raw_get_size(uchar *tx)
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

//NOTE: don't write a tx_compute_hash function, use a Tx struct!

