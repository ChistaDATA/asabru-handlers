//
// Created by Midhun Darvin on 11/01/24.
//

#include "GzipHandler.h"
#include <gzip/compress.hpp>
#include <gzip/decompress.hpp>

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string GzipHandler::HandleUpstreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("GzipHandler::HandleUpstreamData");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer)) );

    // All function calls must pass in a pointer of an
    // immutable character sequence (aka a string in C) and its size
    const char * pointer = (const char *) buffer;
    std::size_t size = buffer_length;

    // Compress returns a std::string
    std::string compressed_data = gzip::compress(pointer, size);
    // Decompress returns a std::string
    const char * compressed_pointer = compressed_data.data();
    std::string decompressed_data = gzip::decompress(compressed_pointer, compressed_data.size());

    return decompressed_data;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string GzipHandler::HandleDownStreamData(void *buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context)
{
    LOG_INFO("GzipHandler::HandleDownStreamData");
    LOG_INFO("Length of Packet is " + std::to_string(buffer_length) );
    LOG_INFO("Packet Type = " + std::to_string((int) *((unsigned char *)buffer)) );

    std::string result;
    result.assign((char *)buffer, buffer_length);
    return result;
}

extern "C" GzipHandler *createGzipHandler()
{
    return new GzipHandler;
}

extern "C" void destroyGzipHandler(GzipHandler *c)
{
    delete c;
}
