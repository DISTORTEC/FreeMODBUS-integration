/**
 * \file
 * \brief errorCodeToFreemodbusError() declaration
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com https://freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_ERRORCODETOFREEMODBUSERROR_HPP_
#define FREEMODBUS_INTEGRATION_INCLUDE_ERRORCODETOFREEMODBUSERROR_HPP_

#include "mberrorcode.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Converts error code to FreeMODBUS' error.
 *
 * \param [in] error is an error code
 *
 * \return \a error converted to FreeMODBUS' error
 */

eMBErrorCode errorCodeToFreemodbusError(int error);

#endif	// FREEMODBUS_INTEGRATION_INCLUDE_ERRORCODETOFREEMODBUSERROR_HPP_
