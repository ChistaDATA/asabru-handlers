#pragma once

#include "CProxyHandler.h"
#include "CHttpParser.h"
#include "BaseHandler.h"
#include "Utils.h"

class CHttpHandler : public BaseHandler {
    CHttpParser *parser = 0;
public:
    CHttpHandler();
    // CHttpHandler(CHttpParser *parser);
    void LogResponse(char * buffer, int len);
};