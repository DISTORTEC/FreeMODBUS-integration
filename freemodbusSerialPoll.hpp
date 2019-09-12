/**
 * \file
 * \brief freemodbusSerialPoll() declaration
 *
 * \author Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_FREEMODBUSSERIALPOLL_HPP_
#define FREEMODBUS_INTEGRATION_FREEMODBUSSERIALPOLL_HPP_

#include "distortos/TickClock.hpp"

struct FreemodbusInstance;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Polls serial port.
 *
 * \param [in] instance is a reference to instance of FreeMODBUS
 * \param [in] deadline is the deadline of polling operation
 */

void freemodbusSerialPoll(FreemodbusInstance& instance, distortos::TickClock::time_point deadline);

#endif	// FREEMODBUS_INTEGRATION_FREEMODBUSSERIALPOLL_HPP_
