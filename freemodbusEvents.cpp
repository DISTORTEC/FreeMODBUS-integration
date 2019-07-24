/**
 * \file
 * \brief Definitions of events-related functions for FreeMODBUS
 *
 * \author Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FreemodbusInstance.hpp"
#include "freemodbusTcpPoll.hpp"
#include "freemodbusTimersPoll.hpp"

#include "mbport.h"

#include "distortos/assert.h"

#include "estd/ReverseAdaptor.hpp"

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local objects
+---------------------------------------------------------------------------------------------------------------------*/

/// max duration of "push" and "pop" operations
constexpr distortos::TickClock::duration maxDuration {std::chrono::milliseconds{100}};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Tries to get a FreeMODBUS event, internal version
 *
 * \param [in] instance is a reference to instance of FreeMODBUS
 * \param [in] event is a reference to buffer for read event
 *
 * \return true if the event was received, false otherwise
 */

bool getEventInternal(FreemodbusInstance& instance, eMBEventType& event)
{
	// reverse order, as "high" events have higher priority
	for (auto& checkedEvent : estd::makeReverseAdaptor(instance.events))
		if (checkedEvent != 0)
		{
			--checkedEvent;
			event = static_cast<eMBEventType>(&checkedEvent - instance.events.begin());
			return true;
		}

	return false;
}

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

extern "C" bool xMBPortEventGet(xMBInstance* const instance, eMBEventType* const event)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	if (getEventInternal(freemodbusInstance, *event) == true)
		return true;

	assert(instance->eMBCurrentMode == MB_TCP);
	freemodbusTcpPoll(freemodbusInstance, distortos::TickClock::now() + maxDuration);

	return getEventInternal(freemodbusInstance, *event);
}

extern "C" bool xMBPortEventInit(xMBInstance* const instance)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	for (auto& event : freemodbusInstance.events)
		event = {};

	return true;
}

extern "C" bool xMBPortEventPost(xMBInstance* const instance, const eMBEventType event)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	if (freemodbusInstance.events[event] ==
			std::numeric_limits<std::decay<decltype(freemodbusInstance.events[event])>::type>::max())
		return false;	// overflow

	++(freemodbusInstance.events)[event];

	return true;
}
