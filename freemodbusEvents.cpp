/**
 * \file
 * \brief Definitions of events-related functions for FreeMODBUS
 *
 * \author Copyright (C) 2019-2022 Kamil Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FreemodbusInstance.hpp"
#include "freemodbusSerialPoll.hpp"
#include "freemodbusTcpPoll.hpp"
#include "freemodbusTimersPoll.hpp"

#include "mbport.h"

#include "estd/ReverseAdaptor.hpp"

#include <cassert>

namespace
{

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
	for (auto& checkedEvent : estd::makeReverseAdaptor(instance.pendingEvents))
		if (checkedEvent != 0)
		{
			--checkedEvent;
			event = static_cast<eMBEventType>(&checkedEvent - instance.pendingEvents.begin());
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

	if (instance->eMBCurrentMode == MB_RTU || instance->eMBCurrentMode == MB_ASCII)
	{
		const auto deadline = freemodbusTimersPoll(freemodbusInstance);
		if (getEventInternal(freemodbusInstance, *event) == true)
			return true;

		assert(freemodbusInstance.timerDuration != decltype(freemodbusInstance.timerDuration)::zero());
		freemodbusSerialPoll(freemodbusInstance,
				std::min(deadline, distortos::TickClock::now() + freemodbusInstance.timerDuration));
		if (getEventInternal(freemodbusInstance, *event) == true)
			return true;

		freemodbusTimersPoll(freemodbusInstance);
	}
#if MB_TCP_ENABLED == 1
	else if (instance->eMBCurrentMode == MB_TCP)
		freemodbusTcpPoll(freemodbusInstance, distortos::TickClock::now() + std::chrono::milliseconds{100});
#endif	// MB_TCP_ENABLED == 1

	return getEventInternal(freemodbusInstance, *event);
}

extern "C" bool xMBPortEventInit(xMBInstance* const instance)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	for (auto& event : freemodbusInstance.pendingEvents)
		event = {};

	return true;
}

extern "C" bool xMBPortEventPost(xMBInstance* const instance, const eMBEventType event)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	if (freemodbusInstance.pendingEvents[event] ==
			std::numeric_limits<std::decay<decltype(freemodbusInstance.pendingEvents[event])>::type>::max())
		return false;	// overflow

	++(freemodbusInstance.pendingEvents)[event];

	return true;
}
