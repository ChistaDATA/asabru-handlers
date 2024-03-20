#include "CServerSocket.h"
#include "CProxyHandler.h"
#include "CHWirePTHandler.h"
#include <any>

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
        (*exec_context)["query_correlation_id"] = (*exec_context)["correlation_id"];
        (*exec_context)["query_start_time"] = std::chrono::high_resolution_clock::now();
        parser->parseClientQuery(temp, buffer_length, exec_context);
    } else if (packet_type == 2) {
//        parser->LogResponse((char *) buffer.c_str(), buffer_length, "Client");
    }


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

    QueryType queryType;
    uint64_t packet_type = 0;
    char * temp = &buffer[0];

    readVarUInt(packet_type, temp);
    LOG_INFO("Packet Type = " + std::to_string((int) packet_type));
    if (packet_type == 1) {
        std::cout << "Data Packet" << std::endl;
        parser->LogResponse((char *) buffer.c_str(), buffer_length, "Server");
    } else if (packet_type == 5)
    {
        std::string query_correlation_id = std::any_cast<std::string>((*exec_context)["query_correlation_id"]);
        auto query_start_time = std::any_cast<std::chrono::high_resolution_clock::time_point>((*exec_context)["query_start_time"]);
        auto query_end_time = std::any_cast<std::chrono::high_resolution_clock::time_point>((*exec_context)["request_stop_time"]);
        auto target_host = std::any_cast<std::string>((*exec_context)["target_host"]);
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(query_end_time - query_start_time);
    }

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