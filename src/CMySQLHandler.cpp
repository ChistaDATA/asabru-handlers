#include "CProxyHandler.h"
#include "CMySQLHandler.h"

extern "C" CMySQLHandler *createCMySQLHandler()
{
    return new CMySQLHandler;
}

extern "C" void destroyCMySQLHandler(CMySQLHandler *c)
{
    delete c;
}