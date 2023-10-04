#include "CServerSocket.h"
#include "CProxyHandler.h"
#include "CMySQLHandler.h"

extern "C" CMySQLHandler *createCMySQLHandler()
{
    return new CMySQLHandler;
}

extern "C" void destroyCHWirePTHandler(CMySQLHandler *c)
{
    delete c;
}