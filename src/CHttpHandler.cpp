
#include <map>
#include "Utils.h"
#include "CHttpHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleUpstreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Client packet..................... " << endl;
    std::cout << "Length of Packet is " << buffer_length << endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

    HttpMetadata metadata = parser->construct(buffer, buffer_length);

    // Log the Content
    parser->logMetadata(&metadata);

    // // Reconstruct the buffer from the metadata struct
    void *deconstructedBuffer = parser->deconstruct(&metadata);

    int deconstructedBufferSize = strlen((char *)deconstructedBuffer);
    cout << "Lengths : " << buffer_length << " " << deconstructedBufferSize << endl;

    std::string result;
    result.assign((char *)deconstructedBuffer, deconstructedBufferSize);
    return result;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHttpHandler::HandleDownStreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
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
