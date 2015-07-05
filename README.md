# freecoin

**freecoin** is a well-documented cryptocurrency that is easy to understand. It is in an **early development stage** at the moment.

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Mathematics and cryptographic principles allow a user is able to prove they were the recipient of a past transaction.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has and the processing of transactions all happens on a few servers. Though they probably take backup and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works. 

### Source guidelines
* Variables/parameters that refer to the count of an array should use size_t. Bytesizes should not. If you're smarter than me and you think they should, please let me know. NOTE: Block/transaction headers are fixed-size and as such are exempt from this.
* Memory allocation (i.e., blocks) for a function's output is always done via implementation. Pointers to the pre-allocated memory block are passed into the function.


### To-Do
* Blocks
* What is the input signature a signature of?
* Functions to build inputs and outputs (as structs)
* Input and output counts should be larger than 1 byte. more than 255 should be allowed
* Need to store the tx hash in the block header.
* Move all pointer astrixes to prefix form
* Fix crypto.c formatting
* Find memory leaks
* `unchar` should be used everywhere
* Remove all functional memory allocation.
* Move #includes that are relevant to the header to the header, and take them out of the source.
* Replace arrays of pointers with appropriate name (..._ptrs)
* Create functions to extract data from transactions
* Better documentation
* How to make different executables from a single makefile



## Rules **(outdated)**

#### Transaction validation
- `count(OUTs) < count(INs)`, unless it's a coinbase transaction in which the counts must be 1 and 0.
- For each `OUT`, if `out_amount > in_amount`, add change for that address (and sum after).
- For each `OUT`, `out_amount <= sum(all_cooresponding_in_amounts)`.
- Zero-amount outputs are forbidden.
- TXHash is valid.

#### Block validation **(outdated)**
- There is only one coinbase tx.
- All TXHashes are valid.
- Blockhash + nonce is valid.


#### Transaction process **(outdated)**

- get_transaction
- assert not in newblock
- assert not in blockchain
- assert inputs_useable
- assert inputs_signed
- assert for each out: (ins(with out_i) combined) >= out amount
 - if greater, return surplus back to sender in a new out

