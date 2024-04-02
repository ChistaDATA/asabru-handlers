#include "CMySQLHandler.h"
#include "CProxyHandler.h"
#include "CServerSocket.h"
#include "Logger.h"
#include "ProtocolHelper.h"
#include "uuid/UuidGenerator.h"

extern "C" CMySQLHandler *createCMySQLHandler() { return new CMySQLHandler; }

extern "C" void destroyCHWirePTHandler(CMySQLHandler *c) { delete c; }

/**
 * Gets the length of a Length-Encoded Integer Type
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param start - the postition to start reading from the buffer
 * @return number of bytes to be read to get the value
 */
uint8_t readMySQLEncodedLength(std::string buffer, int start) {
	uint8_t firstByte = buffer[start];
	switch (firstByte) {
	case 0xFB:
		return 0;
	case 0xFC:
		return 2;
	case 0xFD:
		return 3;
	case 0xFE:
		return 8;
	default:
		return 1;
	}
}

/**
 * Gets a Length-Encoded Integer Type
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param start - the postition to start reading from the buffer
 * @return a length encoded integer in buffer starting from 'start'
 */
uint64_t readMySQLLengthEncodedInt(std::string buffer, int start) {
	uint8_t size = readMySQLEncodedLength(buffer, start);
	switch (size) {
	case 1:
		return readByte(buffer, start);
	case 2:
		return readLEInt16(buffer, start + 1);
	case 3:
		return readLEInt24(buffer, start + 1);
	case 8:
		return readLEInt64(buffer, start + 1);
	}
	return 0;
}

/**
 * Gets a Length-Encoded String
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param start - the postition to start reading from the buffer
 * @return a length encoded string in buffer starting from 'start'
 */
std::string readMySQLLengthEncodedString(std::string buffer, int start) {
	uint64_t length = readMySQLLengthEncodedInt(buffer, start);
	return buffer.substr(start + 1, length);
};

/**
 * Extracts the query from the query command payload
 * @param capabilities - the capabilities the client and server has agreed on
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param start - the postition to start reading from the buffer
 * @return the actual query that was run by the client
 */
std::string extract_query_from_query_command(uint32_t capabilities, std::string buffer, int start) {
	// +------------------------------------------+----------------------+---------------------------------------------------+
	// | Type                                     | Name                 | Description                                       |
	// |------------------------------------------+----------------------+---------------------------------------------------|
	// | int<1>                                   | command              | 0x03: COM_QUERY                                   |
	// |                                                                                                                     |
	// | if CLIENT_QUERY_ATTRIBUTES is set {                                                                                 |
	// |                                                                                                                     |
	// |     int<lenenc>                          | parameter_count      | Number of parameters                              |
	// |     int<lenenc>                          | parameter_set_count  | Number of parameter sets. Currently always 1      |
	// |                                                                                                                     |
	// |     if parameter_count > 0 {                                                                                        |
	// |                                                                                                                     |
	// |         binary<var>                      | null_bitmap          | NULL bitmap, length= (num_params + 7) / 8         |
	// |         int<1>                           | new_params_bind_flag | Always 1. Malformed packet error if not 1         |
	// |                                                                                                                     |
	// |         if new_params_bind_flag, for each parameter {                                                               |
	// |                                                                                                                     |
	// |             int<2>                       | param_type_and_flag  | Parameter type (2 bytes).                         |
	// |             string<lenenc>               | parameter name       | String                                            |
	// |                                                                                                                     |
	// |         }                                                                                                           |
	// |                                                                                                                     |
	// |         binary<var>                      | parameter_values     | parameter value: Binary Protocol Value            |
	// |                                                                                                                     |
	// |     }                                                                                                               |
	// | }                                                                                                                   |
	// |                                                                                                                     |
	// | string<EOF>                              | query                | the text of the SQL query to execute              |
	// +------------------------------------------------+----------------+---------------------------------------------------+
	int pos = start;
	// skip command byte
	pos++;
	if (capabilities & CLIENT_QUERY_ATTRIBUTES) {
		uint8_t parameter_count_length = readMySQLEncodedLength(buffer, pos);
		uint64_t parameter_count = readMySQLLengthEncodedInt(buffer, pos);
		pos++; // skip length of encoding byte
		if (parameter_count_length > 1) {
			// skip length of parameter_set_count if length is greater than 1 byte
			// in cases where length = 1, the value in that byte itself will be the length
			pos += parameter_count_length;
		}

		uint8_t parameter_set_count_length = readMySQLEncodedLength(buffer, pos);
		pos++; // skip length of encoding byte
		if (parameter_set_count_length > 1) {
			// skip length of parameter_set_count if length is greater than 1 byte
			// in cases where length = 1, the value in that byte itself will be the length
			pos += parameter_set_count_length;
		}

		if (parameter_count > 0) {
			pos += (parameter_count + 7) / 8; // skip length of null_bitmap

			uint8_t new_params_bind_flag = buffer[pos];
			pos++; // forward size of new_params_bind_flag

			if (new_params_bind_flag) {
				for (auto i = 0; i < parameter_count; i++) {
					pos += 2; // move size of param_type_and_flag

					uint8_t parameter_name_length = readMySQLEncodedLength(buffer, pos);
					pos++; // skip length of encoding byte
					if (parameter_name_length > 1) {
						// skip length of parameter name if length is greater than 1 byte
						// in cases where length = 1, the value in that byte itself will be the length
						pos += parameter_name_length;
					}
				}
			}

			uint8_t parameter_values_length = readMySQLEncodedLength(buffer, pos);
			pos++; // skip length of encoding byte
			if (parameter_values_length > 1) {
				// skip length of parameter values if length is greater than 1 byte
				// in cases where length = 1, the value in that byte itself will be the length
				pos += parameter_values_length;
			}
		}
	}
	return readCString(buffer, pos);
}

//
/**
 * Extracts the query from the prepare command payload
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param start - the postition to start reading from the buffer
 * @return the actual query that was run by the client
 */
std::string extract_query_from_prepare_command(std::string buffer, int start) {
	// +-------------+---------+------------------------+
	// | Type        | Name    | Description            |
	// +-------------+---------+------------------------+
	// | int<1>      | command | 0x16: COM_STMT_PREPARE |
	// | string<EOF> | query   | The query to prepare   |
	// +-------------+---------+------------------------+
	return readCString(buffer, start + 1);
}

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CMySQLHandler::HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
	const uint32_t capabilities = std::any_cast<uint32_t>((*exec_context)["capabilities"]);
	int start = 0;
	// payload starts after 4 byte header(3 byte length + 1 byte sequence id)
	start += 4;
	const char command_type = buffer[start];
	switch (command_type) {
	case COM_QUERY: {
		auto query_start_time = std::chrono::high_resolution_clock::now();
		(*exec_context)["query_start_time"] = query_start_time;
		// set correlation_id in exec_context
		std::string query_correlation_id = UuidGenerator::generateUuid();
		(*exec_context)["query_correlation_id"] = query_correlation_id;
		const std::string query = extract_query_from_query_command(capabilities, buffer, start);
		LOG_QUERY(query_correlation_id, query);
		break;
	}
	case COM_STMT_PREPARE: {
		auto query_start_time = std::chrono::high_resolution_clock::now();
		(*exec_context)["query_start_time"] = query_start_time;
		// set correlation_id in exec_context
		std::string query_correlation_id = UuidGenerator::generateUuid();
		(*exec_context)["query_correlation_id"] = query_correlation_id;
		const std::string query = extract_query_from_prepare_command(buffer, start);
		LOG_QUERY(query_correlation_id, query);
		break;
	}
	}
	return buffer;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream ( target dbs )
 * @param length - length of the buffer
 */
std::string CMySQLHandler::HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
	if (!exec_context->contains("query_correlation_id")) {
		return buffer;
	}
	std::string query_correlation_id = std::any_cast<std::string>((*exec_context)["query_correlation_id"]);
	auto query_start_time = std::any_cast<std::chrono::high_resolution_clock::time_point>((*exec_context)["query_start_time"]);
	auto query_stop_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(query_stop_time - query_start_time);
	auto target_host = std::any_cast<std::string>((*exec_context)["target_host"]);
	LOG_LATENCY(query_correlation_id, std::to_string(duration.count()) + "," + target_host);
	exec_context->erase("query_correlation_id");
	return buffer;
}
