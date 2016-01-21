

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

M_tx m_tx_gen(
              ushort in_count,
              ushort out_count,
              uint lock_time,
              // body
              uchar *ins,
              uchar *outs
              )
{
    // Determine size
    uint size_ins = SIZE_TX_INPUT  * in_count;
    uint size_outs = SIZE_TX_OUTPUT * out_count
    uint size = SIZE_TX_HEADER + size_ins + size_outs;
    if (size > MAX_TX_SIZE)
        die("Tx overflow");
    
    // Create M_tx struct
    M_tx tx;
    tx.hash = malloc(SIZE_SHA256);
    tx.data = malloc(size);
    tx.size = size;
    tx.max = size;
    
    tx_gen(
           in_count,
           out_count,
           lock_time,
           ins,
           outs,
           dest
           );

    tx_compute_hash(tx.data, tx.hash);
    
    return tx;
}
void m_tx_die(M_tx tx)
{
    if (tx.data != NULL)
    {
        free(tx.data);
        tx.data = NULL;
    }
    if (tx.hash != NULL)
    {
        free(tx.hash);
        tx.hash = NULL;
    }
}
    
void r_tx_gen(
              ushort in_count,
              ushort out_count,
              uint lock_time,
              // body
              uchar *ins,
              uchar *outs,
              uchar *dest
              )
{
    // Add header
    ustob(__VERSION__, &dest[POS_TX_VERSION]);
    ustob(in_count,    &dest[POS_TX_IN_COUNT]);
    ustob(out_count,   &dest[POS_TX_OUT_COUNT]);
    uitob(lock_time,   &dest[POS_TX_LOCK_TIME]);
    
    // Add body
    memcpy(&dest[POS_TX_BODY], ins, size_ins);
    memcpy(&dest[POS_TX_BODY + size_ins], outs, size_outs);
}

void r_tx_in_gen(
                 uchar *ref,
                 ushort index,
                 uchar *pubkey,
                 uchar *sig,
                 uchar *dest
                 )
{
    memcpy(&dest[POS_TX_IN_REF, ref, SIZE_TX_IN_REF);
    ustob(index, &dest[POS_TX_IN_INDEX]);
    memcpy(&dest[POS_TX_IN_PUBKEY], pubkey, SIZE_TX_IN_PUBKEY);
    memcpy(&dest[POS_TX_IN_SIG], sig, SIZE_TX_IN_SIG);
}

void r_tx_out_gen(uchar *address, uint amount, uchar *dest)
{
    memcpy(&dest[POS_TX_OUT_ADDR], address, SIZE_TX_OUT_ADDR);
    uitob(amount, &dest[POS_TX_OUT_AMOUNT]);
}



/*******************
 ** MISCELLANEOUS **
 *******************/

/*
                   ------ e81287 -----
                   |                 |
           ----d1074c----          70a4e6
           |            |             |
       -8ebc6a-     -d5e414-      -89b77c-
       |      |     |      |      |      |
    2d7f4d 3407a8 5edf5a 65c356 89aa32 e3e69c
       |      |      |      |      |      |
    sometx sometx sometx sometx sometx sometx

    Accepts an array of pointers to transactions.
    Generates hashes of those transactions. TODO this is all invalid now
    Hashes those hashes in pairs. The results are TREE HASHES.
    If it's an odd number, the final one is hashed with itself.
    Process is repeated until one hash remains (the MERKLE ROOT).
*/
void compute_merkle_root(uchar *txs, uint tx_count, uchar *out)
{
    uchar *tree_hashes[tx_count];                // An array of pointers to leaves
    uchar *buffer = malloc(SIZE_SHA256);         // The buffer where SHA256s are generated.
    SHA256_CTX *ctx = malloc(sizeof(SHA256_CTX));
    
    // Allocate hashspace
    int i;
    for(i=0; i<tx_count; i++)
        tree_hashes[i] = malloc(SIZE_SHA256);
    
    // Generate base leaves to hashspace
    uint body_cursor = 0;
    for(i=0; i<tx_count; i++)
    {
        uchar *tx = &txs[body_cursor];
        uint tx_size = tx_get_size(tx);
        sha256_init(ctx);
        sha256_update(ctx, tx, tx_size);
        sha256_final(ctx, tree_hashes[i]);
        
        body_cursor += tx_size;
    }
    
    uint leaf_count = tx_count;
    uint remainder;
    uint pair_count;
    uint k;
    while(leaf_count > 1)
    {
        remainder = leaf_count % 2;
        pair_count = (leaf_count - remainder) / 2;
        
        for(k=0,i=0; k<pair_count; k++,i+=2)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[i]  , SIZE_SHA256);
            sha256_update(ctx, tree_hashes[i+1], SIZE_SHA256);
            sha256_final(ctx, buffer);
            
            // Swap buffer and destination ptrs
            uchar *temp = tree_hashes[k];
            tree_hashes[k] = buffer;
            buffer = temp;
        }
        if(remainder)
        {
            sha256_init(ctx);
            sha256_update(ctx, tree_hashes[leaf_count-1], SIZE_SHA256);
            sha256_update(ctx, tree_hashes[leaf_count-1], SIZE_SHA256);
            sha256_final(ctx, buffer);
            
            uchar *temp = tree_hashes[k];
            tree_hashes[k++] = buffer;
            buffer = temp;
        }
        leaf_count = k;
        
    }
    // Copy to merkle_root pointer
    memcpy(out, tree_hashes[0], SIZE_SHA256); 
    
    // Free memory
    free(buffer);
    free(ctx);
    for(i=0; i<tx_count; i++) 
        free(tree_hashes[i]);
}
