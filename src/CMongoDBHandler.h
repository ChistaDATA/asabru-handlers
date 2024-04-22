#pragma
#include "BaseProxyHandler.h"
#include "socket/CServerSocket.h"

#define MSG_LENGTH_LEN 4
#define NULL_CHAR '\0'

enum MongoDbOpCode {
    OP_REPLY = 1,
    DB_MSG = 1000,
    OP_UPDATE = 2001,
    OP_INSERT = 2002,
    RESERVED = 2003,
    OP_QUERY = 2004,
    OP_GET_MORE = 2005,
    OP_DELETE = 2006,
    OP_KILL_CURSORS = 2007,
    OP_COMMAND = 2010,
    OP_COMMANDREPLY = 2011,
    OP_COMPRESSED = 2012,
    OP_MSG = 2013
};

/**
    the complete list of valid BSON types
    see also bsonspec.org
*/
enum BSONType {
    /** smaller than all other types */
    MinKey = -1,
    /** end of object */
    EOO = 0,
    /** double precision floating point value */
    NumberDouble = 1,
    /** character string, stored in utf8 */
    String = 2,
    /** an embedded object */
    Object = 3,
    /** an embedded array */
    Array = 4,
    /** binary data */
    BinData = 5,
    /** (Deprecated) Undefined type */
    Undefined = 6,
    /** ObjectId */
    jstOID = 7,
    /** boolean type */
    Bool = 8,
    /** date type */
    Date = 9,
    /** null type */
    jstNULL = 10,
    /** regular expression, a pattern with options */
    RegEx = 11,
    /** (Deprecated) */
    DBRef = 12,
    /** code type */
    Code = 13,
    /** (Deprecated) a programming language (e.g., Python) symbol */
    Symbol = 14,
    /** (Deprecated) javascript code that can execute on the database server, with SavedContext */
    CodeWScope = 15,
    /** 32 bit signed integer */
    NumberInt = 16,
    /** Two 32 bit signed integers */
    bsonTimestamp = 17,
    /** 64 bit integer */
    NumberLong = 18,
    /** 128 bit decimal */
    NumberDecimal = 19,
    /** max type that is not MaxKey */
    JSTypeMax = 19,
    /** larger than all other types */
    MaxKey = 127
};

enum PostgreSQLServerMessageType { MsgTypeRowDescription = 'T' };

class CMongoDBHandler : public BaseProxyHandler {
public:
    CMongoDBHandler() {}
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

