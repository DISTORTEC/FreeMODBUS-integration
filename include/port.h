/**
 * \file
 * \brief port.h header for FreeMODBUS
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_PORT_H_
#define FREEMODBUS_INTEGRATION_INCLUDE_PORT_H_

#ifdef __cplusplus
extern "C"
{
#endif	/* def __cplusplus */

struct xMBInstance;

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

/** start of critical section */
#define ENTER_CRITICAL_SECTION(instance)

/** end of critical section */
#define EXIT_CRITICAL_SECTION(instance)

/** "inline" keyword */
#define	INLINE									inline

/** this implementation provides vMBPortClose() */
#define MB_PORT_HAS_CLOSE						1

#ifdef __cplusplus
}	/* extern "C" */
#endif	/* def __cplusplus */

#endif	/* FREEMODBUS_INTEGRATION_INCLUDE_PORT_H_ */
