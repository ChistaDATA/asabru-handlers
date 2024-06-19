#include "UpdateEndPointServiceCommand.h"
#include "logger/Logger.h"

extern "C" UpdateEndPointServiceCommand *createUpdateEndPointServiceCommand()
{
    return new UpdateEndPointServiceCommand;
}

extern "C" void  destroyUpdateEndPointServiceCommand(UpdateEndPointServiceCommand *c)
{
    delete c;
}

bool  UpdateEndPointServiceCommand::Execute(ComputationContext *context)
{
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    std::any functionAny = context->Get("update_endpoint_service");

    // Cast and call the function using std::any
    if (functionAny.type() == typeid(std::string (*)(std::string))) {
        auto updateEndPointService = std::any_cast<std::string (*)(std::string)>(functionAny);
        std::string response_content = updateEndPointService(request->content());
        simple_http_server::HttpResponse response(simple_http_server::HttpStatusCode::Ok);
        response.SetHeader("Content-Type", "text/plain");
        response.SetContent(response_content);
        context->Put("response", response);
        return true;
    } else {
        LOG_ERROR("Function pointer type mismatch.");
        return false;
    }
}
