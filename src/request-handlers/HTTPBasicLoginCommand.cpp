//
// Created by Sinan on 19/03/24.
//

#include "HTTPBasicLoginCommand.h"
#include "authentication/AuthenticationStrategy.h"
#include "authentication/BasicAuthenticationStrategy.h"

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
    simple_http_server::HttpResponse response(simple_http_server::HttpStatusCode::Ok);
    
    nlohmann::json body = nlohmann::json::parse(request->content());
    std::string username = body[AUTH_BASIC_USERNAME_KEY].get<std::string>();
    std::string password = body[AUTH_BASIC_PASSWORD_KEY].get<std::string>();

    auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get(AUTHENTICATION_STRATEGY_KEY));
    ComputationContext auth_context;
    auth_context.Put(AUTH_BASIC_USERNAME_KEY, username);
    auth_context.Put(AUTH_BASIC_PASSWORD_KEY, password);
    auth->authenticate(&auth_context);

    auto authenticated = std::any_cast<bool>(auth_context.Get(AUTH_AUTHENTICATED_KEY));
    if (!authenticated) {
        response.SetStatusCode(simple_http_server::HttpStatusCode::Unauthorized);
        response.SetHeader("Content-Type", "application/json");
        response.SetContent("Unauthorized");
        context->Put("response", response);
        return false;
    }

    std::string token = std::any_cast<std::string>(auth_context.Get(AUTH_BASIC_TOKEN_KEY));
    nlohmann::json response_content;
    response_content["token"] = token;

    response.SetHeader("Content-Type", "application/json");
    response.SetContent(response_content.dump());
    context->Put("response", response);
    return true;
}
