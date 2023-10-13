
#include <map>
#include "Utils.h"
#include "CHttpHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"
#include "http_message.h"

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleUpstreamData(void *buffer, int buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Client packet..................... " << endl;
    std::cout << "Length of Packet is " << buffer_length << endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

    simple_http_server::HttpRequest request = simple_http_server::string_to_request(std::string((char *) buffer)); 

    std::string deconstructedRequest = simple_http_server::to_string(request);

    int deconstructedBufferSize = strlen(deconstructedRequest.c_str());
    std::string result;
    result.assign(deconstructedRequest.c_str(), deconstructedBufferSize);
    return result;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleDownStreamData(void *buffer, int buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Server packet..................... " << endl;
    std::cout << "Length of Packet is " << buffer_length << endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

    std::string result;
    result.assign((char *)buffer, buffer_length);
    return result;
}

extern "C" CHttpHandler *createCHttpHandler()
{
    return new CHttpHandler;
}

extern "C" void destroyCHttpHandler(CHttpHandler *c)
{
    delete c;
}
