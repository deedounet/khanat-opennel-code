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



#ifndef RY_S_LINK_EFFECT_DOT_H
#define RY_S_LINK_EFFECT_DOT_H

#include "phrase_manager/s_link_effect.h"

class CSLinkEffectDot : public CSLinkEffectOffensive
{
public:
	inline CSLinkEffectDot( 
		const TDataSetRow & creatorRowId,
		const TDataSetRow & targetRowId,
		sint32 cost,
		SCORES::TScores energyCost,
		SKILLS::ESkills skill,
		uint32 maxDistance,
		DMGTYPE::EDamageType dmgType,
		uint8 power,
		TReportAction& report,
		sint32 dmgChaScore1,sint32 dmgChaScore3,sint32 dmgChaScore2,sint32 vampirise, float vampiriseRatio )
		:CSLinkEffectOffensive ( creatorRowId,targetRowId,EFFECT_FAMILIES::Dot,cost,energyCost, skill, maxDistance, 0,power, report ),
		_DmgType( dmgType ),
		_DmgChaScore1( dmgChaScore1), _DmgChaScore3( dmgChaScore3), _DmgChaScore2(dmgChaScore2),_Vampirise(vampirise),_VampiriseRatio(vampiriseRatio),
		_FirstUpdate(true)
	{
		_MagicFxType = MAGICFX::toMagicFx(dmgType, true);
	}

	DMGTYPE::EDamageType getDamageType(){ return _DmgType; }

	/**
	 * apply the effects of the... effect
	 */
	virtual bool update(CTimerEvent * event, bool applyEffect);

protected:
	DMGTYPE::EDamageType	_DmgType;
	sint32	_DmgChaScore1;
	sint32	_DmgChaScore2;
	sint32	_DmgChaScore3;
	sint32	_Vampirise;
	float	_VampiriseRatio;
	bool	_FirstUpdate;
};


#endif // RY_S_LINK_EFFECT_DOT_H

/* End of s_link_effect_dot.h */
