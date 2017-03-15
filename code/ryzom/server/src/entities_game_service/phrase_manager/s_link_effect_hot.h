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



#ifndef RY_S_LINK_EFFECT_HOT_H
#define RY_S_LINK_EFFECT_HOT_H

#include "phrase_manager/s_link_effect.h"
#include "entity_manager/entity_base.h"

class CSLinkEffectHot : public CSLinkEffect
{
public:
	inline CSLinkEffectHot( const TDataSetRow & creatorRowId,
		const TDataSetRow & targetRowId,
		sint32 cost,
		SCORES::TScores energyCost,
		SKILLS::ESkills skill,
		uint32 maxDistance,
		uint8 power,
		TReportAction& report,
		sint32 healChaScore1, sint32 healChaScore3, sint32 healChaScore2)
		:CSLinkEffect ( creatorRowId,targetRowId,EFFECT_FAMILIES::Hot, cost,energyCost,skill,maxDistance, 0,power,report),
		_HealChaScore1(healChaScore1),_HealChaScore3(healChaScore3),_HealChaScore2(healChaScore2),_FirstUpdate(true)
	{
		_MagicFxType = MAGICFX::healtoMagicFx( _HealChaScore1,_HealChaScore3,_HealChaScore2,true);
		_Report.ActionNature = ACTNATURE::CURATIVE_MAGIC;
	}

	/**
	 * apply the effects of the... effect
	 */
	virtual bool update(CTimerEvent * event, bool applyEffect);

protected:
	void applyOnScore( CEntityBase * caster, CEntityBase * target,SCORES::TScores scoreType, sint32& value );
	sint32 _HealChaScore1;
	sint32 _HealChaScore3;
	sint32 _HealChaScore2;
	bool   _FirstUpdate;
};


#endif // RY_S_LINK_EFFECT_HOT_H

/* End of s_link_effect_hot.h */
