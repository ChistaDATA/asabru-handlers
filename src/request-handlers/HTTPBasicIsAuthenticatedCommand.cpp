//
// Created by Midhun Darvin on 08/11/23.
//

#include "HTTPBasicIsAuthenticatedCommand.h"
#include "AuthenticationStrategy.h"
#include "ComputationContext.h"

// Mutex for thread safety
std::mutex coutMutex;

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
    auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get("authentication"));
    auto token = request->header("Authorization");
    if (token.empty()) {
        context->Put("authenticated", false);
        return true;
    }

    if (token.find("Bearer ") == 0) {
        token = token.substr(7);
    } else {
        context->Put("authenticated", false);
        return true;
    }

    ComputationContext auth_context;
    auth_context.Put("token", token);
    auth->isAuthenticated(&auth_context);
    auto authenticated = std::any_cast<bool>(auth_context.Get("authenticated"));
    context->Put("authenticated", authenticated);
    return true;
}
