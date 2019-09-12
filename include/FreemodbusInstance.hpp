/**
 * \file
 * \brief FreemodbusInstance struct header
 *
 * \author Copyright (C) 2019 Aleksander Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSINSTANCE_HPP_
#define FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSINSTANCE_HPP_

#include "mbinstance.h"

#include "distortos/TickClock.hpp"

#include "estd/ContiguousRange.hpp"

namespace distortos
{

class Mutex;

namespace devices
{

class SerialPort;

}	// namespace devices

}	// namespace distortos

class ListenSocket;

/// FreemodbusInstance struct is an instance of FreeMODBUS
struct FreemodbusInstance
{
	/// size of MBAP header of Modbus TCP
	constexpr static size_t mbapHeaderSize {7};

	/// size of buffer for complete Modbus TCP frame
	constexpr static size_t tcpBufferSize {mbapHeaderSize + MB_SER_SIZE_MAX};

	/// type alias for range of listen sockets for Modbus TCP
	using ListenSocketsRange = estd::ContiguousRange<ListenSocket>;

	/// SerialMode contains possible modes of serial port
	enum class SerialMode : uint8_t
	{
		/// serial port disabled
		disabled,
		/// serial port is in receiver mode
		receiver,
		/// serial port is in transmiter mode
		transmiter,
	};

	/**
	 * \brief FreemodbusInstance's constructor
	 *
	 * \param [in] serialPortt is a pointer to serial port that will be used for communication for Modbus ASCII/RTU,
	 * ignored for Modbus TCP
	 * \param [in] listenSocketsRangee is a range of listen sockets for Modbus TCP, ignored for Modbus ASCII/RTU
	 * \param [in] listenSocketsRangeMutexx is a pointer to mutex used for serialization of access to shared listen
	 * sockets for Modbus TCP, ignored for Modbus ASCII/RTU
	 */

	constexpr FreemodbusInstance(distortos::devices::SerialPort* const serialPortt,
			const ListenSocketsRange listenSocketsRangee, distortos::Mutex* const listenSocketsRangeMutexx) :
					rawInstance{},
					listenSocketsRange{listenSocketsRangee},
					tcpKeepaliveDeadline{},
					tcpKeepaliveDuration{},
					timerDeadline{distortos::TickClock::time_point::max()},
					timerDuration{},
					bytesInBuffer{},
					clientSocket{-1},
					listenSocket{},
					listenSocketsRangeMutex{listenSocketsRangeMutexx},
					serialPort{serialPortt},
					rxPosition{},
					txPosition{},
					frameBuffer{},
					pendingEvents{},
					serialMode{SerialMode::disabled}
	{

	}

	/// instance of FreeMODBUS
	xMBInstance rawInstance;

	/// range of listen sockets for Modbus TCP
	ListenSocketsRange listenSocketsRange;

	/// deadline of Modbus TCP keepalive
	distortos::TickClock::time_point tcpKeepaliveDeadline;

	/// duration of Modbus TCP keepalive
	distortos::TickClock::duration tcpKeepaliveDuration;

	/// timer deadline
	distortos::TickClock::time_point timerDeadline;

	/// timer duration
	distortos::TickClock::duration timerDuration;

	/// number of bytes stored in buffer
	size_t bytesInBuffer;

	/// client socket for Modbus TCP, -1 if no client is connected
	int clientSocket;

	/// listen socket for Modbus TCP
	ListenSocket* listenSocket;

	/// pointer to mutex used for serialization of access to shared listen sockets for Modbus TCP
	distortos::Mutex* listenSocketsRangeMutex;

	/// pointer to serial port that will be used for communication for Modbus ASCII/RTU
	distortos::devices::SerialPort* serialPort;

	/// current receiver position
	size_t rxPosition;

	/// current transmiter position
	size_t txPosition;

	/// buffer for bytes
	uint8_t frameBuffer[tcpBufferSize];

	/// array with counters of pending events
	std::array<uint8_t, 4> pendingEvents;

	/// current mode of serial port
	SerialMode serialMode;
};

#endif	// FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSINSTANCE_HPP_
