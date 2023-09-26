#pragma once

#include "ServerSocket.h"
#include "CProxyHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"
#include <map>

pair<string, string> ChopLine(string str);

class BaseHandler : public CProxyHandler
{
public:
    // BaseHandler();
    /**
     * Function that handles upstream data
     * @param buffer - the buffer that we receive from upstream ( source dbs )
     * @param length - length of the buffer
     */
    void *HandleUpstreamData(void *buffer, int buffer_length, SocketClient *target)
    {
        std::cout << "=============== CH https (up) ===================" << endl;
        std::cout << "Received a Client packet..................... " << endl;
        std::cout << "Length of Packet is " << buffer_length << endl;
        std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

        // target_socket->SendBytes((char *) buffer, length);
        // Data received from the client, forward it to the target server
        // uv_write_t write_req;
        // uv_buf_t write_buf = uv_buf_init((char *) buffer, length);
        // uv_write(&write_req, (uv_stream_t *) target, &write_buf, 1, NULL);

        target->SendBytes((char *)buffer, buffer_length);
    }

    /**
     * Function that handles downstream data
     * @param buffer - the buffer / response that we receive from downstream ( target dbs )
     * @param length - length of the buffer
     */
    void *HandleDownStreamData(void *buffer, int buffer_length, Socket *client)
    {

        // Log the Content and Forward the Data to the EndPoint
        std::cout << "================= CH https (down) =================" << endl;
        std::cout << "Received a Server packet..................... " << endl;
        std::cout << "Length of Packet is " << buffer_length << endl;
        std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

        // Data received from the target server, forward it to the client
        // uv_write_t write_req;
        // uv_buf_t write_buf = uv_buf_init((char *) buffer, buffer_length);
        // uv_write(&write_req, client, &write_buf, 1, NULL);

        client->SendBytes((char *)buffer, buffer_length);
    }

    void LogResponse(char *buffer, int len)
    {
        LineGrabber ln(buffer, len);
        string first_line = ln.getNextLine();
        char first_line2[1024];
        ::strcpy(first_line2, first_line.c_str());
        char method[4096];
        char url[4096];
        char protocol[4096];
        sscanf(first_line2, "%s %s %s", method, url, protocol);
        cout << "================================================================================" << endl;
        cout << "SERVER RESPONSE" << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "Http Method: " << method << endl;
        cout << "URL: " << url << endl;
        cout << "Protocol: " << protocol << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "Headers" << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
        map<string, string> header_map;
        while (!ln.isEof())
        {
            string test = ln.getNextLine();
            if (strlen(test.c_str()) == 0) // break if there is an empty line
            {
                break;
            }
            pair<string, string> temp = ChopLine(test);
            header_map.insert(temp);
            cout << temp.first << ": " << temp.second << endl;
        }

        // Rest of the packet will be the body
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "Body" << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
        string body;
        while (!ln.isEof())
        {
            if (!body.empty())
            {
                body += CRLF;
            }
            body += ln.getNextLine();
        }
        cout << body << endl;
        cout << "================================================================================" << endl;
    }
};
