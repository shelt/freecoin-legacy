//TODO rem: bit shifting loads bytes into register, equiv of converting to big-endian

// This file has to do with serialized blocks.

/*
    The act of using a pointer parameter for returning results
    is useful for when the result is serialized into already-allocated
    memory.
    Functions that generate data
*/

// TODO perform checking as to not exceed MAX_BLOCK_SIZE

/*  ********************** BLOCK **********************
    Possible size: 83B to 1MB (max size)

              HEADER 83B                         BODY
    |-----USED IN BLOCKHASH ---------------|
    |--------------------------------------|     |--|
    0000 [4B] [4B] [32B] [32B] 00 [4B] [4B]      [?B]
    |    |    |   |     |     |   |     |        |
    |    |    |   |     |     |   |     tx_count |
    |    |    |   |     |     |   |              |
    |    |    |   |     |     |   nonce          txs
    |    |    |   |     |     |
    |    |    |   |     |     target
    |    |    |   |     |
    |    |    |   |     merkle_root     
    |    |    |   |
    |    |    |   prev_hash           
    |    |    |
    |    |    height
    |    |
    |    time
    |
    version
*/

void block_add_header(
                      uint time,
                      uint height,
                      uchar *prev_hash,
                      uchar target,
                      uchar *dest
                      )
{
    ustob(__VERSION__, &dest[POS_BLOCK_VERSION]);
    uitob(time       , &dest[POS_BLOCK_TIME]);
    uitob(height     , &dest[POS_BLOCK_HEIGHT);
    
    memcpy(&dest[POS_BLOCK_PREV_HASH], prev_hash, SIZE_SHA256);
    memset(&dest[POS_BLOCK_MERKLE_ROOT], 0, SIZE_SHA256);
    
    dest[POS_BLOCK_TARGET] = target;
    
    uitob(0, &dest[POS_BLOCK_NONCE]);
    uitob(0, &dest[POS_BLOCK_TX_COUNT]);
}

// TODO: This needs to be as fast as possible.
void block_inc_nonce(uchar *block)
{
    uint nonce = btoui(&block[POS_BLOCK_NONCE]);
    uitob(++nonce, &block[POS_BLOCK_NONCE]);
}

void block_update_root(uchar *block)
{
    compute_merkle_root(
                        &block[POS_BLOCK_BODY],
                        btoui(&dest[POS_BLOCK_TX_COUNT]),
                        &block[POS_BLOCK_MERKLE_ROOT]
                        );
}

void block_add_tx(uchar *block, M_tx *tx)
{
    uint size = block_compute_size(block);
    if ((size + tx->size) >= MAX_BLOCK_SIZE)
        fatal("Block overflow during block_add_tx");
    memcpy(&block[size], tx->data, tx->size);
    uint tx_count = btoui(&block[POS_BLOCK_TX_COUNT]);
    uitob(++tx_count, &block[POS_BLOCK_TX_COUNT]);
}
    
uint block_compute_size(uchar *block)
{
    uchar tx_count = &dest[POS_BLOCK_TX_COUNT];
    uint cursor = SIZE_BLOCK_HEADER;
    for(int i=0; i<tx_count; i++)
        cursor += tx_compute_size(&block[cursor]);
    return cursor;
}
