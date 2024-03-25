#ifndef POSTGRESQL_PROTOCOL_HANDLER_H
#define POSTGRESQL_PROTOCOL_HANDLER_H
#include "BaseProxyHandler.h"
#include "CServerSocket.h"

#define MSG_LENGTH_LEN 4
#define NULL_CHAR '\0'

enum PostgreSQLClientMessageType { MsgTypeQuery = 'Q', MsgTypeParse = 'P' };
enum PostgreSQLServerMessageType { MsgTypeRowDescription = 'T' };

class CPostgreSQLHandler : public BaseProxyHandler {
  public:
	CPostgreSQLHandler() {}
	/**
	 * Function that handles upstream data
	 * @param buffer - the buffer that we receive from upstream ( source dbs )
	 * @param length - length of the buffer
	 */
	std::string HandleUpstreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) override;

	/**
	 * Function that handles downstream data
	 * @param buffer - the buffer / response that we receive from downstream (
	 * target dbs )
	 * @param length - length of the buffer
	 */
	std::string HandleDownStreamData(std::string buffer, ssize_t buffer_length, EXECUTION_CONTEXT *exec_context) override;
};

#endif
