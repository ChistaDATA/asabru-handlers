#pragma once

#include "BaseProxyHandler.h"
#include "CHttpParser.h"
#include "Utils.h"
#include "http/http_message.h"
#include "interface/CProxyHandler.h"
#include <map>

class CHttpHandler : public BaseProxyHandler
{
private:
    CHttpParser *parser = 0;

public:
    CHttpHandler()
    {
        parser = new CHttpParser();
    };

    /**
     * Function that handles upstream data
     * @param buffer - the buffer that we receive from upstream ( source dbs )
     * @param length - length of the buffer
     */
    std::string HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context);

    /**
     * Function that handles downstream data
     * @param buffer - the buffer / response that we receive from downstream ( target dbs )
     * @param length - length of the buffer
     */
    std::string HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context);
};