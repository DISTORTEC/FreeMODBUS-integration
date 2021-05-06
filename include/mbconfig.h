/**
 * \file
 * \brief FreeMODBUS configuration
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_MBCONFIG_H_
#define FREEMODBUS_INTEGRATION_INCLUDE_MBCONFIG_H_

#include "FreeMODBUS-configuration.h"

#ifdef __cplusplus
extern "C"
{
#endif	/* def __cplusplus */

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#ifndef MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS
/** Duration of delay before enabling transmitter in Modbus ASCII mode, milliseconds */
#define MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS		0
#endif	/* !def MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS */

#ifndef MB_FUNC_HANDLERS_MAX
/** Number of supported Modbus functions codes */
#define MB_FUNC_HANDLERS_MAX						16
#endif	/* !def MB_FUNC_HANDLERS_MAX */

#ifdef __cplusplus
}	/* extern "C" */
#endif	/* def __cplusplus */

#endif	/* FREEMODBUS_INTEGRATION_INCLUDE_MBCONFIG_H_ */
