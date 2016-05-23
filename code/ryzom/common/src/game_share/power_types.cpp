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
#include "power_types.h"
#include "nel/misc/string_conversion.h"

namespace POWERS
{
	NL_BEGIN_STRING_CONVERSION_TABLE (TPowerType)
		NL_STRING_CONVERSION_TABLE_ENTRY(Taunt)
		NL_STRING_CONVERSION_TABLE_ENTRY(Shielding)
		NL_STRING_CONVERSION_TABLE_ENTRY(SpeedingUp)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore1Concentration)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore2Concentration)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore3Concentration)
		NL_STRING_CONVERSION_TABLE_ENTRY(ConvertChaScore2)
		NL_STRING_CONVERSION_TABLE_ENTRY(ConvertChaScore3)
		NL_STRING_CONVERSION_TABLE_ENTRY(Berserk)
		NL_STRING_CONVERSION_TABLE_ENTRY(BalanceChaScore1)
		NL_STRING_CONVERSION_TABLE_ENTRY(Heal)
		NL_STRING_CONVERSION_TABLE_ENTRY(Invulnerability)
		NL_STRING_CONVERSION_TABLE_ENTRY(EnchantWeapon)

		NL_STRING_CONVERSION_TABLE_ENTRY(HealChaScore1C)
		NL_STRING_CONVERSION_TABLE_ENTRY(HealChaScore3C)
		NL_STRING_CONVERSION_TABLE_ENTRY(HealChaScore2C)
		NL_STRING_CONVERSION_TABLE_ENTRY(HealChaScore4C)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChgCharac)

		NL_STRING_CONVERSION_TABLE_ENTRY(ModDefenseSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModDodgeSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModParrySkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModCraftSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModMeleeSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModRangeSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModMagicSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModDesertForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModForestForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModLacustreForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModJungleForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModPrimaryRootForageSkill)
		NL_STRING_CONVERSION_TABLE_ENTRY(ModMagicProtection)

		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore1Aura)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore2Aura)
		NL_STRING_CONVERSION_TABLE_ENTRY(ChaScore3Aura)
		NL_STRING_CONVERSION_TABLE_ENTRY(MeleeProtection)
		NL_STRING_CONVERSION_TABLE_ENTRY(Umbrella)
		NL_STRING_CONVERSION_TABLE_ENTRY(AntiMagicShield)
		NL_STRING_CONVERSION_TABLE_ENTRY(WarCry)
		NL_STRING_CONVERSION_TABLE_ENTRY(FireWall)
		NL_STRING_CONVERSION_TABLE_ENTRY(ThornWall)
		NL_STRING_CONVERSION_TABLE_ENTRY(WaterWall)
		NL_STRING_CONVERSION_TABLE_ENTRY(LightningWall)

		NL_STRING_CONVERSION_TABLE_ENTRY(UnknownType)
	NL_END_STRING_CONVERSION_TABLE(TPowerType, ConversionTable, UnknownType)

	const std::string & toString(TPowerType type)
	{
		return ConversionTable.toString(type);
	}

	TPowerType toPowerType(const std::string &str)
	{
		return ConversionTable.fromString(str);
	}
}; // POWERS
