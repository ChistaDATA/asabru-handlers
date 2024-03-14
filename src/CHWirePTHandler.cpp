#include "CServerSocket.h"
#include "CProxyHandler.h"
#include "CHWirePTHandler.h"

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CHWirePTHandler::HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Client packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );

    QueryType queryType;
    uint64_t packet_type = 0;
    char * temp = &buffer[0];

    readVarUInt(packet_type, temp);
    if (packet_type == 0)
    {
        parser->parseClientHello(temp, buffer_length);
    }
    else if (packet_type == 1)
    {
        parser->parseClientQuery(temp, buffer_length, exec_context);
    }

    parser->LogResponse((char *) buffer.c_str(), buffer_length);
    return buffer;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CHWirePTHandler::HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Server packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );

    return buffer;
}

extern "C" CHWirePTHandler *createCHWirePTHandler()
{
    return new CHWirePTHandler;
}

extern "C" void destroyCHWirePTHandler(CHWirePTHandler *c)
{
    delete c;
}