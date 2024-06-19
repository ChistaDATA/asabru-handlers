#include "UpdateConfigurationCommand.h"
#include "logger/Logger.h"

extern "C" UpdateConfigurationCommand *createUpdateConfigurationCommand()
{
    return new UpdateConfigurationCommand;
}

extern "C" void destroyUpdateConfigurationCommand(UpdateConfigurationCommand *c)
{
    delete c;
}

bool UpdateConfigurationCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    std::any functionAny = context->Get("update_configuration");
    // Cast and call the function using std::any
    if (functionAny.type() == typeid(std::string (*)(std::string))) {
        auto updateConfiguration = std::any_cast<std::string (*)(std::string)>(functionAny);
        std::string response_content = updateConfiguration(request->content());
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
