/**
 * \file
 * \brief Definitions of FreeMODBUS functions related to serial port
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freemodbusSerialPoll.hpp"

#include "FreemodbusInstance.hpp"

#include "mbport.h"

#include "distortos/devices/communication/SerialPort.hpp"

#include "distortos/assert.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

void freemodbusSerialPoll(FreemodbusInstance& instance, const distortos::TickClock::time_point deadline)
{
	assert(instance.serialPort != nullptr);

	while (instance.serialMode == FreemodbusInstance::SerialMode::receiver)
	{
		const auto ret = instance.serialPort->tryReadUntil(deadline, instance.frameBuffer,
				sizeof(instance.frameBuffer));
		if (ret.first != 0 || ret.second == 0)
			return;

		instance.bytesInBuffer = ret.second;
		instance.rxPosition = {};
		for (size_t i {}; i < ret.second; ++i)
			instance.rawInstance.pxMBFrameCBByteReceived(&instance.rawInstance);
	}

	if (instance.serialMode == FreemodbusInstance::SerialMode::transmiter)
	{
		while (instance.serialMode == FreemodbusInstance::SerialMode::transmiter)
			instance.rawInstance.pxMBFrameCBTransmitterEmpty(&instance.rawInstance);

		instance.serialPort->tryWriteUntil(deadline, instance.frameBuffer, instance.txPosition);
	}
}

extern "C" void vMBPortSerialEnable(xMBInstance* const instance, const bool rxEnable, const bool txEnable)
{
	assert(instance != nullptr);
	assert(rxEnable == false || txEnable == false);	// no more than one function may be enabled

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	freemodbusInstance.serialMode = rxEnable == true ? FreemodbusInstance::SerialMode::receiver :
			txEnable == true ? FreemodbusInstance::SerialMode::transmiter : FreemodbusInstance::SerialMode::disabled;

	if (rxEnable == true)
		freemodbusInstance.rxPosition = {};

	if (txEnable == true)
		freemodbusInstance.txPosition = {};
}

extern "C" void xMBPortSerialClose(xMBInstance* const instance)
{
	assert(instance != nullptr);

	xMBPortTimersClose(instance);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	freemodbusInstance.serialMode = FreemodbusInstance::SerialMode::disabled;

	assert(freemodbusInstance.serialPort != nullptr);
	const auto ret = freemodbusInstance.serialPort->close();
	assert(ret == 0);
}

extern "C" bool xMBPortSerialGetByte(xMBInstance* const instance, uint8_t* const byte)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	if (freemodbusInstance.rxPosition >= freemodbusInstance.bytesInBuffer)
		return false;

	*byte = freemodbusInstance.frameBuffer[freemodbusInstance.rxPosition++];
	return true;
}

extern "C" bool xMBPortSerialInit(xMBInstance* const instance, uint8_t, const uint32_t baudRate, const uint8_t dataBits,
		const eMBParity parity)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	const auto uartParity = parity == MB_PAR_ODD ? distortos::devices::UartParity::odd :
			parity == MB_PAR_EVEN ? distortos::devices::UartParity::even : distortos::devices::UartParity::none;
	assert(freemodbusInstance.serialPort != nullptr);
	const auto ret = freemodbusInstance.serialPort->open(baudRate, dataBits, uartParity, false);
	return ret == 0;
}

extern "C" bool xMBPortSerialPutByte(xMBInstance* const instance, const uint8_t byte)
{
	assert(instance != nullptr);

	auto& freemodbusInstance = *reinterpret_cast<FreemodbusInstance*>(instance);
	if (freemodbusInstance.txPosition >= sizeof(freemodbusInstance.frameBuffer) - 1)
		return false;

	freemodbusInstance.frameBuffer[freemodbusInstance.txPosition++] = byte;
	return true;
}
