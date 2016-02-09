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



#include "stdpch.h"

#include "nel/misc/debug.h"
#include "continent.h"
// nel
#include "nel/misc/string_conversion.h"

using namespace std;
using namespace NLMISC;

namespace CONTINENT
{
	NL_BEGIN_STRING_CONVERSION_TABLE (TContinent)
		NL_STRING_CONVERSION_TABLE_ENTRY(INDOORS)
		NL_STRING_CONVERSION_TABLE_ENTRY(NEWBIELAND)
		NL_STRING_CONVERSION_TABLE_ENTRY(UNKNOWN)
	NL_END_STRING_CONVERSION_TABLE(TContinent, ContinentConversion, UNKNOWN)



	//-----------------------------------------------
	// toContinent:
	//-----------------------------------------------
	TContinent toContinent(const std::string &str)
	{
		return ContinentConversion.fromString(str);
	}


	//-----------------------------------------------
	// toString :
	//-----------------------------------------------
	const std::string& toString(TContinent continent)
	{
		return ContinentConversion.toString(continent);
	}
}; // CONTINENT
