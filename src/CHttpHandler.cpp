#include "CHttpHandler.h"

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleUpstreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Client packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer)) );

    std::string request_string = (char *) buffer;
    request_string = request_string.substr(0, buffer_length);
    simple_http_server::HttpRequest request = simple_http_server::string_to_request(request_string);
    LOG_INFO(std::string("QUERY : ") + request.content());
//    std::string deconstructedRequest = simple_http_server::to_string(request);
//
//    int deconstructedBufferSize = strlen(deconstructedRequest.c_str());
    std::string result;
    result.assign((char *) buffer, buffer_length);
    return result;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleDownStreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Server packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer)) );

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
