#include "http/http_message.h"
#include "interface/CApiGatewaySocket.h"
#include "interface/CProtocolSocket.h"
#include "logger/Logger.h"
#include "socket/SocketSelect.h"
#include "uuid/UuidGenerator.h"

extern "C" void *HttpRedirectPipeline(CApiGatewaySocket *cProxySocket, void *lptr) {
	LOG_INFO("HttpRedirectPipeline::start");
	auto correlation_id = UuidGenerator::generateUuid();
	auto start = std::chrono::high_resolution_clock::now();

	CLIENT_DATA clientData;
	memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

	auto *client_socket = (Socket *)clientData.client_socket;
	if (client_socket == nullptr) {
		LOG_ERROR("The client socket is not defined. Exiting!");
		return nullptr;
	}

	EXECUTION_CONTEXT exec_context;

	ProtocolHelper::SetReadTimeOut(client_socket->GetSocket(), 1);

	while (true) {
		try {
			SocketSelect sel = SocketSelect(client_socket, nullptr, NonBlockingSocket);
			bool client_still_connected = true;

			try {
				if (sel.Readable(client_socket)) {
					LOG_INFO("client socket is readable, reading bytes : " + std::to_string(client_socket->GetSocket()));
					std::string input_bytes = client_socket->ReceiveBytes();
					if (!input_bytes.empty()) {
						LOG_INFO("Calling Proxy Upstream Handler..");

						// Get Hostname
						simple_http_server::HttpRequest request = simple_http_server::string_to_request(input_bytes);
						std::string client_source = request.GetHeader("Host");
						std::string source_hostname = Utils::split(client_source, ":")[0];
						std::string source_uri_path = request.uri().path();

						// Get RESOLVED_SERVICE for the hostname
						auto hostname_map = cProxySocket->hostname_map;
						std::map<std::string, RESOLVED_SERVICE> uri_map = hostname_map.find(source_hostname)->second;
						std::ostringstream ss;
						ss << std::this_thread::get_id() << " - " << client_socket->GetSocket();
						std::string identifier = ss.str();

						LOG_INFO(identifier + ": source_hostname : " + source_hostname);
						LOG_INFO(identifier + ": source_uri : " + source_uri_path);

						END_POINT target_endpoint;
						auto uri_iterator = uri_map.end();

						// Check if any uri path in our configuration matches the source_uri
						for (auto iter = uri_map.begin(); iter != uri_map.end(); ++iter) {
							std::string key = iter->first;
							if (source_uri_path.rfind(key, 0) == 0) {
								LOG_INFO("Found config matching uri path : " + source_uri_path + " => " + key);
								uri_iterator = iter;
								break;
							}
						}

						// Assign the wildcard path configuration, since we did not match any uri path
						if (uri_iterator == uri_map.end()) {
							uri_iterator = uri_map.find("*");
						}

						// If we did not find any matching uri path ( even wildcard uri path )
						if (uri_iterator == uri_map.end()) {
							LOG_ERROR("URI path does not match any path in API Gateway configuration")
							delete client_socket;
							return nullptr;
						}

						target_endpoint.ipaddress = uri_iterator->second.ipaddress;
						target_endpoint.port = uri_iterator->second.port;

						LOG_INFO(identifier + ": Resolved (Target) Host: " + target_endpoint.ipaddress);
						LOG_INFO(identifier + ": Resolved (Target) Port: " + std::to_string(target_endpoint.port));

						CClientSocket *target_socket;
						try {
							target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);
						} catch (std::exception &e) {
							LOG_ERROR(identifier + std::string("Error connecting to target : ") + e.what());
							delete client_socket;
							return nullptr;
						}
						identifier += "- " + std::to_string(target_socket->GetSocket()) + " : ";

						// Send initial packet to target
						target_socket->SendBytes(&input_bytes[0], input_bytes.size());

						int count = 0;
						while (true) {
							try {
								SocketSelect sel2 = SocketSelect(target_socket, nullptr, NonBlockingSocket);
								bool target_still_connected = true;

								try {
									if (sel2.Readable(target_socket)) {
										LOG_INFO("target socket is readable, reading bytes : ");
										std::string output_bytes = target_socket->ReceiveBytes();

										if (!output_bytes.empty()) {
											client_socket->SendBytes((char *)output_bytes.c_str(), output_bytes.size());
										}

										if (output_bytes.empty())
											target_still_connected = false;
									}

									/* TODO: This is done to exit after a timeout, else will be stuck in the loop as socket won't
									 * be readable. */
									if (count++ == 5000)
										target_still_connected = false;
								} catch (std::exception &e) {
									LOG_ERROR("Error while sending to client " + std::string(e.what()));
									target_still_connected = false;
								}

								if (!target_still_connected) {
									// Close the client socket
									LOG_INFO("Closing the target socket");
									delete target_socket;
									break;
								}
							} catch (std::exception &e) {
								LOG_ERROR(e.what());
								LOG_ERROR("error occurred while creating socket select ");
							}
						}
					}

					if (input_bytes.empty())
						client_still_connected = false;
				}
			} catch (std::exception &e) {
				LOG_ERROR("Error while reading from client " + std::string(e.what()));
				client_still_connected = false;
			}

			if (!client_still_connected) {
				// Close the client socket
				LOG_INFO("Closing the client socket");
				delete client_socket;
				break;
			}
		} catch (std::exception &e) {
			LOG_ERROR(e.what());
			LOG_ERROR("error occurred while creating socket select ");
		}
	}

	LOG_INFO("HttpRedirectPipeline::end");
#ifdef WINDOWS_OS
	return 0;
#else
	return nullptr;
#endif
}
