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
#include "phrase_manager.h"

using namespace NLNET;
using namespace NLMISC;
using namespace RY_GAME_SHARE;
using namespace std;

std::vector< std::pair< std::string , IMagicActionFactory* > >* IMagicActionFactory::Factories = NULL;


class CMagicActionBasicDamage : public IMagicAction
{
public:
	CMagicActionBasicDamage()
		:_DmgChaScore1(0),_DmgChaScore3(0),_DmgChaScore2(0),_DmgType(DMGTYPE::UNDEFINED){}
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
				
			case TBrickParam::MA_DMG_TYPE:
				INFOLOG("MA_DMG_TYPE: %s",((CSBrickParamMagicDmgType *)brick.Params[i])->DmgType.c_str());
				_DmgType = DMGTYPE::stringToDamageType( ((CSBrickParamMagicDmgType *)brick.Params[i])->DmgType );
				if ( _DmgType == DMGTYPE::UNDEFINED )
				{
					nlwarning("<CMagicActionBasicDamage addBrick> invalid dmg type %s", ((CSBrickParamMagicDmgType *)brick.Params[i])->DmgType.c_str());
					return false;
				}
				break;

			case TBrickParam::MA_DMG:
				INFOLOG("MA_DMG: %u %u %u",((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore1,((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore3,((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore1);
				_DmgChaScore1 = ((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore1;
				_DmgChaScore3 = ((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore3;
				_DmgChaScore2 = ((CSBrickParamMagicDmg *)brick.Params[i])->ChaScore2;
				break;
				
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
		return PHRASE_UTILITIES::validateSpellTarget(phrase->getActor(),phrase->getTargets()[0],ACTNATURE::OFFENSIVE);
	}
	virtual void apply( CMagicPhrase * phrase, float successFactor,MBEHAV::CBehaviour & behav , bool isMad )
	{
		///\todo nico:
		//		- location
		//		- armor + shield
		//		- player damages + on armor
		//		- behaviour + chat messages
		//		- aggro
		
		CEntityBase* actor = CEntityBaseManager::getEntityBasePtr( phrase->getActor() );
		if (!actor)
			return;

		/// test resistance
		if ( successFactor <= 0.0f )
		{
			if ( actor->getId().getType() == RYZOMID::player )
				CCharacter::sendMessageToClient( actor->getId(),"MAGIC_TOTAL_MISS" );
			return;
		}
		
		const std::vector< TDataSetRow > & targets = phrase->getTargets();
		
		SSkill * skillAtt = actor->getSkills().getSkillStruct( _Skill );
		if ( ! skillAtt )
		{
			nlwarning("<CMagicActionBasicDamage apply> %s is not a valid skill",SKILLS::toString(_Skill).c_str());
			return;
		}
		
		const CSEffect * debuff = actor->lookForSEffect( EFFECT_FAMILIES::DebuffSkillMagic );
		sint skillValue = skillAtt->Current;
		if ( debuff )
			skillValue -= debuff->getParamValue();

		for ( uint i = 0; i < targets.size(); i++ )
		{
			// check target
			CEntityBase* target = CEntityBaseManager::getEntityBasePtr( targets[i] );
			if ( !target)
				continue;
			if ( isMad || PHRASE_UTILITIES::validateSpellTarget(actor->getEntityRowId(),target->getEntityRowId(),ACTNATURE::OFFENSIVE) )
			{	
				///\NOT USED NOW
				//behav.Magic.SpellPower = PHRASE_UTILITIES::getAttackIntensity( phrase->getSabrinaCost() );

				
				// test resistance
/*				SSkill * skillResist = target->getSkills().getSkillStruct( SKILLS::MagicDefense ); //TODO skill missing
				if ( ! skillResist )
				{
					nlwarning("<CMagicActionBasicDamage apply> MagicDefense is not a valid skill");
					return;
				}
*/				// get the chances ( delta level is divided by 10 because a level is 10
				const uint8 chances = PHRASE_UTILITIES::getSuccessChance( ( skillValue /*- skillResist->Current*/ )/10 ); //TODO skillResist
				const uint8 roll = (uint8)RandomGenerator.rand( 99 );
				float resistFactor = PHRASE_UTILITIES::getSucessFactor(chances, roll);

				if ( resistFactor > 0.0f  )
				{
					if ( resistFactor > 1.0f )
						resistFactor = 1.0f;
					//behav.Spell.Resist = 0;
					behav.Spell.SpellId =  MAGICFX::toMagicFx( _DmgType ,false);

					float mult = resistFactor;
					const CSEffect * effect = target->lookForSEffect( EFFECT_FAMILIES::MagicDmgAmpli );
					if ( effect )
						mult *=  ( effect->getParamValue() / 100.0f );

					sint32 realDmgChaScore1 =  sint32 ( _DmgChaScore1 * mult );
					realDmgChaScore1 = sint32( target->applyDamageOnArmor( _DmgType, realDmgChaScore1 ) );
					if ( target->changeCurrentChaScore1( - realDmgChaScore1 ) )
					{
						// send mission event
						if ( actor->getId().getType()== RYZOMID::player )
						{
							CMissionEventKill event ( target->getEntityRowId() );
							((CCharacter*) actor)->processMissionEvent( event );
						}
					}
					if ( target->getScores()._PhysicalScores[SCORES::cha_score1].Current <= 0)
					{
						target->getScores()._PhysicalScores[SCORES::cha_score1].Current = 0;
						//behav.Spell.KillingBlow = 1;
					}
					PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realDmgChaScore1 ,SCORES::cha_score1 , ACTNATURE::OFFENSIVE);

					sint32 realDmgChaScore3;
					{
						RY_GAME_SHARE::SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score3];
						realDmgChaScore3 = sint32( _DmgChaScore3 * mult );
						realDmgChaScore3 = target->applyDamageOnArmor( _DmgType, realDmgChaScore3 );
						score.Current = score.Current - realDmgChaScore3;
						if ( score.Current < 0)
							score.Current = 0;
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realDmgChaScore3 ,SCORES::cha_score3 , ACTNATURE::OFFENSIVE);
					}

					sint32 realDmgChaScore2;
					{
						RY_GAME_SHARE::SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score2];
						realDmgChaScore2 =  sint32( _DmgChaScore2 * mult );
						realDmgChaScore2 = target->applyDamageOnArmor( _DmgType, realDmgChaScore2 );
						score.Current = score.Current - realDmgChaScore2;
						if ( score.Current < 0)
							score.Current = 0;
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor, target, realDmgChaScore2 ,SCORES::cha_score2 , ACTNATURE::OFFENSIVE);
					}

					///\todo nico: real value
					behav.Spell.SpellIntensity = 5;
					
					// compute aggro
					sint32 max = target->getPhysScores()._PhysicalScores[SCORES::cha_score1].Max;
					if (max)
					{
						const sint32 aggro = (-1) * sint32((100.0 * float(realDmgChaScore1))/float(max) );
						// update the report
						CAiEventReport report;
						report.AggroMul = 1.0f;
						report.AggroAdd = aggro;
						report.addDelta(AI_EVENT_REPORT::ChaScore1, (-1)*realDmgChaScore1);
						CPhraseManager::getInstance()->addAiEventReport(report);
					}

					max = target->getPhysScores()._PhysicalScores[SCORES::cha_score3].Max;
					if (max)
					{
						const sint32 aggro = (-1) * sint32((100.0 * float(realDmgChaScore3))/float(max) );
						// update the report
						CAiEventReport report;
						report.AggroMul = 1.0f;
						report.AggroAdd = aggro;
						report.addDelta(AI_EVENT_REPORT::ChaScore3, (-1)*realDmgChaScore3);
						CPhraseManager::getInstance()->addAiEventReport(report);
					}

					max = target->getPhysScores()._PhysicalScores[SCORES::cha_score2].Max;
					if (max)
					{
						const sint32 aggro = (-1) * sint32((100.0 * float(realDmgChaScore2))/float(max) );
						// update the report
						CAiEventReport report;
						report.AggroMul = 1.0f;
						report.AggroAdd = aggro;
						report.addDelta(AI_EVENT_REPORT::ChaScore2, (-1)*realDmgChaScore2);
						CPhraseManager::getInstance()->addAiEventReport(report);
					}	
				}
				else
				{
					if ( actor->getId().getType() == RYZOMID::player )
						CCharacter::sendMessageToClient( actor->getId(),"MAGIC_TOTAL_RESIST" );
					if ( target->getId().getType() == RYZOMID::player )
						CCharacter::sendMessageToClient( target->getId(),"MAGIC_U_TOTAL_RESIST" );
					///\todo nico msgs
					//behav.Spell.Resist = 1;
				}
				/// compute resist XP gain
/*				if ( target->getId().getType() == RYZOMID::player && resistFactor < 1.0f)
				{
					///\todo nico resistFactor  is the quality factor of the action for xp
					((CCharacter*) target)->actionReport( actor, (skillResist->Current - skillValue)/10, ACTNATURE::DEFENSIVE, SKILLS::toString( SKILLS::MagicDefense ) );
				}
*/			}
		}
	}
	DMGTYPE::EDamageType	_DmgType;
	sint32					_DmgChaScore1;
	sint32					_DmgChaScore3;
	sint32					_DmgChaScore2;
};
BEGIN_MAGIC_ACTION_FACTORY(CMagicActionBasicDamage)
	ADD_MAGIC_ACTION_TYPE( "mto" )	
END_MAGIC_ACTION_FACTORY(CMagicActionBasicDamage)

