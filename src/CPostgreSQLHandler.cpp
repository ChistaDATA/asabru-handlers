#include "CPostgreSQLHandler.h"
#include "CProxyHandler.h"
#include "CServerSocket.h"
#include "Logger.h"
#include <cstring>
#include <string>

extern "C" CPostgreSQLHandler *createCPostgreSQLHandler() { return new CPostgreSQLHandler; }

extern "C" void destroyCPostgreSQLHandler(CPostgreSQLHandler *c) { delete c; }

/**
 * Function to return the length of the message body
 * @param buffer - the buffer containing the message bytes
 */
size_t getMessageBodyLength(std::string buffer) {
	const std::string packet_length_nw_bytes = buffer.substr(1, MSG_LENGTH_LEN);
	uint32_t packet_length_nw = 0;
	memcpy(&packet_length_nw, &packet_length_nw_bytes, MSG_LENGTH_LEN);
	const uint32_t packet_length = ntohl(packet_length_nw);
	return packet_length - MSG_LENGTH_LEN;
}

/**
 * Function that handles upstream data
 * @param buffer - the buffer that we receive from upstream ( source dbs )
 * @param length - length of the buffer
 */
std::string CPostgreSQLHandler::HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
	const int message_type = buffer[0];
	if (message_type == MsgTypeQuery) {
		/* Message structure
		 * Query (F)
		 *	Byte1('Q')
		 *		Identifies the message as a simple query.
		 *	Int32
		 *		Length of the message contents in bytes, including self.
		 *	String
		 *		The query string itself
		 */
		const std::string query_correlation_id = std::any_cast<std::string>((*exec_context)["correlation_id"]);
		const size_t body_length = getMessageBodyLength(buffer);
		const size_t a = -1;
		// length includes the null character as well. so subtracting 1
		const size_t query_length = body_length - 1;
		const std::string query = buffer.substr(5, query_length);
		LOG_QUERY(query_correlation_id, query);
	} else if (message_type == MsgTypeParse) {
		/*
		 * Parse (F)
		 *	Byte1('P')
		 *		Identifies the message as a Parse command.
		 *	Int32
		 *		Length of message contents in bytes, including self.
		 *	String
		 *		The name of the destination prepared statement (an empty string selects the unnamed prepared statement).
		 *	String
		 *		The query string to be parsed. Int16 The number of parameter data types specified (can be zero). Note that this is
		 *		not an indication of the number of parameters that might appear in the query string, only the number that the
		 *		frontend wants to prespecify types for.
		 *
		 *	Then, for each parameter, there is the following:
		 *
		 *	Int32
		 *		Specifies the object ID of the parameter data type. Placing a zero here is equivalent to leaving the type
		 *		unspecified.
		 */
		const std::string query_correlation_id = std::any_cast<std::string>((*exec_context)["correlation_id"]);
		const size_t packet_length = getMessageBodyLength(buffer);
		// String types are null terminated in postgres protocol
		// First null terminated string is statement name
		const size_t stmt_name_end_idx = buffer.find(NULL_CHAR, MSG_LENGTH_LEN);
		const size_t query_start_idx = stmt_name_end_idx + 1;
		// The next null terminated string is the actual query
		const size_t query_end_idx = buffer.find(NULL_CHAR, query_start_idx);
		const size_t query_length = query_end_idx - query_start_idx;
		const std::string query = buffer.substr(query_start_idx, query_length);
		LOG_QUERY(query_correlation_id, query);
	}
	return buffer;
}

/**
 * Function that handles downstream data
 * @param buffer - the buffer / response that we receive from downstream (
 * target dbs )
 * @param length - length of the buffer
 */
std::string CPostgreSQLHandler::HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) {
	return buffer;
}
