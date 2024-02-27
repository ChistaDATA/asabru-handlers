
#include <map>
#include "Utils.h"
#include "./CHttpsHandler.h"
#include "http_message.h"

CHttpsHandler::CHttpsHandler()
{
}

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHttpsHandler::HandleUpstreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Client packet..................... " << std::endl;
    std::cout << "Length of Packet is " << buffer_length << std::endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << std::endl;

    std::string request_string = (char *) buffer;
    request_string = request_string.substr(0, buffer_length);
    simple_http_server::HttpRequest request = simple_http_server::string_to_request(request_string);
    LOG_INFO(std::string("QUERY : ") + request.content());

    std::string result;
    result.assign((char *)buffer, buffer_length);
    return result;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHttpsHandler::HandleDownStreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Server packet..................... " << std::endl;
    std::cout << "Length of Packet is " << buffer_length << std::endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << std::endl;

    std::string result;
    result.assign((char *)buffer, buffer_length);
    return result;
}

extern "C" CHttpsHandler *createCHttpsHandler()
{
    return new CHttpsHandler;
}

extern "C" void destroyCHttpsHandler(CHttpsHandler *c)
{
    delete c;
}