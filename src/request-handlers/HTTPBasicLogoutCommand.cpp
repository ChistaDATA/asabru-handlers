//
// Created by Sinan on 19/03/24.
//

#include "HTTPBasicLogoutCommand.h"
#include "authentication/AuthenticationStrategy.h"
#include "authentication/BasicAuthenticationStrategy.h"

extern "C" HTTPBasicLogoutCommand *createHTTPBasicLogoutCommand()
{
    return new HTTPBasicLogoutCommand;
}

extern "C" void destroyHTTPBasicLogoutCommand(HTTPBasicLogoutCommand *c)
{
    delete c;
}

bool HTTPBasicLogoutCommand::Execute(ComputationContext *context)
{
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    auto *response = new simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);

    auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get(AUTHENTICATION_STRATEGY_KEY));

    auto token = request->header("Authorization");
    if (token.empty())
    {
        context->Put(AUTH_AUTHENTICATED_KEY, false);
        return true;
    }

    if (token.find("Bearer ") == 0)
    {
        token = token.substr(7);
    }
    else
    {
        context->Put(AUTH_AUTHENTICATED_KEY, false);
        return true;
    }

    ComputationContext auth_context;
    auth_context.Put(AUTH_BASIC_TOKEN_KEY, token);
    auth->removeAuthentication(&auth_context);

    response->SetHeader("Content-Type", "application/json");
    response->SetContent("Logout successful");
    context->Put("response", response);
    return true;
}
