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



#ifndef RY_MOD_MAGPRO_EFFECT_H
#define RY_MOD_MAGPRO_EFFECT_H

#include "phrase_manager/s_effect.h"
#include "game_share/protection_type.h"

#include "game_share/persistent_data.h"

/**
 * Effect class modify magic protection
 * \author Alain Saffray
 * \author Nevrax France
 * \date 2005
 */
class CModMagicProtectionEffect : public CSTimedEffect
{
public:
	NLMISC_DECLARE_CLASS(CModMagicProtectionEffect)
	
	DECLARE_PERSISTENCE_METHODS

	// default ctor
	CModMagicProtectionEffect() : CSTimedEffect() {}

	///\ctor
	CModMagicProtectionEffect( const TDataSetRow & creatorRowId, uint32 endDate, EFFECT_FAMILIES::TEffectFamily effectFamily, PROTECTION_TYPE::TProtectionType affectedProtection, float modifier1, float modifier2)
		:CSTimedEffect(creatorRowId, creatorRowId, effectFamily, true, 0, 0, endDate)
	{
#ifdef NL_DEBUG
		_LastUpdateDate = CTickEventHandler::getGameCycle();
#endif
		_Modifier1 = (sint32)modifier1;
		_Modifier2 = (sint32)modifier2;
		_AffectedProtection = affectedProtection;
	}
	
	/**
	 * apply the effects of the... effect
	 */
	virtual bool update(CTimerEvent * event, bool applyEffect);

	/// callback called when the effect is actually removed
	virtual void removed();

	// set disable time for targets
	inline void setTargetDisableTime(NLMISC::TGameCycle time) { _TargetDisableTime = time; }

	// get effect value
	inline sint32 getEffectValue() const { return _Modifier1 + _Modifier2; }

	// get affected protection
	inline PROTECTION_TYPE::TProtectionType getAffectedProtection() const { return _AffectedProtection; }

	// re-activate a magic protection modifier loaded with character
	void activate();

private:
	// disableTime for targets
	NLMISC::TGameCycle		_TargetDisableTime;
	
	/// affected protection
	PROTECTION_TYPE::TProtectionType _AffectedProtection;

	/// characteristics modifiers
	sint32				_Modifier1;
	sint32				_Modifier2;

	NLMISC::CEntityId	_CreatorEntityId;
			
#ifdef NL_DEBUG
	NLMISC::TGameCycle	_LastUpdateDate;
#endif
};


#endif // RY_MOD_MAGPRO_EFFECT_H

/* End of mod_magic_protection_effet.h */
