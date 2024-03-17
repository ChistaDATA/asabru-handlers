#include "CSnowflakeHttpHandler.h"
#include <nlohmann/json.hpp>
#include <gzip/decompress.hpp>

using json = nlohmann::json;

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CSnowflakeHttpHandler::HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Client packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );
    std::string correlation_id = std::any_cast<std::string>((*exec_context)["correlation_id"]);

    std::string request_string = buffer;
    simple_http_server::HttpRequest request;
    if (!request_string.empty()) {
        request_string = request_string.substr(0, buffer_length);

        request = simple_http_server::string_to_request(request_string);

        /**
         * Modify SNI for forward proxy to work ( Eg. snowflake )
         */
        request.SetHeader("Host", "ip24827.ap-southeast-1.snowflakecomputing.com");

        if (request.GetHeader("Content-Encoding") == "gzip") {
            std::string decompressed_data = gzip::decompress(request.content().data(), request.content().size());
            LOG_INFO(decompressed_data);
            nlohmann::json body = nlohmann::json::parse(decompressed_data);
            std::string sqlText = body["sqlText"] != nullptr ? body["sqlText"] : "";
            LOG_INFO(sqlText);
            if (!sqlText.empty()) {
                LOG_QUERY(correlation_id, sqlText);
            }
        } else {
            LOG_QUERY(correlation_id, request.content());
        }
    }

    std::string deconstructedRequest = simple_http_server::to_string(request);
    return deconstructedRequest;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CSnowflakeHttpHandler::HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("Received a Server packet..................... ");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer.c_str())) );

    return buffer;
}

extern "C" CSnowflakeHttpHandler *createCSnowflakeHttpHandler()
{
    return new CSnowflakeHttpHandler;
}

extern "C" void destroyCSnowflakeHttpHandler(CSnowflakeHttpHandler *c)
{
    delete c;
}
