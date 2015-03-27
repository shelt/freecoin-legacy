




#ifndef FREECOIN_ECCRYPTOVERIFY_H
#define FREECOIN_ECCRYPTOVERIFY_H

#include <vector>
#include <cstdlib>

class uint256;

namespace eccrypto {

bool Check(const unsigned char *vch);
bool CheckSignatureElement(const unsigned char *vch, int len, bool half);

} // eccrypto namespace

#endif // FREECOIN_ECCRYPTOVERIFY_H
