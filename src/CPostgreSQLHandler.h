#ifndef POSTGRESQL_PROTOCOL_HANDLER_H
#define POSTGRESQL_PROTOCOL_HANDLER_H

#include "ServerSocket.h"
#include "BaseHandler.h"

class CPostgreSQLHandler : public BaseHandler
{
public:
    CPostgreSQLHandler() {}
};

#endif