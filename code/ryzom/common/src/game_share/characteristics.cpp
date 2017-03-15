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

#include "nel/misc/i18n.h"
#include "nel/misc/string_conversion.h"

#include "characteristics.h"

using namespace std;
using namespace NLMISC;

namespace CHARACTERISTICS
{
	// The conversion table
	const CStringConversion<TCharacteristics>::CPair stringTable [] =
	{
		{ "Cha1", cha1 },	//Cha1 Max
		{ "Cha1Reg", cha1_reg },		//Cha1 Regeneration
		{ "Cha3", cha3 },	//Cha3 Max
		{ "Cha3Reg", cha3_reg },				//Cha3 Regeneration
		{ "Cha2", cha2 },			//Cha2 Max
		{ "Cha2Reg", cha2_reg },	//Cha2 Regeneration
		{ "Cha4", cha4 },			//Cha4 Max
		{ "Cha4Reg", cha4_reg },					//Cha4 Regeneration

		{ "Unknown", Unknown },

	};
	CStringConversion<TCharacteristics> conversion(stringTable, sizeof(stringTable) / sizeof(stringTable[0]), Unknown );


	/**
	 * get the right characteristic enum from the input string
	 * \param str the input string
	 * \return the TCharacteristics associated to this string (unknown if the string cannot be interpreted)
	 */
	TCharacteristics toCharacteristic( const std::string &str )
	{
		return conversion.fromString(str);
	}

	/**
	 * get the right characteristic string from the gived enum
	 * \param c is the enum number of characteristic
	 * \return the string associated to this enum number (Unknown if enum number not exist)
	 */
	const std::string& toString( TCharacteristics c )
	{
		return conversion.toString(c);
	}
	const std::string& toString( uint c )
	{
		return conversion.toString((TCharacteristics)c);
	}

	// The code conversion table
	const CStringConversion<TCharacteristics>::CPair codeTable [] =
	{
		{ "s1", cha1 },
		{ "r1", cha1_reg },
		{ "s2", cha2 },
		{ "r2", cha2_reg },
		{ "s3", cha3 },
		{ "r3", cha3_reg },
		{ "s4", cha4 },
		{ "r4", cha4_reg },
	};
	CStringConversion<TCharacteristics> codeCharac(codeTable, sizeof(codeTable) / sizeof(codeTable[0]), Unknown );

	const std::string &getCharacteristicCode( TCharacteristics c )
	{
		return codeCharac.toString(c);
	}

	TCharacteristics getCharacteristicFromCode( const std::string &code )
	{
		return codeCharac.fromString(code);
	}
}; // CHARACTERISTICS
