/**
 * \file
 * \brief freemodbusErrorToErrorCode() declaration
 *
 * \author Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSERRORTOERRORCODE_HPP_
#define FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSERRORTOERRORCODE_HPP_

#include "mberrorcode.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Converts FreeMODBUS' error to error code.
 *
 * \param [in] error is a FreeMODBUS' error
 *
 * \return \a error converted to error code
 */

int freemodbusErrorToErrorCode(eMBErrorCode error);

#endif	// FREEMODBUS_INTEGRATION_INCLUDE_FREEMODBUSERRORTOERRORCODE_HPP_
