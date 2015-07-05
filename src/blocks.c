

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