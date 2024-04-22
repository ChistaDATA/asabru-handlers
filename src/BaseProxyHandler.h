#pragma once

#include "socket/CServerSocket.h"
#include "interface/CProxyHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"
#include "Utils.h"
#include <map>
#include "logger/Logger.h"

class BaseProxyHandler : public CProxyHandler
{
    public:
        /**
         * Function that handles upstream data
         * @param buffer - the buffer that we receive from upstream ( source dbs )
         * @param length - length of the buffer
         */
        virtual std::string HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
        {
            LOG_INFO("Received a Client packet..................... ");
            LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
            LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );

            return buffer;
        }

        /**
         * Function that handles downstream data
         * @param buffer - the buffer / response that we receive from downstream ( target dbs )
         * @param length - length of the buffer
         */
        virtual std::string HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
        {
            LOG_INFO("Received a Server packet..................... ");
            LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
            LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );

            return buffer;
        }

        void LogResponse(char *buffer, int len)
        {
            LineGrabber ln(buffer, len);
            std::string first_line = ln.getNextLine();
            char first_line2[1024];
            ::strcpy(first_line2, first_line.c_str());
            char method[4096];
            char url[4096];
            char protocol[4096];
            sscanf(first_line2, "%s %s %s", method, url, protocol);
            LOG_INFO("================================================================================");
            LOG_INFO("SERVER RESPONSE");
            LOG_INFO("--------------------------------------------------------------------------------");
            LOG_INFO("Http Method: " + std::string(method));
            LOG_INFO("URL: " + std::string(url));
            LOG_INFO("Protocol: " + std::string(protocol));
            LOG_INFO("--------------------------------------------------------------------------------");
            LOG_INFO("Headers");
            LOG_INFO("--------------------------------------------------------------------------------");
            std::map<std::string, std::string> header_map;
            while (!ln.isEof())
            {
                std::string test = ln.getNextLine();
                if (strlen(test.c_str()) == 0) // break if there is an empty line
                {
                    break;
                }
                std::pair<std::string, std::string> temp = Utils::ChopLine(test);
                header_map.insert(temp);
                std::cout << temp.first << ": " << temp.second << std::endl;
            }

            // Rest of the packet will be the body
            LOG_INFO("--------------------------------------------------------------------------------");
            LOG_INFO("Body");
            LOG_INFO("--------------------------------------------------------------------------------");
            std::string body;
            while (!ln.isEof())
            {
                if (!body.empty())
                {
                    body += CRLF;
                }
                body += ln.getNextLine();
            }
            LOG_INFO(body);
            LOG_INFO("================================================================================");
        }
};
