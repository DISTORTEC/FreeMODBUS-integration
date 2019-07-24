/**
 * \file
 * \brief errorCodeToFreemodbusError() definition
 *
 * \author Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "errorCodeToFreemodbusError.hpp"

#include <cerrno>

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

eMBErrorCode errorCodeToFreemodbusError(const int error)
{
	if (error == 0)
		return MB_ENOERR;
	if (error == ENOENT)
		return MB_ENOREG;
	if (error == EINVAL)
		return MB_EINVAL;
	if (error == ENOMEM)
		return MB_ENORES;
	if (error == EIO)
		return MB_EIO;
	if (error == ENOTRECOVERABLE)
		return MB_EILLSTATE;
	if (error == ETIMEDOUT)
		return MB_ETIMEDOUT;

	return MB_EPORTERR;
}
