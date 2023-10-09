#ifndef POSTGRESQL_PROTOCOL_HANDLER_H
#define POSTGRESQL_PROTOCOL_HANDLER_H

#include "CServerSocket.h"
#include "BaseProxyHandler.h"

class CPostgreSQLHandler : public BaseProxyHandler
{
public:
    CPostgreSQLHandler() {}
};

#endif