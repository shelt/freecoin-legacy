




#ifndef FREECOIN_INIT_H
#define FREECOIN_INIT_H

#include <string>

class CWallet;

namespace boost
{
class thread_group;
} // namespace boost

extern CWallet* pwalletMain;

void StartShutdown();
bool ShutdownRequested();
void Shutdown();
bool AppInit2(boost::thread_group& threadGroup);

/** The help message mode determines what help message to show */
enum HelpMessageMode {
    HMM_FREECOIND,
    HMM_FREECOIN_QT
};

/** Help for options shared between UI and daemon (for -help) */
std::string HelpMessage(HelpMessageMode mode);
/** Returns licensing information (for -version) */
std::string LicenseInfo();

#endif // FREECOIN_INIT_H
