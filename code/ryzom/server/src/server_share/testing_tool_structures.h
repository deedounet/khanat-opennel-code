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



#ifndef TESTING_TOOL_STRUCTURES_H
#define TESTING_TOOL_STRUCTURES_H

// misc
#include "nel/misc/types_nl.h"
#include "nel/misc/stream.h"
#include "nel/misc/sheet_id.h"

// game share
#include "game_share/ryzom_entity_id.h"

// stl
#include <vector>

struct SActorBeginTest
{
//	uint16 Role;
//	uint16 Level;
	sint32 ChaScore1;
	sint32 ChaScore2;
	sint32 ChaScore3;
	NLMISC::CSheetId Armor;
	uint16	ArmorQuality;
	NLMISC::CSheetId RightHand;
	uint16	RightHandQuality;
	NLMISC::CSheetId LeftHand;
	uint16	LeftHandQuality;
	std::vector< NLMISC::CSheetId > Sentence1;
	std::vector< NLMISC::CSheetId > Sentence2;
	std::vector< NLMISC::CSheetId > Sentence3;

	void serial(NLMISC::IStream &f) throw(NLMISC::EStream)
	{
//		f.serial( Role );
//		f.serial( Level );
		f.serial( ChaScore1 );
		f.serial( ChaScore2 );
		f.serial( ChaScore3 );
		f.serial( Armor );
		f.serial( ArmorQuality );
		f.serial( RightHand );
		f.serial( RightHandQuality );
		f.serial( LeftHand );
		f.serial( LeftHandQuality );
		f.serialCont( Sentence1 );
		f.serialCont( Sentence2 );
		f.serialCont( Sentence3 );
	}
};

struct SLogReport
{
	std::vector< NLMISC::CSheetId > UsedBrick; //
	bool	Stop;
	uint8	SentenceSuccesLuck; //
	bool	SentenceSucces;//
	uint8	HitLuck;//
	bool	Hit;//
	bool	CriticalHit;
	uint32	MaxDamage; //
	uint32	MinDamage; //
	uint32	GivenDamage; //
	uint16	Localized; //
	uint32	ShieldAbsorption; //
 	uint32	ArmorAbsorption; //
	uint32	ChaScore1Lost; //
	bool	SpecialEffectResist;
	uint16	SpecialEffectStart;
	uint32	SpecialEffectDuration;
	uint32	UsedChaScore2; //
	uint32	UsedChaScore3;
	uint32	ChaScore2Left; //
	uint32	ChaScore3Left;

	void serial(NLMISC::IStream &f) throw(NLMISC::EStream)
	{
		f.serialCont( UsedBrick );
		f.serial( Stop );
		f.serial( SentenceSuccesLuck );
		f.serial( SentenceSucces );
		f.serial( HitLuck );
		f.serial( Hit );
		f.serial( CriticalHit );
		f.serial( MaxDamage );
		f.serial( MinDamage );
		f.serial( GivenDamage );
		f.serial( Localized );
		f.serial( ShieldAbsorption );
		f.serial( ArmorAbsorption );
		f.serial( ChaScore1Lost );
		f.serial( SpecialEffectResist );
		f.serial( SpecialEffectStart );
		f.serial( SpecialEffectDuration );
		f.serial( UsedChaScore2 );
		f.serial( UsedChaScore3 );
		f.serial( ChaScore2Left );
		f.serial( ChaScore3Left );
	}
};

extern SLogReport LogReportStructure;

#endif //TESTING_TOOL_H

