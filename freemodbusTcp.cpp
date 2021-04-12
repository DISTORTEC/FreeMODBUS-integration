/**
 * \file
 * \brief Definitions of TCP-related functions for FreeMODBUS
 *
 * \author Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freemodbusTcpPoll.hpp"

#if MB_TCP_ENABLED == 1

#include "FreemodbusInstance.hpp"
#include "ListenSocket.hpp"

#include "mbport.h"

#include "lwip/sockets.h"

#include "distortos/assert.h"
#include "distortos/Mutex.hpp"

#include "estd/ScopeGuard.hpp"

#include <mutex>

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local objects
+---------------------------------------------------------------------------------------------------------------------*/

/// default port for Modbus TCP
constexpr uint16_t defaultPort {502};

/// index of high byte of frame length in MBAP header
constexpr size_t frameLengthHigh {4};

/// index of low byte of frame length in MBAP header
constexpr size_t frameLengthLow {frameLengthHigh + 1};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Releases client socket from FreemodbusInstance.
 *
 * \param [in] freemodbusInstance is a reference to FreemodbusInstance from which client socket will be released
 */

void releaseClientSocket(FreemodbusInstance& freemodbusInstance)
{
	{
		assert(freemodbusInstance.listenSocket != nullptr);
		assert(freemodbusInstance.listenSocketsRangeMutex != nullptr);

		std::lock_guard<distortos::Mutex> lockGuard {*freemodbusInstance.listenSocketsRangeMutex};

		const auto ret = freemodbusInstance.listenSocket->clientDisconnected() != 0;
		if (ret != 0)
			return;
	}

	while (lwip_recv(freemodbusInstance.clientSocket, freemodbusInstance.frameBuffer,
			sizeof(freemodbusInstance.frameBuffer), MSG_DONTWAIT) > 0);
	lwip_close(freemodbusInstance.clientSocket);
	freemodbusInstance.clientSocket = -1;
}

/**
 * \brief Disconnects client if Modbus TCP keepalive deadline has expired.
 *
 * \param [in] freemodbusInstance is a reference to FreemodbusInstance from which client socket will be released
 */

void checkKeepalive(FreemodbusInstance& freemodbusInstance)
{
	// if client is disconnected do nothing
	if (freemodbusInstance.clientSocket == -1)
		return;

	// if keepalive is disabled do nothing
	if (freemodbusInstance.tcpKeepaliveDuration == distortos::TickClock::duration{})
		return;

	if (freemodbusInstance.tcpKeepaliveDeadline < distortos::TickClock::now())
		releaseClientSocket(freemodbusInstance);
}

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

void freemodbusTcpPoll(FreemodbusInstance& instance, const distortos::TickClock::time_point deadline)
{
	assert(instance.listenSocket != nullptr);

	distortos::TickClock::duration left;
	while ((left = deadline - distortos::TickClock::now()) >= distortos::TickClock::duration{})
	{
		auto keepaliveScopeGuard = estd::makeScopeGuard(
				[&instance]()
				{
					checkKeepalive(instance);
				});

		fd_set fdSet;
		FD_ZERO(&fdSet);
		if (instance.clientSocket != -1)
			FD_SET(instance.clientSocket, &fdSet);
		else if (instance.listenSocket->getSocket() != -1)
			FD_SET(instance.listenSocket->getSocket(), &fdSet);

		{
			const auto leftSeconds = std::chrono::duration_cast<std::chrono::seconds>(left);
			const auto leftMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(left - leftSeconds);
			timeval timeout {};
			timeout.tv_sec = leftSeconds.count();
			timeout.tv_usec = leftMicroseconds.count();
			if (lwip_select(std::max(instance.clientSocket, instance.listenSocket->getSocket()) + 1, &fdSet, nullptr,
					nullptr, &timeout) <= 0)
				return;
		}

		if (instance.clientSocket != -1 && FD_ISSET(instance.clientSocket, &fdSet) != 0)
		{
			const uint16_t length = instance.bytesInBuffer < FreemodbusInstance::mbapHeaderSize - 1 ? 0 :
					((instance.frameBuffer[frameLengthHigh] << 8) | instance.frameBuffer[frameLengthLow]);
			const size_t totalSize = FreemodbusInstance::mbapHeaderSize - 1 + length;
			if (totalSize > sizeof(instance.frameBuffer))
			{
				instance.bytesInBuffer = 0;
				releaseClientSocket(instance);
				continue;
			}

			const auto ret = lwip_recv(instance.clientSocket, &instance.frameBuffer[instance.bytesInBuffer],
					totalSize - instance.bytesInBuffer, {});
			if (ret <= 0)
			{
				instance.bytesInBuffer = 0;
				releaseClientSocket(instance);
				continue;
			}
			else
			{
				instance.bytesInBuffer += ret;
				if (totalSize >= FreemodbusInstance::mbapHeaderSize && instance.bytesInBuffer == totalSize)
				{
					instance.tcpKeepaliveDeadline = distortos::TickClock::now() + instance.tcpKeepaliveDuration;
					xMBPortEventPost(&instance.rawInstance, EV_FRAME_RECEIVED);
					keepaliveScopeGuard.release();
					return;
				}
			}
		}
		if (instance.listenSocket->getSocket() != -1 && FD_ISSET(instance.listenSocket->getSocket(), &fdSet) != 0)
		{
			const auto clientSocket = lwip_accept(instance.listenSocket->getSocket(), nullptr, nullptr);
			if (clientSocket == -1)
				return;

			auto closeScopeGuard = estd::makeScopeGuard(
					[clientSocket]()
					{
						lwip_close(clientSocket);
					});

			instance.tcpKeepaliveDeadline = distortos::TickClock::now() + instance.tcpKeepaliveDuration;

			{
				assert(instance.listenSocketsRangeMutex != nullptr);

				std::lock_guard<distortos::Mutex> lockGuard {*instance.listenSocketsRangeMutex};

				const auto ret = instance.listenSocket->clientConnected();
				if (ret != 0)
					return;
			}

			closeScopeGuard.release();
			instance.clientSocket = clientSocket;
			keepaliveScopeGuard.release();
		}
	}
}

extern "C" void vMBTCPPortClose(xMBInstance* const instance)
{
	assert(instance != nullptr);
	vMBTCPPortDisable(instance);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	{
		assert(freemodbusInstance.listenSocket != nullptr);
		assert(freemodbusInstance.listenSocketsRangeMutex != nullptr);

		std::lock_guard<distortos::Mutex> lockGuard {*freemodbusInstance.listenSocketsRangeMutex};

		freemodbusInstance.listenSocket->unbind();
	}

	freemodbusInstance.listenSocket = nullptr;
}

extern "C" void vMBTCPPortDisable(xMBInstance* const instance)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	if (freemodbusInstance.clientSocket != -1)
		releaseClientSocket(freemodbusInstance);
}

extern "C" bool xMBTCPPortGetRequest(xMBInstance* const instance, uint8_t** const frame, uint16_t* const length)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	*frame = freemodbusInstance.frameBuffer;
	*length = freemodbusInstance.bytesInBuffer;
	freemodbusInstance.bytesInBuffer = {};
	return true;
}

extern "C" bool xMBTCPPortInit(xMBInstance* const instance, const uint16_t port)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	assert(freemodbusInstance.listenSocket == nullptr);
	assert(freemodbusInstance.listenSocketsRangeMutex != nullptr);
	const auto realPort = port != 0 ? port : defaultPort;

	std::lock_guard<distortos::Mutex> lockGuard {*freemodbusInstance.listenSocketsRangeMutex};

	auto chosenListenSocket = std::find_if(freemodbusInstance.listenSocketsRange.begin(),
			freemodbusInstance.listenSocketsRange.end(),
			[realPort](const ListenSocket& listenSocket) -> bool
			{
				return listenSocket.getPort() == realPort;
			});

	if (chosenListenSocket == freemodbusInstance.listenSocketsRange.end())
		chosenListenSocket = std::find_if(freemodbusInstance.listenSocketsRange.begin(),
				freemodbusInstance.listenSocketsRange.end(),
				[](const ListenSocket& listenSocket) -> bool
				{
					return listenSocket.getPort() == 0;
				});

	if (chosenListenSocket == freemodbusInstance.listenSocketsRange.end())
		return false;

	if (chosenListenSocket->bind(realPort) != 0)
		return false;

	freemodbusInstance.listenSocket = chosenListenSocket;
	return true;
}

extern "C" bool xMBTCPPortSendResponse(xMBInstance* const instance, const uint8_t* const frame, const uint16_t length)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	const auto ret = lwip_send(freemodbusInstance.clientSocket, frame, length, {});
	if (ret != length)
	{
		releaseClientSocket(freemodbusInstance);
		return false;
	}

	return true;
}

#endif	// MB_TCP_ENABLED == 1
