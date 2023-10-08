
#include <map>
#include "Utils.h"
#include "./CHttpsHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"

using namespace std;

CHttpsHandler::CHttpsHandler()
{
}

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHttpsHandler::HandleUpstreamData(void *buffer, int buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Client packet..................... " << endl;
    std::cout << "Length of Packet is " << buffer_length << endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

    std::string result;
    result.assign((char *)buffer, buffer_length);
    return result;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHttpsHandler::HandleDownStreamData(void *buffer, int buffer_length, EXECUTION_CONTEXT *exec_context)
{
    std::cout << "Received a Server packet..................... " << endl;
    std::cout << "Length of Packet is " << buffer_length << endl;
    std::cout << "Packet Type = " << (int)*((unsigned char *)buffer) << endl;

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