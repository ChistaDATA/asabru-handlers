//
// Created by Sinan on 19/03/24.
//

#include "HTTPBasicIsAuthenticatedCommand.h"
#include "authentication/AuthenticationStrategy.h"
#include "ComputationContext.h"
#include "authentication/BasicAuthenticationStrategy.h"

extern "C" HTTPBasicIsAuthenticatedCommand *createHTTPBasicIsAuthenticatedCommand()
{
    return new HTTPBasicIsAuthenticatedCommand;
}

extern "C" void destroyHTTPBasicIsAuthenticatedCommand(HTTPBasicIsAuthenticatedCommand *c)
{
    delete c;
}

bool HTTPBasicIsAuthenticatedCommand::Execute(ComputationContext *context) {
    auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
    auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get(AUTHENTICATION_STRATEGY_KEY));
    auto token = request->header("Authorization");
    if (token.empty()) {
        context->Put(AUTH_AUTHENTICATED_KEY, false);
        return true;
    }

    if (token.find("Bearer ") == 0) {
        token = token.substr(7);
    } else {
        context->Put(AUTH_AUTHENTICATED_KEY, false);
        return true;
    }

    ComputationContext auth_context;
    auth_context.Put(AUTH_BASIC_TOKEN_KEY, token);
    auth->isAuthenticated(&auth_context);
    auto authenticated = std::any_cast<bool>(auth_context.Get(AUTH_AUTHENTICATED_KEY));
    context->Put(AUTH_AUTHENTICATED_KEY, authenticated);
    context->Put(AUTH_AUTHENTICATED_INFO, auth_context.GetString(AUTH_AUTHENTICATED_INFO));

    return true;
}
