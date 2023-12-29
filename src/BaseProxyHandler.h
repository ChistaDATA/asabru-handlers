#pragma once

#include "CServerSocket.h"
#include "CProxyHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"
#include "Utils.h"
#include <map>
class BaseProxyHandler : public CProxyHandler
{
    public:
        /**
         * Function that handles upstream data
         * @param buffer - the buffer that we receive from upstream ( source dbs )
         * @param length - length of the buffer
         */
        std::string HandleUpstreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
        {
            std::cout << "Received a Client packet..................... " << std::endl;
            std::cout << "Length of Packet is " << buffer_length << std::endl;
            std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << std::endl;

            std::string result;
            result.assign((char *) buffer, buffer_length); 
            return result;
        }

        /**
         * Function that handles downstream data
         * @param buffer - the buffer / response that we receive from downstream ( target dbs )
         * @param length - length of the buffer
         */
        std::string HandleDownStreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
        {
            std::cout << "Received a Server packet..................... " << std::endl;
            std::cout << "Length of Packet is " << buffer_length << std::endl;
            std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << std::endl;

            std::string result;
            result.assign((char *) buffer, buffer_length); 
            return result;
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
            std::cout << "================================================================================" << std::endl;
            std::cout << "SERVER RESPONSE" << std::endl;
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
            std::cout << "Http Method: " << method << std::endl;
            std::cout << "URL: " << url << std::endl;
            std::cout << "Protocol: " << protocol << std::endl;
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
            std::cout << "Headers" << std::endl;
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
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
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
            std::cout << "Body" << std::endl;
            std::cout << "--------------------------------------------------------------------------------" << std::endl;
            std::string body;
            while (!ln.isEof())
            {
                if (!body.empty())
                {
                    body += CRLF;
                }
                body += ln.getNextLine();
            }
            std::cout << body << std::endl;
            std::cout << "================================================================================" << std::endl;
        }
};
