//
// Created by Sinan on 19/03/24.
//

#include "HTTPBasicLoginCommand.h"
#include "AuthenticationStrategy.h"

extern "C" HTTPBasicLoginCommand *createHTTPBasicLoginCommand()
{
    return new HTTPBasicLoginCommand;
}

extern "C" void destroyHTTPBasicLoginCommand(HTTPBasicLoginCommand *c)
{
    delete c;
}

bool HTTPBasicLoginCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    auto *response = new simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);
    
    nlohmann::json body = nlohmann::json::parse(request->content());
    std::string username = body["username"].get<std::string>();
    std::string password = body["password"].get<std::string>();

    auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get("authentication"));
    ComputationContext auth_context;
    auth_context.Put("username", username);
    auth_context.Put("password", password);
    auth->authenticate(&auth_context);

    auto authenticated = std::any_cast<bool>(auth_context.Get("authenticated"));
    if (!authenticated) {
        response->SetStatusCode(simple_http_server::HttpStatusCode::Unauthorized);
        response->SetHeader("Content-Type", "application/json");
        response->SetContent("Unauthorized");
        context->Put("response", response);
        return false;
    }

    std::string token = std::any_cast<std::string>(auth_context.Get("token"));
    nlohmann::json response_content;
    response_content["token"] = token;

    response->SetHeader("Content-Type", "application/json");
    response->SetContent(response_content.dump());
    context->Put("response", response);
    return true;
}
