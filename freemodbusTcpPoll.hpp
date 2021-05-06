/**
 * \file
 * \brief freemodbusTcpPoll() declaration
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_FREEMODBUSTCPPOLL_HPP_
#define FREEMODBUS_INTEGRATION_FREEMODBUSTCPPOLL_HPP_

#include "mbconfig.h"

#if MB_TCP_ENABLED == 1

#include "distortos/TickClock.hpp"

struct FreemodbusInstance;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Polls TCP connections
 *
 * \param [in] instance is a reference to instance of FreeMODBUS
 * \param [in] deadline is the deadline of polling operation
 */

void freemodbusTcpPoll(FreemodbusInstance& instance, distortos::TickClock::time_point deadline);

#endif	// MB_TCP_ENABLED == 1

#endif	// FREEMODBUS_INTEGRATION_FREEMODBUSTCPPOLL_HPP_
