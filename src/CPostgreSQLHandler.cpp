#include "CServerSocket.h"
#include "CProxyHandler.h"
#include "CPostgreSQLHandler.h"

extern "C" CPostgreSQLHandler *createCPostgreSQLHandler()
{
    return new CPostgreSQLHandler;
}

extern "C" void destroyCPostgreSQLHandler(CPostgreSQLHandler *c)
{
    delete c;
}
