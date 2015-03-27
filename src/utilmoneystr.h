




/**
 * Money parsing/formatting utilities.
 */
#ifndef FREECOIN_UTILMONEYSTR_H
#define FREECOIN_UTILMONEYSTR_H

#include <stdint.h>
#include <string>

#include "amount.h"

std::string FormatMoney(const CAmount& n, bool fPlus=false);
bool ParseMoney(const std::string& str, CAmount& nRet);
bool ParseMoney(const char* pszIn, CAmount& nRet);

#endif // FREECOIN_UTILMONEYSTR_H
