#include "http/http_message.h"
#include "interface/CProtocolSocket.h"
#include "interface/CProxySocket.h"
#include "logger/Logger.h"
#include "socket/SocketSelect.h"
#include "uuid/UuidGenerator.h"

extern "C" void *HttpRedirectPipeline(CProxySocket *cProxySocket, void *lptr)
{
	LOG_INFO("HttpRedirectPipeline::start");
	auto correlation_id = UuidGenerator::generateUuid();
	auto start = std::chrono::high_resolution_clock::now();

	CLIENT_DATA clientData;
	memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

	// Check if handler is defined
	CProxyHandler *proxy_handler = cProxySocket->GetHandler();
	if (proxy_handler == nullptr)
	{
		LOG_ERROR("The handler is not defined. Exiting!");
		return nullptr;
	}

	auto *client_socket = (Socket *)clientData.client_socket;
	if (client_socket == nullptr)
	{
		LOG_ERROR("The client socket is not defined. Exiting!");
		return nullptr;
	}

	// Receive Initial HTTP packet
	std::string initial_packet = client_socket->ReceiveBytes();

	if (initial_packet.empty()) {
		// Close the client socket
		LOG_INFO("Closing the client socket");
		delete client_socket;
		return nullptr;
	}

	// Get Hostname
	simple_http_server::HttpRequest request = simple_http_server::string_to_request(initial_packet);
	std::string client_source = request.GetHeader("Host");
	std::string source_hostname = Utils::split(client_source, ":")[0];

	// Get RESOLVED_SERVICE for the hostname
	auto hostname_map = ((CProxySocket *)clientData.ptr_to_instance)->hostname_map;
	RESOLVED_SERVICE currentService = hostname_map->find(source_hostname)->second;
	END_POINT target_endpoint {
		currentService.ipaddress, currentService.port, currentService.r_w, currentService.alias, currentService.reserved, "  "
	};

	LOG_INFO("Resolved (Target) Host: " + target_endpoint.ipaddress);
	LOG_INFO("Resolved (Target) Port: " + std::to_string(target_endpoint.port));

	CClientSocket *target_socket;
	try {
		target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		LOG_ERROR(std::string("Error connecting to target : ") + e.what());
		delete client_socket;
		return nullptr;
	}

	// Send initial packet to target
	target_socket->SendBytes(&initial_packet[0], initial_packet.size());

	// Receive bytes from target
	std::string response = target_socket->ReceiveBytes();

	// Send response to client
	client_socket->SendBytes(&response[0], response.size());

	EXECUTION_CONTEXT exec_context;

	ProtocolHelper::SetReadTimeOut(client_socket->GetSocket(), 1);
	ProtocolHelper::SetReadTimeOut(target_socket->GetSocket(), 1);

	while (true)
	{
		SocketSelect *sel;
		try
		{
			sel = new SocketSelect(client_socket, target_socket, NonBlockingSocket);
		}
		catch (std::exception &e)
		{
			LOG_ERROR(e.what());
			LOG_ERROR("error occurred while creating socket select ");
		}

		bool still_connected = true;

		try
		{
			if (sel->Readable(target_socket))
			{
				LOG_INFO("target socket is readable, reading bytes : ");
				std::string bytes = target_socket->ReceiveBytes();

				if (!bytes.empty()) {
					client_socket->SendBytes((char *) bytes.c_str(), bytes.size());
				}

				if (bytes.empty())
					still_connected = false;
			}
		}
		catch (std::exception &e)
		{
			LOG_ERROR("Error while sending to client " + std::string(e.what()));
			still_connected = false;
		}

		try {
			if (sel->Readable(client_socket)) {
				LOG_INFO("client socket is readable, reading bytes : ");
				std::string bytes = client_socket->ReceiveBytes();

				if (!bytes.empty()) {
					LOG_INFO("Calling Proxy Upstream Handler..");

					start = std::chrono::high_resolution_clock::now();
					target_socket->SendBytes((char *)bytes.c_str(), bytes.size());
				}

				if (bytes.empty())
					still_connected = false;
			}
		} catch (std::exception &e) {
			LOG_ERROR("Error while sending to target " + std::string(e.what()));
			still_connected = false;
		}

		if (!still_connected)
		{
			// Delete Select from memory
			delete sel;

			// Close the client socket
			LOG_INFO("Closing the client socket");
			delete client_socket;
			break;
		}
	}
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	// Close the server socket
	LOG_INFO("Closing the target socket");
	delete target_socket;

	LOG_INFO("HttpRedirectPipeline::end");
	LOG_LATENCY(
		correlation_id,
		std::to_string(duration.count())+ "," +target_endpoint.ipaddress+":"+std::to_string(target_endpoint.port));
#ifdef WINDOWS_OS
	return 0;
#else
	return nullptr;
#endif
}
