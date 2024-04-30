//
// Created by Sinan on 19/03/24.
//
#include "HTTPAfcasIsAuthorizedCommand.h"
#include "ComputationContext.h"
#include "authentication/AuthenticationStrategy.h"
#include "authorization/AfcasAuthorizationStrategy.h"
#include "authorization/AuthorizationStrategy.h"

extern "C" HTTPAfcasIsAuthorizedCommand *createHTTPAfcasIsAuthorizedCommand() { return new HTTPAfcasIsAuthorizedCommand(); }

extern "C" void destroyHTTPAfcasIsAuthorizedCommand(HTTPAfcasIsAuthorizedCommand *c) { delete c; }

typedef struct {
	std::string resourceId;
	std::string operationId;
	std::string principalName;
} Predicate;

std::vector<Predicate> parseAfcasAuthorizationData(const std::string &data, ComputationContext *context) {
	auto username = std::any_cast<std::string>(context->Get(AUTH_AUTHENTICATED_INFO));

	LOG_INFO("username is " + username);

	tinyxml2::XMLDocument doc;
	auto xmlError = doc.Parse(data.c_str());
	if (xmlError != tinyxml2::XML_SUCCESS) {
		auto error = "Error parsing afcas data: " + std::to_string(xmlError);
		LOG_ERROR(error);
		throw std::runtime_error(error);
	}

	tinyxml2::XMLElement *rootElement = doc.FirstChildElement("authorization");
	if (!rootElement) {
        auto error = "Error parsing afcas data: root element authorization not found";
        LOG_ERROR(error);
        throw std::runtime_error(error);
    }

	tinyxml2::XMLElement *predicatesElement = rootElement->FirstChildElement("predicates");
	if (!predicatesElement) {
		auto error = "Error parsing afcas data: predicates element not found";
		LOG_ERROR(error);
		throw std::runtime_error(error);
	}

	std::vector<Predicate> predicates;
	tinyxml2::XMLElement *predicateElement = predicatesElement->FirstChildElement("predicate");
	while (predicateElement) {
		Predicate p;
		p.resourceId = predicateElement->FirstChildElement("resourceId")->GetText();
		p.operationId = predicateElement->FirstChildElement("operationId")->GetText();
		p.principalName = username;
		predicates.push_back(p);
		predicateElement = predicateElement->NextSiblingElement("predicate");
	}

	return predicates;
}

bool HTTPAfcasIsAuthorizedCommand::Execute(ComputationContext *context) {
	auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
	auto *auth = std::any_cast<AuthorizationStrategy *>(context->Get(AUTHORIZATION_STRATEGY_KEY));

	auto data = std::any_cast<std::string>(context->Get(AUTHORIZATION_DATA_KEY));
	auto predicates = parseAfcasAuthorizationData(data, context);
	for (auto &predicate : predicates) {
		ComputationContext auth_context;
		auth_context.Put(AFCAS_PRINCIPAL_NAME_KEY, predicate.principalName);
		auth_context.Put(AFCAS_RESOURCE_ID_KEY, predicate.resourceId);
		auth_context.Put(AFCAS_OPERATION_ID_KEY, predicate.operationId);
		auth->isAuthorized(&auth_context);
		if (!std::any_cast<bool>(auth_context.Get(AUTHORIZATION_AUTHORIZED_KEY))) {
			context->Put(AUTHORIZATION_AUTHORIZED_KEY, false);
			return true;
		}
	}

	context->Put(AUTHORIZATION_AUTHORIZED_KEY, true);
	return true;
}
