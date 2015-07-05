

/*  ********************** BLOCK **********************
    Total size: 10B+(160*input_count)+(132*output_count)

          HEADER 70B                      BODY
    |-----------------------|        |------------|
    0000 00000000 [32B] [32B]        [size_t] [?B]
    |    |        |     |            |        |
    |    |        |     merkle_root  |        Transactions
    |    |        |                  |
    |    |        prev_block_hash    tx_count
    |    |
    |    time
    |
    freecoin_version
*/

void generate_block(unchar version, unint time, unchar *prev_block_hash, *merkle_root, unint nonce, // Header variables
                    unchar** txs, size_t tx_count);                                                 // Body variables