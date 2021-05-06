/**
 * \file
 * \brief ListenSocket class header
 *
 * \author Copyright (C) 2019 Aleksander Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_LISTENSOCKET_HPP_
#define FREEMODBUS_INTEGRATION_INCLUDE_LISTENSOCKET_HPP_

#include "mbconfig.h"

#if MB_TCP_ENABLED == 1

#include <cstddef>
#include <cstdint>

/// ListenSocket represents a listen socket for Modbus TCP
class ListenSocket
{
public:

	/**
	 * \brief ListenSocket's constructor
	 *
	 * \param [in] backlogSize is the size of listen socket's backlog
	 */

	constexpr explicit ListenSocket(const int backlogSize) :
			backlogSize_{backlogSize},
			bindCounter_{},
			clientCounter_{},
			socket_{-1},
			port_{}
	{

	}

	/**
	 * \brief Binds FreeMODBUS instance, increases maximum allowed number of connections.
	 *
	 * \param [in] port is a port for Modbus TCP to open
	 *
	 * \return 0 on success, error code otherwise:
	 * - EBUSY - tried to bind already opened socket with wrong port number;
	 * - error codes returned by openSocket();
	 */

	int bind(uint16_t port);

	/**
	 * \brief Increases number of connected clients, closes listen socket when maximum allowed number of connections is
	 * reached.
	 *
	 * \return 0 on success, error code otherwise:
	 * - error codes returned by lwIP library;
	 */

	int clientConnected();

	/**
	 * \brief Decreases number of connected clients, opens listen socket when there are less than maximum allowed number
	 * of connections.
	 *
	 * \return 0 on success, error code otherwise:
	 * - error codes returned by openSocket();
	 */

	int clientDisconnected();

	/**
	 * \return current port of listen socket for Modbus TCP
	 */

	uint16_t getPort() const
	{
		return port_;
	}

	/**
	 * \return listen socket for Modbus TCP, -1 if no listen socket is created
	 */

	int getSocket() const
	{
		return socket_;
	}

	/**
	 * \brief Unbinds FreeMODBUS instance, decreases maximum allowed number of connections.
	 *
	 * \return 0 on success, error code otherwise:
	 * - error codes returned by lwIP library;
	 */

	int unbind();

private:

	/**
	 * \brief Opens listen socket.
	 *
	 * \param [in] port is a port for Modbus TCP to open
	 *
	 * \return 0 on success, error code otherwise:
	 * - error codes returned by lwIP library;
	 */

	int openSocket(uint16_t port);

	/// size of listen socket's backlog
	int backlogSize_;

	/// count of FreeMODBUS instances bound with this socket, aka maximum allowed number of connections
	size_t bindCounter_;

	/// count of TCP clients connected with this socket
	size_t clientCounter_;

	/// listen socket for Modbus TCP, -1 if no listen socket is created
	int socket_;

	/// current port of listen socket for Modbus TCP
	uint16_t port_;
};

#endif	// MB_TCP_ENABLED == 1

#endif	// FREEMODBUS_INTEGRATION_INCLUDE_LISTENSOCKET_HPP_
