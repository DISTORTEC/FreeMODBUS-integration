/**
 * \file
 * \brief ListenSocket class implementatnion
 *
 * \author Copyright (C) 2019 Aleksander Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ListenSocket.hpp"

#if MB_TCP_ENABLED == 1

#include "lwip/sockets.h"

#include "distortos/assert.h"

#include "estd/ScopeGuard.hpp"

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

int ListenSocket::bind(const uint16_t port)
{
	if (socket_ != -1 && port_ != port)
		return EBUSY;

	if (socket_ == -1)
	{
		const auto ret = openSocket(port);
		if (ret != 0)
			return ret;
	}

	++bindCounter_;
	return 0;
}

int ListenSocket::clientConnected()
{
	assert(clientCounter_ < bindCounter_);

	if (clientCounter_ + 1 == bindCounter_)
	{
		const auto ret = lwip_close(socket_);
		socket_ = -1;
		if (ret == -1)
			return errno;
	}

	++clientCounter_;
	return 0;
}

int ListenSocket::clientDisconnected()
{
	assert(clientCounter_ > 0);

	if (clientCounter_ == bindCounter_ && socket_ == -1)
	{
		const auto ret = openSocket(port_);
		if (ret != 0)
			return ret;
	}

	--clientCounter_;
	return 0;
}

int ListenSocket::unbind()
{
	assert(bindCounter_ > 0);

	if (clientCounter_ >= bindCounter_ - 1 || bindCounter_ == 1)
	{
		if (socket_ != -1)
		{
			const auto ret = lwip_close(socket_);
			socket_ = -1;
			if (ret == -1)
				return errno;
		}
	}

	--bindCounter_;
	if (bindCounter_ == 0)
		port_ = {};

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

int ListenSocket::openSocket(const uint16_t port)
{
	const auto listenSocket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == -1)
		return errno;

	auto closeScopeGuard = estd::makeScopeGuard(
			[listenSocket]()
			{
				lwip_close(listenSocket);
			});

	{
		const int optionValue = 1;
		if (lwip_setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)) == -1)
			return errno;
	}
	{
		sockaddr_in serverAddress {};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);
		if (lwip_bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1)
			return errno;
	}

	if (lwip_listen(listenSocket, backlogSize_) == -1)
		return errno;

	sockaddr_in serverAddress {};
	socklen_t length = sizeof(serverAddress);
	if (lwip_getsockname(listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), &length) == -1)
		return errno;

	closeScopeGuard.release();

	socket_ = listenSocket;
	port_ = ntohs(serverAddress.sin_port);
	return 0;
}

#endif	// MB_TCP_ENABLED == 1
