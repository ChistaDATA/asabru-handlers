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
        //    HTTP/1.1 200 OK
        //    Content-Type: text/plain

        //    Hello World\n

        std::cout<<"HandleData "<<endl;
        string response = "";
        response += "HTTP/1.1 200 OK\n";
        response+="Content-Type: text/plain\n";

        response+="Content-Length: 12";

        //        map<string, string> *headers = &((*httpMetadata).headers);
        //        for (auto i : *headers)
        //        {
        //            buffer += i.first + ":" + i.second + "\r\n";
        //        }
        //
        response += "\n";
        response += "Hello World\n";

        size_t response_length = strlen(response.c_str());
        char *result = (char *)malloc(response_length);
        memcpy(result, response.c_str(), response_length);

        return result;
    }

};
