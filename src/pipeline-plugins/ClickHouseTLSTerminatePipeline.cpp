#include "interface/CProtocolSocket.h"
#include "interface/CProxySocket.h"
#include "logger/Logger.h"
#include "socket/CClientSocket.h"
#include "socket/SSLSocket.h"
#include "socket/Socket.h"
#include "socket/SocketSelect.h"

/**
 * TLS Termination pipeline :
 * The proxy serves a TLS certificate and connection to the client is
 * via TLS. The TLS connection is terminated here and the decrypted
 * packets are transferred to the target endpoint via TCP/HTTP.
 */
extern "C" void *ClickHouseTLSTerminatePipeline(CProxySocket *ptr, void *lptr) {
	LOG_INFO("::start");
	CLIENT_DATA clientData;
	memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

	// Retrieve the load balancer class
	auto loadBalancer = ptr->loadBalancer;

	// Check if handler is defined
	CProxyHandler *proxy_handler = ptr->GetHandler();
	if (proxy_handler == nullptr) {
		LOG_ERROR("The handler is not defined. Exiting!");
		return nullptr;
	}

	RESOLVED_SERVICE currentService = loadBalancer->requestServer();
	END_POINT target_endpoint = END_POINT{currentService.ipaddress, currentService.port,	 currentService.r_w,
										  currentService.alias,		currentService.reserved, "  "};

	LOG_INFO("Resolved (Target) Host: " + target_endpoint.ipaddress);
	LOG_INFO("Resolved (Target) Port: " + std::to_string(target_endpoint.port));

	auto *client_socket = new SSLSocket(((Socket *)clientData.client_socket)->GetSocket());
	auto *target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);

	EXECUTION_CONTEXT exec_context;

	ProtocolHelper::SetReadTimeOut(client_socket->GetSocket(), 1);
	ProtocolHelper::SetKeepAlive(client_socket->GetSocket(), 1);
	ProtocolHelper::SetReadTimeOut(target_socket->GetSocket(), 1);
	ProtocolHelper::SetKeepAlive(target_socket->GetSocket(), 1);

	while (true) {
		try {
			SocketSelect sel(client_socket, target_socket, NonBlockingSocket);
			bool still_connected = true;
			try {
				if (sel.Readable(client_socket)) {
					LOG_INFO("client socket is readable, reading bytes : ")
					std::string bytes = client_socket->ReceiveBytes();

					LOG_INFO("Calling Proxy Upstream Handler..")
					std::string response = proxy_handler->HandleUpstreamData(bytes, bytes.size(), &exec_context);
					target_socket->SendBytes((char *)response.c_str(), response.size());

					if (bytes.empty())
						still_connected = false;
				}
			} catch (std::exception &e) {
				LOG_ERROR("Error while sending to target " + std::string(e.what()));
			}

			try {
				if (sel.Readable(target_socket)) {
					LOG_INFO("target socket is readable, reading bytes : ")
					std::string bytes = target_socket->ReceiveBytes();

					LOG_INFO("Calling Proxy Downstream Handler..")
					std::string response = proxy_handler->HandleDownStreamData(bytes, bytes.size(), &exec_context);
					client_socket->SendBytes((char *)response.c_str(), response.size());

					if (bytes.empty())
						still_connected = false;
				}
			} catch (std::exception &e) {
				LOG_ERROR("Error while sending to client " + std::string(e.what()));
			}

			if (!still_connected) {
				break;
			}
		} catch (std::exception &e) {
			LOG_ERROR("error occurred while creating socket select " + std::string(e.what()));
		}
	}

	// Close the client socket
	delete client_socket;
	// Close the server socket
	delete target_socket;
	LOG_INFO("::end");
#ifdef WINDOWS_OS
	return 0;
#else
	return nullptr;
#endif
}
