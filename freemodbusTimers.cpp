/**
 * \file
 * \brief Definitions of timers-related functions for FreeMODBUS
 *
 * \author Copyright (C) 2019 Aleksander Szczygiel https://distortec.com https://freddiechopin.info
 * \author Copyright (C) 2021 Kamil Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freemodbusTimersPoll.hpp"

#include "FreemodbusInstance.hpp"

#include "mbport.h"

#include "distortos/assert.h"
#include "distortos/ThisThread.hpp"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

distortos::TickClock::time_point freemodbusTimersPoll(FreemodbusInstance& instance,
		const distortos::TickClock::time_point deadline)
{
	distortos::ThisThread::sleepUntil(std::min(deadline, instance.timerDeadline));

	if (distortos::TickClock::now() >= instance.timerDeadline)
	{
		instance.timerDeadline = distortos::TickClock::time_point::max();
		instance.rawInstance.pxMBPortCBTimerExpired(&instance.rawInstance);
	}

	return instance.timerDeadline;
}

extern "C" void vMBPortTimersDelay(xMBInstance*, const uint16_t timeoutMs)
{
	distortos::ThisThread::sleepFor(std::chrono::milliseconds{timeoutMs});
}

extern "C" void vMBPortTimersDisable(xMBInstance* const instance)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	freemodbusInstance.timerDeadline = decltype(freemodbusInstance.timerDeadline)::max();
}

extern "C" void vMBPortTimersEnable(xMBInstance* const instance)
{
	assert(instance != nullptr);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	freemodbusInstance.timerDeadline = distortos::TickClock::now() + freemodbusInstance.timerDuration;
}

extern "C" void xMBPortTimersClose(xMBInstance*)
{
	// Does not use any hardware resources
}

extern "C" bool xMBPortTimersInit(xMBInstance* const instance, const uint16_t timeout50us)
{
	assert(instance != nullptr);
	assert(timeout50us != 0);
	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);

	const auto duration = std::chrono::microseconds{timeout50us * 50};
	freemodbusInstance.timerDuration = std::chrono::duration_cast<decltype(freemodbusInstance.timerDuration)>(duration);
	if (freemodbusInstance.timerDuration < duration)
		++freemodbusInstance.timerDuration;

	freemodbusInstance.timerDeadline = decltype(freemodbusInstance.timerDeadline)::max();
	return true;
}
