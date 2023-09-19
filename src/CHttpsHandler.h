#pragma  once

#include "ServerSocket.h"
#include "CProxyHandler.h"

pair<string, string> ChopLine(string str);

class CHttpsHandler : public CProxyHandler {
public:
    CHttpsHandler();
    virtual void * HandleUpstreamData(void * buffer, int buffer_length, uv_stream_t * target);
    virtual void * HandleDownStreamData(void * buffer, int buffer_length, uv_stream_t *client);

    void LogResponse(char * buffer, int len);
};
