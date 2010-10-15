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


#ifndef PDSS_UTILS_H
#define PDSS_UTILS_H


//-------------------------------------------------------------------------------------------------
// includes
//-------------------------------------------------------------------------------------------------

#include "nel/misc/sstring.h"


//-------------------------------------------------------------------------------------------------
// Handy utilities
//-------------------------------------------------------------------------------------------------

NLMISC::CSString buildDateString(uint32 timeValue);
NLMISC::CSString buildDowTimeString(uint32 timeValue);
NLMISC::CSString buildDurationString(uint32 timeValue);

NLMISC::CSString lookupSessionName(uint32 shardId);
uint32 lookupSessionId(const NLMISC::CSString& sessionName);


#endif // PDSS_UTILS_H
