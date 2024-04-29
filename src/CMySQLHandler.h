#ifndef CH_MYSQL_PROTOCOL_HANDLER_H
#define CH_MYSQL_PROTOCOL_HANDLER_H

#include "socket/CServerSocket.h"
#include "BaseProxyHandler.h"

// Values for the capabilities flag bitmask used by the MySQL protocol.
enum MySQLCapabalities {
	CLIENT_LONG_PASSWORD = 1 << 0,	   // Use the improved version of Old Password Authentication.
	CLIENT_FOUND_ROWS = 1 << 1,		   // Send found rows instead of affected rows in EOF_Packet.
	CLIENT_LONG_FLAG = 1 << 2,		   // Get all column flags.
	CLIENT_CONNECT_WITH_DB = 1 << 3,   // Database (schema) name can be specified on connect in Handshake Response Packet.
	CLIENT_NO_SCHEMA = 1 << 4,		   // DEPRECATED: Don't allow database.table.column.
	CLIENT_COMPRESS = 1 << 5,		   // Compression protocol supported.
	CLIENT_ODBC = 1 << 6,			   // Special handling of ODBC behavior.
	CLIENT_LOCAL_FILES = 1 << 7,	   // Can use LOAD DATA LOCAL.
	CLIENT_IGNORE_SPACE = 1 << 8,	   // Ignore spaces before '('.
	CLIENT_PROTOCOL_41 = 1 << 9,	   // New 4.1 protocol.
	CLIENT_INTERACTIVE = 1 << 10,	   // This is an interactive client.
	CLIENT_SSL = 1 << 11,			   // Use SSL encryption for the session.
	CLIENT_IGNORE_SIGPIPE = 1 << 12,   // Client only flag.
	CLIENT_TRANSACTIONS = 1 << 13,	   // Client knows about transactions.
	CLIENT_RESERVED = 1 << 14,		   // DEPRECATED: Old flag for 4.1 protocol
	CLIENT_RESERVED2 = 1 << 15,		   // DEPRECATED: Old flag for 4.1 authentication \ CLIENT_SECURE_CONNECTION.
	CLIENT_MULTI_STATEMENTS = 1 << 16, // Enable/disable multi-stmt support.
	CLIENT_MULTI_RESULTS = 1 << 17,	   // Enable/disable multi-results.
	CLIENT_PS_MULTI_RESULTS = 1 << 18, // Multi-results and OUT parameters in PS-protocol.
	CLIENT_PLUGIN_AUTH = 1 << 19,	   // Client supports plugin authentication.
	CLIENT_CONNECT_ATTRS = 1 << 20,	   // Client supports connection attributes.
	CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA = 1 << 21, // Enable authentication response packet to be larger than 255 bytes.
	CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS = 1 << 22,	 // Don't close the connection for a user account with expired password.
	CLIENT_SESSION_TRACK = 1 << 23,					 // Capable of handling server state change information.
	CLIENT_DEPRECATE_EOF = 1 << 24,					 // Client no longer needs EOF_Packet and will use OK_Packet instead.
	CLIENT_OPTIONAL_RESULTSET_METADATA = 1 << 25,	 // The client can handle optional metadata information in the resultset.
	CLIENT_ZSTD_COMPRESSION_ALGORITHM = 1 << 26,	 // Compression protocol extended to support zstd compression method.
	CLIENT_QUERY_ATTRIBUTES =
		1 << 27, // Support optional extension for query parameters into the COM_QUERY and COM_STMT_EXECUTE packets.
	MULTI_FACTOR_AUTHENTICATION = 1 << 28,	 // Support Multi factor authentication.
	CLIENT_CAPABILITY_EXTENSION = 1 << 29,	 // This flag will be reserved to extend the 32bit capabilities structure to 64bits.
	CLIENT_SSL_VERIFY_SERVER_CERT = 1 << 30, // Verify server certificate.
	CLIENT_REMEMBER_OPTIONS = 1 << 31,		 // Don't reset the options after an unsuccessful connect.
};

// A list of all MySQL protocol commands.
enum MySQLCommandType {
	COM_QUERY = 0x3,		 // Send a Text Protocol based SQL query
	COM_STMT_PREPARE = 0x16, // Creates a prepared statement for the passed query string.
};

class CMySQLHandler : public BaseProxyHandler {

  public:
	CMySQLHandler() {}
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
