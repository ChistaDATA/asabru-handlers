#include "PingHandler.h"
#include "CServerSocket.h"

bool PingHandler::Handler(void *Buffer, int len, CLIENT_DATA &clientData)
{
    std::cout << "Pinged from Remote Server ....................." << std::endl;
    return true;
}

extern "C" PingHandler *createPingHandler()
{
    return new PingHandler;
}

extern "C" void destroyPingHandler(PingHandler *c)
{
    delete c;
}
