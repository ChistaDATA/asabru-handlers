#pragma once

#include "CServerSocket.h"
#include "CProtocolHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"
#include "Utils.h"
#include <map>

class BaseProtocolHandler : public CProtocolHandler
{
public:
    /**
     * Function that handles upstream data
     * @param buffer - the buffer that we receive from upstream ( source dbs )
     * @param length - length of the buffer
     */
    std::string HandleData(void *buffer, int buffer_length, EXECUTION_CONTEXT *exec_context)
    {
        // Respond with a simple "Hello, World!" message
        // std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!";

        string response = "";
        response += "HTTP/1.1 200 OK\r\n";
        response+="Content-Length: 12\r\n";
        response += "\r\n";
        response += "Hello World!";

        return response;
    }

};
