# freecoin

**freecoin** is a lightweight cryptocurrency implementation with a focus on portability and comprehension.

## Rules

#### Transaction validation
- `count(OUTs) < count(INs)`, unless it's a coinbase transaction in which the counts must be 1 and 0.
- For each `OUT`, if `out_amount > in_amount`, add change for that address (and sum after).
- For each `OUT`, `out_amount <= sum(all_cooresponding_in_amounts)`.
- Zero-amount outputs are forbidden.
- TXHash is valid.

#### Block validation
- There is only one coinbase tx.
- All TXHashes are valid.
- Blockhash + nonce is valid.


#### Transaction process

- get_transaction
- assert not in newblock
- assert not in blockchain
- assert inputs_useable
- assert inputs_signed
- assert for each out: (ins(with out_i) combined) >= out amount
 - if greater, return surplus back to sender in a new out



#### To-Do
0.0.1

* IRC transaction submission listening
* IRC block submission and listening
* Transaction adding after assertions
* Divide main.py into respective files
* Address generation
 * NOTE: public addresses are: hex(pubkey)[2:]
* Automatic target calculation
* Blockchain updating:
 * The blockchain file should be updated upon new block (via irc)
 * The old object should then be disbanded
 * New blockchain objects should verify their hashes
* **Detail other verification processes in README**
* **Reduce need for extensive verification (add implicit verification)**
