#pragma once

#include "CProxyHandler.h"
#include "CHttpParser.h"

pair<string, string> ChopLine(string str);

class CHttpHandler : public CProxyHandler {
    CHttpParser *parser = 0;
public:
    CHttpHandler();
    // CHttpHandler(CHttpParser *parser);
    virtual void * HandleUpstreamData(void * buffer, int buffer_length, SocketClient *target);
    virtual void * HandleDownStreamData(void * buffer, int buffer_length, Socket *client);

    void LogResponse(char * buffer, int len);
};