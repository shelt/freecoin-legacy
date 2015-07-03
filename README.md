# freecoin

**freecoin** is a lightweight cryptocurrency with a focus on comprehension. It is in an **early development stage** at the moment.

### What is a cryptocurrency?

A cryptocurrency is a currency that is functionally possible because of cryptography. Without the need for identification, a user is able to prove they were the recipient of a past transaction. This is only made possible because of math and our inability to factor very large numbers.

### What does it mean to be "decentralized"?

With banks that deal with regular currencies, all regulation happens in one place. Storing the amount of money each person has and the processing of transactions all happens on a few servers. Though they probably take backup and security very seriously, banks (and the governments that regulate them) represent a central authority over a vastly powerful resource. With decentralized currencies, transactions are processed and balances are stored by the people. Everyone agrees on how things should work, so it works. 

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
