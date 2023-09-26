
#include <map>
#include "Utils.h"
#include "./CHttpsHandler.h"
#include "LineGrabber.h"
#include "CommonTypes.h"

using namespace std;

CHttpsHandler::CHttpsHandler()
{
}

extern "C" CHttpsHandler *createCHttpsHandler()
{
    return new CHttpsHandler;
}

extern "C" void destroyCHttpsHandler(CHttpsHandler *c)
{
    delete c;
}