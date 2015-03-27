




#ifndef FREECOIN_VALIDATIONINTERFACE_H
#define FREECOIN_VALIDATIONINTERFACE_H

#include <boost/signals2/signal.hpp>

class CBlock;
class CBlockLocator;
class CTransaction;
class CValidationInterface;
class CValidationState;
class uint256;

// These functions dispatch to one or all registered wallets

/** Register a wallet to receive updates from core */
void RegisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister a wallet from core */
void UnregisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister all wallets from core */
void UnregisterAllValidationInterfaces();
/** Push an updated transaction to all registered wallets */
void SyncWithWallets(const CTransaction& tx, const CBlock* pblock = NULL);

class CValidationInterface {
protected:
    virtual void SyncTransaction(const CTransaction &tx, const CBlock *pblock) {};
    virtual void EraseFromWallet(const uint256 &hash) {};
    virtual void SetBestChain(const CBlockLocator &locator) {};
    virtual void UpdatedTransaction(const uint256 &hash) {};
    virtual void Inventory(const uint256 &hash) {};
    virtual void ResendWalletTransactions() {};
    virtual void BlockChecked(const CBlock&, const CValidationState&) {};
    friend void ::RegisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterAllValidationInterfaces();
};

struct CMainSignals {
    /** Notifies listeners of updated transaction data (transaction, and optionally the block it is found in. */
    boost::signals2::signal<void (const CTransaction &, const CBlock *)> SyncTransaction;
    /** Notifies listeners of an erased transaction (currently disabled, requires transaction replacement). */
    boost::signals2::signal<void (const uint256 &)> EraseTransaction;
    /** Notifies listeners of an updated transaction without new data (for now: a coinbase potentially becoming visible). */
    boost::signals2::signal<void (const uint256 &)> UpdatedTransaction;
    /** Notifies listeners of a new active block chain. */
    boost::signals2::signal<void (const CBlockLocator &)> SetBestChain;
    /** Notifies listeners about an inventory item being seen on the network. */
    boost::signals2::signal<void (const uint256 &)> Inventory;
    /** Tells listeners to broadcast their data. */
    boost::signals2::signal<void ()> Broadcast;
    /** Notifies listeners of a block validation result */
    boost::signals2::signal<void (const CBlock&, const CValidationState&)> BlockChecked;
};

CMainSignals& GetMainSignals();

#endif // FREECOIN_VALIDATIONINTERFACE_H
