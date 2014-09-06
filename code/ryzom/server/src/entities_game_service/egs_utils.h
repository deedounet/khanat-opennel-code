// Ryzom - MMORPG Framework <http://dev.ryzom.com/projects/ryzom/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RY_EGS_UTILS_H
#define RY_EGS_UTILS_H

#include "nel/misc/debug.h"

/// if defined, the EGS persistant data generated by the PDS will be in debug mode
/// It should only concern dynamic_casts from PD genrated type to EGS types
#define RY_EGS_PD_DEBUG

/// assert and cast that must be used to test the PD generated class
#ifdef RY_EGS_PD_DEBUG
	#define EGS_PD_AST nlassert
	#define EGS_PD_CAST dynamic_cast
#else
	#define EGS_PD_AST ((void)0)
	#define EGS_PD_CAST static_cast
#endif

/// if USE_PDS is defined, we use the Persistant Data service, otherwise, EGS takes full care of persistance
//#define USE_PDS

#endif // egs_utils

/* End of egs_utils.h */
