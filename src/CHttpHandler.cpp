
#include <map>
#include "Utils.h"
#include "CHttpHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"

// CHttpHandler::CHttpHandler(CHttpParser *parser)
// {
//     this->parser = parser;
// }

CHttpHandler::CHttpHandler()
{
    // this->parser = parser;
}

void CHttpHandler::LogResponse(char *buffer, int len)
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
        pair<string, string> temp = Utils::ChopLine(test);
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

extern "C" CHttpHandler *createCHttpHandler()
{
    return new CHttpHandler;
}

extern "C" void destroyCHttpHandler(CHttpHandler *c)
{
    delete c;
}
