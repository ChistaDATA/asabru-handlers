//
// Created by JOSEPH ABRAHAM on 21/03/24.
//
#include <sstream>

#include "Logger.h"
#include "HTTPBasicLoginRequestCommand.h"
#include "AuthenticationStrategy.h"

extern "C" HTTPBasicLoginRequestCommand *createHTTPBasicLoginRequestCommand()
{
  return new HTTPBasicLoginRequestCommand;
}

extern "C" void HTTPBasicLoginRequestCommand(HTTPBasicLoginRequestCommand *c)
{
  delete c;
}

bool HTTPBasicLoginRequestCommand::Execute(ComputationContext *context)
{
//  auto *request = std::any_cast<const simple_http_server::HttpRequest *>(context->Get("request"));
//  auto *response = new simple_http_server::HttpResponse(simple_http_server::HttpStatusCode::Ok);
//
//  nlohmann::json body = nlohmann::json::parse(request->content());
//  std::string username = body["username"].get<std::string>();
//  std::string password = body["password"].get<std::string>();
//
//  auto *auth = std::any_cast<AuthenticationStrategy *>(context->Get("authentication"));
//  ComputationContext auth_context;
//  auth_context.Put("username", username);
//  auth_context.Put("password", password);
//  auth->authenticate(&auth_context);
//
//  auto authenticated = std::any_cast<bool>(auth_context.Get("authenticated"));
//  if (!authenticated) {
//    response->SetStatusCode(simple_http_server::HttpStatusCode::Unauthorized);
//    response->SetHeader("Content-Type", "application/json");
//    response->SetContent("Unauthorized");
//    context->Put("response", response);
//    return false;
//  }
//
//  std::string token = std::any_cast<std::string>(auth_context.Get("token"));
//  nlohmann::json response_content;
//  response_content["token"] = token;
//
//  response->SetHeader("Content-Type", "application/json");
//  response->SetContent(response_content.dump());
 LOG_INFO("HTTPBasicLoginRequestCommand");
  //auto requestStr = std::any_cast<std::string>(context->Get("request"));

  simple_http_server::HttpRequest request;
  request.SetMethod(simple_http_server::HttpMethod::POST);
  request.SetUri(simple_http_server::Uri("/auth/login"));

 const std::string username=std::any_cast<std::string>(context->Get("username"));
 //const std::string &password=std::any_cast<std::string>(context->Get("password"));
// int port = std::any_cast<int>(context->Get("port"));


  //const std::string username="admin";
  const std::string password="adminpwd";
  int port = 8080;

  //request.SetContent("{\"username\":\"admin\",\"password\":\"adminpwd\"}");
  //request.SetContent("{\"username\":\"" << username << "\",\"password\":\"" << password << "\"}");

 // Constructing the JSON string with variables
 std::ostringstream jsonContentStream;
 jsonContentStream << "{\"username\":\"" << username << "\",\"password\":\"" << password << "\"}";
 std::string jsonContent = jsonContentStream.str();
 std::cout << "Json Data : " << jsonContent <<std::endl;
 request.SetContent(jsonContent);

  auto requestStr = simple_http_server::to_string(request);
  LOG_INFO("Request : " + requestStr + "\n");
  TcpClient* _client = new TcpClient("localhost", port);
  _client->sendTcpMessage(requestStr);
  sleep(5);
  auto result=_client->ReceiveBytes();
  LOG_INFO("Result : " + result + "\n");
  simple_http_server::HttpResponse httpResponse =  simple_http_server::string_to_response(result);

  //std::string token = std::any_cast<std::string>(context->Get("token"));
  //context->Put("token", token);

  if(httpResponse.status_code() == simple_http_server::HttpStatusCode::Ok)
  {
      nlohmann::json body = nlohmann::json::parse(httpResponse.content());
      std::string token  = body["token"];
      std::string response = "Successfully send request...\n";
      context->Put("response", response);
      context->Put("token",token);

      return true;
  }
  else
  {
      return  false;
  }



}