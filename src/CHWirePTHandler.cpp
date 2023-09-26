#include "ServerSocket.h"
#include "CProxyHandler.h"
#include "CHWirePTHandler.h"

extern "C" CHWirePTHandler *createCHWirePTHandler()
{
    return new CHWirePTHandler;
}

extern "C" void destroyCHWirePTHandler(CHWirePTHandler *c)
{
    delete c;
}