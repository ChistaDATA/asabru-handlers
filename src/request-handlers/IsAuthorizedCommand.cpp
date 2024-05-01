//
// Created by Sinan on 19/03/24.
//
#include "IsAuthorizedCommand.h"
#include "authorization/AfcasAuthorizationStrategy.h"

extern "C" IsAuthorizedCommand *createIsAuthorizedCommand() { return new IsAuthorizedCommand(); }

extern "C" void destroyIsAuthorizedCommand(IsAuthorizedCommand *c) { delete c; }

bool IsAuthorizedCommand::Execute(ComputationContext *context) {
	auto *auth = std::any_cast<AuthorizationStrategy *>(context->Get(AUTHORIZATION_STRATEGY_KEY));
	auth->isAuthorized(context);
	return true;
}
