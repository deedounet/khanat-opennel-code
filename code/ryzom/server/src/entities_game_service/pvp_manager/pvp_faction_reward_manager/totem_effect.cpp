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
#include "totem_effect.h"
#include "game_share/effect_families.h"

#include "nel/net/message.h"

#include "phrase_manager/phrase_utilities_functions.h"

#include "entity_manager/entity_base.h"
#include "entity_manager/entity_manager.h"

#include "player_manager/player_manager.h"
#include "player_manager/player.h"
#include "player_manager/character.h"

using namespace EFFECT_FAMILIES;
using namespace CHARACTERISTICS;

//----------------------------------------------------------------------------

bool CTotemEffect::update( CTimerEvent* event, bool applyEffect )
{
	CCharacter* character = PlayerManager.getChar( _TargetRowId );
	if ( !character )
		return true;

	switch ( _Family )
	{
	case TotemCombatRes :
		character->updateMagicProtectionAndResistance();
		break;

	case TotemCombatPar :
		character->incParryModifier( _Value );
		character->incDodgeModifier( _Value );
		break;

	case TotemCombatPoZ :
		character->incDamageShield( (uint16)_Value );
		break;
	}

	return false;
}

//----------------------------------------------------------------------------

void CTotemEffect::removed()
{
	CCharacter* character = PlayerManager.getChar( _TargetRowId );
	if ( !character )
		return;

	switch ( _Family )
	{
	case TotemCombatRes :
		character->updateMagicProtectionAndResistance();
		break;

	case TotemCombatPar :
		character->incParryModifier( -_Value );
		character->incDodgeModifier( -_Value );
		break;

	case TotemCombatPoZ :
		character->decDamageShield( (uint16)_Value );
		break;
	}
	
	// send messages to clients
	PHRASE_UTILITIES::sendEffectStandardEndMessages( _CreatorRowId, _TargetRowId, toString( _Family ) );
}


//----------------------------------------------------------------------------

CTotemCharacEffect::CTotemCharacEffect(	const TDataSetRow & creatorRowId,
										const TDataSetRow & targetRowId,
										TEffectFamily family,
										sint32 effectValue
									) :	CTotemEffect( creatorRowId, targetRowId, family, effectValue )
{
	switch ( family )
	{
		case TotemStatsChaScore1 :
			_AffectedCharac = cha1;
			break;

		case TotemRegenChaScore1 :
			_AffectedCharac = cha1_reg;
			break;

		case TotemStatsChaScore2 :
			_AffectedCharac = cha2;
			break;

		case TotemRegenChaScore2 :
			_AffectedCharac = cha2_reg;
			break;

		case TotemStatsChaScore3 :
			_AffectedCharac = cha3;
			break;

		case TotemRegenChaScore3 :
			_AffectedCharac = cha3_reg;
			break;

		case TotemStatsChaScore4 :
			_AffectedCharac = cha4;
			break;

		case TotemRegenChaScore4 :
			_AffectedCharac = cha4_reg;
			break;

		case TotemMiscMov :
			_AffectedCharac = cha1; // arbitrary value < Unknown
			break;
		
		default :
			_AffectedCharac = CHARACTERISTICS::Unknown;
			break;
	}
}


//----------------------------------------------------------------------------

bool CTotemCharacEffect::update( CTimerEvent* event, bool applyEffect )
{
	if ( _AffectedCharac >= CHARACTERISTICS::NUM_CHARACTERISTICS )
		return true;

	if ( !TheDataset.isAccessible( _TargetRowId ) )
		return true;

	CEntityBase *entity = CEntityBaseManager::getEntityBasePtr( _TargetRowId );
	if ( !entity )
		return true;

	switch ( _Family ) 
	{
	case TotemMiscMov :
		entity->getScores().SpeedVariationModifier += _Value;
		break;
		
	default :
		entity->getCharacteristics()._PhysicalCharacteristics[_AffectedCharac].Modifier += _Value;
		break;
	}

	return false;
}


//----------------------------------------------------------------------------

void CTotemCharacEffect::removed()
{
	if ( _AffectedCharac >= CHARACTERISTICS::NUM_CHARACTERISTICS )
		return;

	if ( !TheDataset.isAccessible( _TargetRowId ) )
		return;

	CEntityBase *entity = CEntityBaseManager::getEntityBasePtr( _TargetRowId );
	if ( !entity )
		return;
	
	switch ( _Family ) 
	{
	case TotemMiscMov :
		entity->getScores().SpeedVariationModifier -= _Value;
		break;
		
	default :
		entity->getCharacteristics()._PhysicalCharacteristics[_AffectedCharac].Modifier -= _Value;
		break;
	}

	// send messages to clients
	PHRASE_UTILITIES::sendEffectStandardEndMessages( _CreatorRowId, _TargetRowId,toString( _Family ) );
}
