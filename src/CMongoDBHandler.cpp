#include "CMongoDBHandler.h"
#include <cstring>
#include <string>
#include "util/endian.h"
#include "nlohmann/json.hpp"
#include "ProtocolHelper.h"

extern "C" CMongoDBHandler *createCMongoDBHandler() { return new CMongoDBHandler; }

extern "C" void destroyCMongoDBHandler(CMongoDBHandler *c) { delete c; }

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CMongoDBHandler::HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
    /**
     * Read MsgHeader
     * struct MsgHeader {
     *       int32   messageLength; // total message size, including this
     *       int32   requestID;     // identifier for this message
     *       int32   responseTo;    // requestID from the original request
     *                              //   (used in responses from the database)
     *       int32   opCode;        // message type
     *   }
     */
    const std::string query_correlation_id = std::any_cast<std::string>((*exec_context)["correlation_id"]);
    uint32_t messageLength = readInt32(buffer, 0, 4);
    uint32_t requestID = readInt32(buffer, 4, 4);
    uint32_t responseTo = readInt32(buffer, 8, 4);
    uint32_t opCode = readInt32(buffer, 12, 4);

    if (opCode == OP_MSG) {
        /**
         * OP_MSG {
         *       MsgHeader header;           // standard message header
         *       uint32 flagBits;            // message flags
         *       Sections[] sections;        // data sections
         *       optional<uint32> checksum;  // optional CRC-32C checksum
         * }
         */
        uint32_t flagBits = readInt32(buffer, 16, 4);
        int kind = (unsigned int) buffer[20];
        if (kind == 0) {
            /**
             * Kind 0: Body
             * A body section is encoded as a single BSON object.
             * The size in the BSON object also serves as the size of the section. This section kind is the standard command request and reply body.
             */
            uint32_t bodySize = readInt32(buffer, 21, 4);
            try {
                // deserialize it with BSON
                std::string bson_string = buffer.substr(21);
                nlohmann::json object_json = nlohmann::json::from_bson(buffer.substr(21, bodySize));
                if (object_json.contains("find") || object_json.contains("insert") || object_json.contains("drop")) {
                    // print the deserialized JSON value
                    LOG_QUERY(query_correlation_id, object_json.dump())
                }
            } catch (std::exception e) {
                LOG_ERROR(e.what())
            }
        }
    }

    return buffer;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream (
 * target dbs )
 * @param length - length of the buffer
 */
std::string CMongoDBHandler::HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
    return buffer;
}
