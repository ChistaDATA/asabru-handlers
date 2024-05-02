//
// Created by JOSEPH ABRAHAM on 21/03/24.
//
#include <sstream>

#include "authentication/AuthenticationStrategy.h"
#include "socket/CClientSocket.h"
#include "HTTPRequestCommand.h"
#include "logger/Logger.h"

extern "C" HTTPRequestCommand *createHTTPRequestCommand() {
  return new HTTPRequestCommand;
}

extern "C" void HTTPRequestCommand(HTTPRequestCommand *c) { delete c; }

bool HTTPRequestCommand::Execute(ComputationContext *context) {
  LOG_INFO("HTTPRequestCommand");

  const simple_http_server::HttpMethod method =
      std::any_cast<simple_http_server::HttpMethod>(context->Get("method"));
  const std::string host = std::any_cast<std::string>(context->Get("host"));
  int port = std::any_cast<int>(context->Get("port"));
  const std::string requestUri =
      std::any_cast<std::string>(context->Get("requestUri"));

  simple_http_server::HttpRequest request;
  request.SetMethod(method);
  request.SetUri(simple_http_server::Uri(requestUri));

  std::string content = std::any_cast<std::string>(context->Get("content"));
  request.SetContent(content);

  //"multipart/form-data; boundary=" + boundary //get it from client side
  std::string content_type= std::any_cast<std::string>(context->Get("content_type"));
  request.SetHeader("Content-Type",content_type );

  auto requestStr = simple_http_server::to_string(request);
  CClientSocket *clientSocket = new CClientSocket(host, port);
  clientSocket->SendBytes((void *)requestStr.c_str(), requestStr.size());

  std::string buffer = clientSocket->ReceiveBytes();

  simple_http_server::HttpResponse httpResponse =
      simple_http_server::string_to_response(buffer);

  if (httpResponse.status_code() >=
      simple_http_server::HttpStatusCode::BadRequest) {
    std::string error_message =
        "HTTP request failed with status code: " +
        std::to_string(static_cast<int>(httpResponse.status_code()));
    LOG_ERROR(error_message);
    return false;
  }

  if (!httpResponse.content().empty()) {
    context->Put("response_content", httpResponse.content());
  }

  std::string response = "Successfully send request...\n";
  context->Put("response", response);

  return true;
}