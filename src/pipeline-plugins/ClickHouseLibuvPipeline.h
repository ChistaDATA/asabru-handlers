//
// Created by Midhun Darvin on 02/05/24.
//

#ifndef CHISTA_ASABRU_CLICKHOUSELIBUVPIPELINE_H
#define CHISTA_ASABRU_CLICKHOUSELIBUVPIPELINE_H

#include "interface/CProtocolSocket.h"
#include "interface/LibuvProxySocket.h"

typedef struct {
	Socket *client_socket;
	Socket *target_socket;
	std::string target_address;
	int target_port;
} Connection;

typedef struct {
	ClientTargetPair *pair;
	CProxyHandler *proxy_handler;
} ConnectionData;

#endif // CHISTA_ASABRU_CLICKHOUSELIBUVPIPELINE_H
