#ifndef PING_HANDLER_DOT_H
#define PING_HANDLER_DOT_H

#include "protocol-handlers/BaseProtocolHandler.h"

class PingHandler : public BaseProtocolHandler {
public:
    PingHandler() {}
    std::string HandleData(std::string request_string, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) override;
    virtual ~PingHandler() {}
};

#endif