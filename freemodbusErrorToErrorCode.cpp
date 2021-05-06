/**
 * \file
 * \brief freemodbusErrorToErrorCode() definition
 *
 * \author Copyright (C) 2019 Kamil Szczygiel https://distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "freemodbusErrorToErrorCode.hpp"

#include <cerrno>

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

int freemodbusErrorToErrorCode(const eMBErrorCode error)
{
	static const int errorCodes[]
	{
			0,					// MB_ENOERR, no error
			ENOENT,				// MB_ENOREG, illegal register address
			EINVAL,				// MB_EINVAL, illegal argument
			ENETDOWN,			// MB_EPORTERR, porting layer error
			ENOMEM,				// MB_ENORES, insufficient resources
			EIO,				// MB_EIO, I/O error
			ENOTRECOVERABLE,	// MB_EILLSTATE, protocol stack in illegal state
			ETIMEDOUT,			// MB_ETIMEDOUT, timeout error occurred
	};


	if (static_cast<size_t>(error) >= sizeof(errorCodes) / sizeof(*errorCodes))
		return EINVAL;

	return errorCodes[error];
}
