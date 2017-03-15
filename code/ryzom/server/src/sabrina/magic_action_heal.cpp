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
#include "magic_action.h"
#include "magic_phrase.h"
#include "creature.h"
#include "character.h"
#include "phrase_utilities_functions.h"
#include "game_share/entity_structure/statistic.h"
#include "game_share/magic_fx.h"
#include "s_effect.h"

using namespace NLNET;
using namespace NLMISC;
using namespace RY_GAME_SHARE;
using namespace std;



class CMagicActionBasicHeal : public IMagicAction
{
public:
	CMagicActionBasicHeal()
		:_HealChaScore1(0),_HealChaScore3(0),_HealChaScore2(0){}
protected:
	virtual bool addBrick( const CStaticBrick & brick, CMagicPhrase * phrase, bool &effectEnd )
	{
		for ( uint i=0 ; i<brick.Params.size() ; ++i)
		{
			switch(brick.Params[i]->id())
			{
			case TBrickParam::MA_END:
				INFOLOG("MA_END Found: end of effect");
				effectEnd = true;
				return true;
			case TBrickParam::MA_HEAL:
				INFOLOG("MA_HEAL: %u %u %u",((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore1,((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore3,((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore2);
				_HealChaScore1 = ((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore1;
				_HealChaScore3 = ((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore3;
				_HealChaScore2 = ((CSBrickParamMagicHeal *)brick.Params[i])->ChaScore2;
				
			default:
				// unused param, can be useful in the phrase
				phrase->applyBrickParam( brick.Params[i] );
				break;
			}
		}
		///\todo nico: check if everything is set
		return true;
	}
	virtual bool validate(CMagicPhrase * phrase)
	{
		return PHRASE_UTILITIES::validateSpellTarget(phrase->getActor(),phrase->getTargets()[0],ACTNATURE::DEFENSIVE);
	}
	virtual void apply( CMagicPhrase * phrase, float successFactor,MBEHAV::CBehaviour & behav , bool isMad )
	{
		///\todo nico:
		//		- behaviour + messages de chat
		//		- aggro

		CEntityBase* actor = CEntityBaseManager::getEntityBasePtr( phrase->getActor() );
		if (!actor)
			return;
		if ( successFactor <= 0.0f )
		{
			if ( actor->getId().getType() == RYZOMID::player )
				CCharacter::sendMessageToClient( actor->getId(),"MAGIC_TOTAL_MISS" );
		}
		const std::vector< TDataSetRow > & targets = phrase->getTargets();

		/// compute XP gain
		SSkill * skillCast = actor->getSkills().getSkillStruct( _Skill );
		if ( ! skillCast )
		{
			nlwarning("<CMagicActionHeal apply> %s is not a valid skill",SKILLS::toString(_Skill).c_str());
			return;
		}
		const CSEffect * debuff = actor->lookForSEffect( EFFECT_FAMILIES::DebuffSkillMagic );
		sint skillValue = skillCast->Current;
		if ( debuff )
			skillValue -= debuff->getParamValue();

		if ( actor->getId().getType() == RYZOMID::player )
		{
			///\todo nico successFactor is the quality factor of the action for xp
			((CCharacter*) actor)->actionReport( NULL, ( skillValue - phrase->getSabrinaCost() )/10, ACTNATURE::DEFENSIVE, SKILLS::toString( _Skill ) );
		}	
		for ( uint i = 0; i < targets.size(); i++ )
		{
			///\todo nico : healing a bad guy is PVP, but it should be possible to heal escort NPCS
			// check target
			CEntityBase* target = CEntityBaseManager::getEntityBasePtr( targets[i] );
			if ( !target)
				continue;

			if ( isMad || PHRASE_UTILITIES::validateSpellTarget(actor->getEntityRowId(),target->getEntityRowId(),ACTNATURE::DEFENSIVE) )
			{		
				//behav.Magic.SpellPower = PHRASE_UTILITIES::getAttackIntensity( phrase->getSabrinaCost() );
				//behav.Spell.Resist = 0;
				//behav.Spell.KillingBlow = 0;
			
				///
				sint32 realHealChaScore1 = 0;
				{
					RY_GAME_SHARE::SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score1];
					score.Current = score.Current + sint32(_HealChaScore1 * successFactor);
					if ( score.Current >= score.Max )
					{
						realHealChaScore1 = sint32(_HealChaScore1 * successFactor) + score.Max - score.Current;
						if ( realHealChaScore1 > 0)
							PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target,  realHealChaScore1,SCORES::cha_score1 , ACTNATURE::DEFENSIVE);
						score.Current = score.Max;
					}
					else
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realHealChaScore1 ,SCORES::cha_score1 , ACTNATURE::DEFENSIVE);
				}
				sint32 realHealChaScore3 = 0;
				{
					RY_GAME_SHARE::SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score3];
					score.Current = score.Current + sint32(_HealChaScore3 * successFactor);
					if ( score.Current >= score.Max )
					{
						realHealChaScore3 = sint32(_HealChaScore3 * successFactor) + score.Max - score.Current;
						if ( realHealChaScore3 > 0)
							PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target,  realHealChaScore3,SCORES::cha_score3 , ACTNATURE::DEFENSIVE);
						score.Current = score.Max;
					}
					else
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realHealChaScore3 ,SCORES::cha_score3 , ACTNATURE::DEFENSIVE);
				}

				sint32 realHealChaScore2 = 0;
				{
					RY_GAME_SHARE::SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score2];
					score.Current = score.Current + sint32(_HealChaScore2 * successFactor);
					if ( score.Current >= score.Max )
					{
						realHealChaScore2 = sint32(_HealChaScore2 * successFactor) + score.Max - score.Current;
						if ( realHealChaScore2 > 0)
							PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target,  realHealChaScore2,SCORES::cha_score2 , ACTNATURE::DEFENSIVE);
						score.Current = score.Max;
					}
					else
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realHealChaScore2 ,SCORES::cha_score2 , ACTNATURE::DEFENSIVE);
				}
				/*behav.Magic.ImpactIntensity = PHRASE_UTILITIES::getImpactIntensity( realHealChaScore1 ,targets[i],SCORES::cha_score1);
				behav.Magic.ImpactIntensity += PHRASE_UTILITIES::getImpactIntensity( realHealChaScore3 ,targets[i],SCORES::cha_score3);
				behav.Magic.ImpactIntensity = PHRASE_UTILITIES::getImpactIntensity( realHealChaScore2 ,targets[i],SCORES::cha_score2);
				*/
				behav.Spell.SpellIntensity = 5;
				behav.Spell.SpellId =  MAGICFX::healtoMagicFx( _HealChaScore1,_HealChaScore3,_HealChaScore2,false );
			}
		}
	}
	sint32					_HealChaScore1;
	sint32					_HealChaScore3;
	sint32					_HealChaScore2;
};

BEGIN_MAGIC_ACTION_FACTORY(CMagicActionBasicHeal)
	ADD_MAGIC_ACTION_TYPE( "mtch" )	
END_MAGIC_ACTION_FACTORY(CMagicActionBasicHeal)
