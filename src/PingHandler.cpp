#include "PingHandler.h"

extern "C" PingHandler *createPingHandler()
{
    return new PingHandler;
}

extern "C" void destroyPingHandler(PingHandler *c)
{
    delete c;
}

std::string PingHandler::HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) {
    return "PONG";
}
