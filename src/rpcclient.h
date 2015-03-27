




#ifndef FREECOIN_RPCCLIENT_H
#define FREECOIN_RPCCLIENT_H

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_writer_template.h"

json_spirit::Array RPCConvertValues(const std::string& strMethod, const std::vector<std::string>& strParams);

#endif // FREECOIN_RPCCLIENT_H
