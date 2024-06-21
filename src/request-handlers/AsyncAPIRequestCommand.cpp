//
// Created by sinsinan on 19/06/24.
//
#include <sstream>

#include "socket/CClientSocket.h"
#include "HTTPRequestCommand.h"
#include "logger/Logger.h"
#include "AsyncAPIRequestCommand.h"

extern "C" AsyncAPIRequestCommand *createAsyncAPIRequestCommand() {
  return new AsyncAPIRequestCommand;
}

extern "C" void AsyncAPIRequestCommand(AsyncAPIRequestCommand *c) { delete c; }

bool AsyncAPIRequestCommand::Execute(ComputationContext *context) {
  LOG_INFO("AsyncAPIRequestCommand");

  const std::string host = context->GetString("host");
  const int port = std::any_cast<int>(context->Get("port"));

  const std::string serialized_request_packet = context->GetString("serialized_request_packet");

  LOG_INFO("host: "+ host + " ,port: " + std::to_string(port) + " ,str: " + serialized_request_packet);

  CClientSocket *clientSocket = new CClientSocket(host, port);
  clientSocket->SendBytes((void *) serialized_request_packet.c_str(), serialized_request_packet.size());

  std::string buffer = clientSocket->ReceiveBytes();

  context->Put("serialized_response_packet", buffer);

  return true;
}