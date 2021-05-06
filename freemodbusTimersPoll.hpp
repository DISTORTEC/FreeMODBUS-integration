/**
 * \file
 * \brief freemodbusTimersPoll() declaration
 *
 * \author Copyright (C) 2019 Aleksander Szczygiel https://distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_FREEMODBUSTIMERSPOLL_HPP_
#define FREEMODBUS_INTEGRATION_FREEMODBUSTIMERSPOLL_HPP_

#include "distortos/TickClock.hpp"

struct FreemodbusInstance;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Polls the timer until either timer's deadline or \a deadline, whichever comes first.
 *
 * \param [in] instance is a reference to instance of FreeMODBUS
 * \param [in] deadline is the deadline of polling operation
 *
 * \return next deadline of timer, distortos::TickClock::time_point::max() if timer is not enabled.
 */

distortos::TickClock::time_point freemodbusTimersPoll(FreemodbusInstance& instance,
		distortos::TickClock::time_point deadline);

#endif	// FREEMODBUS_INTEGRATION_FREEMODBUSTIMERSPOLL_HPP_
