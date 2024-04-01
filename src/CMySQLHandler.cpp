#include "CMySQLHandler.h"
#include "CProxyHandler.h"
#include "CServerSocket.h"

extern "C" CMySQLHandler *createCMySQLHandler() { return new CMySQLHandler; }

extern "C" void destroyCHWirePTHandler(CMySQLHandler *c) { delete c; }
