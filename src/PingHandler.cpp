#include "PingHandler.h"
#include "CServerSocket.h"



extern "C" PingHandler *createPingHandler()
{
    return new PingHandler;
}

extern "C" void destroyPingHandler(PingHandler *c)
{
    delete c;
}
