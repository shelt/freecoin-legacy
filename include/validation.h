#ifndef VALIDATION_H
#define VALIDATION_H

//TODO
    // chain has no gaps
    // chain height == blocks count
    // all block heights in chain point to blocks listing identical block heights
    // all txs are found in specified block heights


/*
    There is no valid_block() function. There is no
    way to be certain about the validity of a single block.
    We can only determine the validity of a chain.
*/

int valid_chain


/*
    Returns: 1 if a start_hash can be traced back to a block in the chain
    using blocks in limbo.
    Returns: 0 if the chain formed from start_hash never encounters the blockchain
    (or after REASONABLE_CONFIRMATIONS is passed, at which point it becomes
    infeasible that we will find a shared block anyway).
    If check_validity > 0, each block is checked for validity during traversal.
    Returns: -1 if check_validity > 0 and an invalid block was found.
*/
int valid_connectable_to_chain(Dbs *dbs, uchar *start_hash, uchar check_validity);

/*
    Traverses blocks starting at start_hash until a missing block is encountered.
    This missing block is stored into missing.
*/
int valid_compute_missing_block(Dbs *dbs, uchar *start_hash, *uchar missing);

#endif
