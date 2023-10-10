#include "CHttpProtocolHandler.h"
#include "CServerSocket.h"

extern "C" CHttpProtocolHandler *createCHttpProtocolHandler()
{
    return new CHttpProtocolHandler;
}

extern "C" void destroyCHttpProtocolHandler(CHttpProtocolHandler *c)
{
    delete c;
}

