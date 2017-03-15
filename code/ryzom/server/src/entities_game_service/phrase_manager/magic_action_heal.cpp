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
#include "phrase_manager/magic_phrase.h"
#include "creature_manager/creature.h"
#include "player_manager/character.h"
#include "phrase_manager/phrase_utilities_functions.h"
#include "entity_structure/statistic.h"
#include "phrase_manager/s_effect.h"
#include "phrase_manager/phrase_manager.h"
#include "creature_manager/creature_manager.h"
#include "egs_sheets/egs_static_ai_action.h"
#include "progression/progression_pvp.h"
//
#include "game_share/magic_fx.h"
#include "ai_share/ai_event_report.h"

using namespace NLNET;
using namespace NLMISC;
using namespace std;

extern CCreatureManager CreatureManager;


class CMagicActionBasicHeal : public IMagicAction
{
public:
	CMagicActionBasicHeal()
		:_HealChaScore1(0),_HealChaScore3(0),_HealChaScore2(0){}

	/// build from AI Action
	bool initFromAiAction( const CStaticAiAction *aiAction, CMagicPhrase *phrase )
	{
#ifdef NL_DEBUG
		nlassert(phrase);
		nlassert(aiAction);
#endif
		if (aiAction->getType() != AI_ACTION::HealSpell ) return false;

		sint32 healBonus = 0;
		CCreature *creature = CreatureManager.getCreature( phrase->getActor() );
		if (creature && creature->getForm())
			healBonus = sint32(aiAction->getData().Spell.SpellPowerFactor * creature->getForm()->getAttackLevel());

		switch(aiAction->getData().Spell.AffectedScore)
		{
		case SCORES::cha_score3:
			_HealChaScore3 = sint32(aiAction->getData().Spell.SpellParamValue + healBonus);
			break;
		case SCORES::cha_score2:
			_HealChaScore2 = sint32(aiAction->getData().Spell.SpellParamValue + healBonus);
			break;
		case SCORES::cha_score1:
			_HealChaScore1 = sint32(aiAction->getData().Spell.SpellParamValue + healBonus);
			break;
		default:
			return false;
		};

		phrase->setMagicFxType( MAGICFX::healtoMagicFx( _HealChaScore1,_HealChaScore3,_HealChaScore2,false ), 3);

		return true;
	}

protected:
	struct CTargetInfos
	{
		TDataSetRow	RowId;
		bool		MainTarget;
		sint32		HealChaScore1;
		sint32		HealChaScore3;
		sint32		HealChaScore2;
	};

protected:
	/// add brick
	virtual bool addBrick( const CStaticBrick & brick, CMagicPhrase * phrase, bool &effectEnd, CBuildParameters &buildParams )
	{
		for ( uint i=0 ; i<brick.Params.size() ; ++i)
		{
			const TBrickParam::IId* param = brick.Params[i];

			switch(param->id())
			{
			case TBrickParam::MA_END:
				INFOLOG("MA_END Found: end of effect");
				effectEnd = true;
				return true;
			case TBrickParam::MA_HEAL:
				INFOLOG("MA_HEAL: %u %u %u",((CSBrickParamMagicHeal *)param)->ChaScore1,((CSBrickParamMagicHeal *)param)->ChaScore3,((CSBrickParamMagicHeal *)param)->ChaScore2);
				_HealChaScore1 = ((CSBrickParamMagicHeal *)param)->ChaScore1;
				_HealChaScore3 = ((CSBrickParamMagicHeal *)param)->ChaScore3;
				_HealChaScore2 = ((CSBrickParamMagicHeal *)param)->ChaScore2;
				phrase->setMagicFxType( MAGICFX::healtoMagicFx( _HealChaScore1,_HealChaScore3,_HealChaScore2,false ), brick.SabrinaValue);
				
			default:
				// unused param, can be useful in the phrase
				phrase->applyBrickParam( brick.Params[i], brick, buildParams );
				break;
			}
		}
		///\todo nico: check if everything is set
		return true;
	}
	virtual bool validate(CMagicPhrase * phrase, std::string &errorCode)
	{
		if ( !PHRASE_UTILITIES::validateSpellTarget(phrase->getActor(),phrase->getTargets()[0].getId(),ACTNATURE::CURATIVE_MAGIC, errorCode, true) )
		{
			return false;
		}

		return true;
	}

	virtual void launch( CMagicPhrase * phrase, sint deltaLevel, sint skillLevel, float successFactor, MBEHAV::CBehaviour & behav,
						 const std::vector<float> &powerFactors, NLMISC::CBitSet & affectedTargets, const NLMISC::CBitSet & invulnerabilityOffensive,
						 const NLMISC::CBitSet & invulnerabilityAll, bool isMad, NLMISC::CBitSet & resists, const TReportAction & actionReport )
	{
		///\todo nico:
		//		- behaviour + messages de chat
		//		- aggro

		// Critical Fail
		if(successFactor <= 0.0f)
			return;
		// Get Spell Caster
		CEntityBase* actor = CEntityBaseManager::getEntityBasePtr( phrase->getActor() );
		if (!actor)
			return;

		// Get Spell Targets
		const std::vector< CSpellTarget > & targets = phrase->getTargets();
		const uint nbTargets = (uint)targets.size();
		
		// apply power factor os used item
		successFactor *= (1 + phrase->getUsedItemStats().getPowerFactor(_Skill, phrase->getBrickMaxSabrinaCost()) );

		resists.clearAll();
		for ( uint i = 0 ; i < nbTargets ; ++i )
		{
			///\todo nico : healing a bad guy is PVP, but it should be possible to heal escort NPCS
			// check target
			CEntityBase* target = CEntityBaseManager::getEntityBasePtr( targets[i].getId() );
			if ( !target)
				continue;

			string errorCode;
			if (!isMad && !PHRASE_UTILITIES::validateSpellTarget(actor->getEntityRowId(),target->getEntityRowId(),ACTNATURE::CURATIVE_MAGIC, errorCode, i==0))
			{
				// dont warn because of multi target
				// PHRASE_UTILITIES::sendSimpleMessage(phrase->getActor(), errorCode);
				continue;
			}
			affectedTargets.set(i);

			// test if target really affected
			if (invulnerabilityAll[i])
			{
				resists.set(i);
				continue;
			}

			CTargetInfos targetInfos;
			targetInfos.RowId		= target->getEntityRowId();
			targetInfos.MainTarget	= (i == 0);
			targetInfos.HealChaScore1		= 0;
			targetInfos.HealChaScore3		= 0;
			targetInfos.HealChaScore2		= 0;

			float factor = successFactor * powerFactors[i] * phrase->getAreaSpellPowerFactor();
			if ( _HealChaScore1 != 0)
			{
				targetInfos.HealChaScore1 = sint32(_HealChaScore1 * factor);

				// update behaviour for healed ChaScore1
				behav.DeltaChaScore1 += sint16(targetInfos.HealChaScore1);
			}
			if (_HealChaScore3 != 0)
			{
				targetInfos.HealChaScore3 = sint32(_HealChaScore3 * factor);
			}
			if ( _HealChaScore2 != 0 )
			{
				targetInfos.HealChaScore2 = sint32(_HealChaScore2 * factor);
			}

			_ApplyTargets.push_back(targetInfos);
		}
	}

	virtual void apply( CMagicPhrase * phrase, sint deltaLevel, sint skillLevel, float successFactor, MBEHAV::CBehaviour & behav,
						const std::vector<float> &powerFactors, NLMISC::CBitSet & affectedTargets, const NLMISC::CBitSet & invulnerabilityOffensive,
						const NLMISC::CBitSet & invulnerabilityAll, bool isMad, NLMISC::CBitSet & resists, const TReportAction & actionReport,
						sint32 vamp, float vampRatio, bool reportXp )
	{
		CEntityBase * actor = CEntityBaseManager::getEntityBasePtr( phrase->getActor() );
		if (!actor)
			return;

		const uint nbTargets = (uint)_ApplyTargets.size();

		vector<TReportAction> actionReports;
		if (reportXp)
			actionReports.reserve(nbTargets);

		for (uint i = 0; i < nbTargets; i++)
		{
			CEntityBase * target = CEntityBaseManager::getEntityBasePtr( _ApplyTargets[i].RowId );

			if ( !target)
				continue;

			// an area heal cannot affect caster !
			if ( target->getId().getType() == RYZOMID::player && target == actor)
				continue;

			string errorCode;
			if (!PHRASE_UTILITIES::validateSpellTarget(actor->getEntityRowId(),target->getEntityRowId(),ACTNATURE::CURATIVE_MAGIC, errorCode, _ApplyTargets[i].MainTarget))
			{
				// dont warn because of multi target
				// PHRASE_UTILITIES::sendSimpleMessage(phrase->getActor(), errorCode);
				continue;
			}

			TReportAction reportAction = actionReport;
			reportAction.TargetRowId = _ApplyTargets[i].RowId;
			reportAction.DeltaLvl = deltaLevel;
			reportAction.Skill = _Skill;
			if (!reportXp)
			{
				reportAction.Skill = SKILLS::unknown;						// no xp gain but damage must be registered
				reportAction.SkillLevel = phrase->getBrickMaxSabrinaCost();	// use the real level of the enchantment
			}

			CAiEventReport aiReport;
			aiReport.Originator = phrase->getActor();
			aiReport.Target = _ApplyTargets[i].RowId;
			aiReport.Type = ACTNATURE::CURATIVE_MAGIC;

			if ( _HealChaScore1 != 0)
			{
				sint32 & realHealChaScore1 = _ApplyTargets[i].HealChaScore1;
				// clip heal ChaScore1
				if (realHealChaScore1 + target->currentChaScore1() > target->maxChaScore1())
				{
					realHealChaScore1 = target->maxChaScore1() - target->currentChaScore1();
				}

				target->changeCurrentChaScore1(realHealChaScore1);
				if ( target->currentChaScore1() >= target->maxChaScore1() )
				{
					if ( realHealChaScore1 > 0)
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore1, realHealChaScore1, SCORES::cha_score1 , ACTNATURE::CURATIVE_MAGIC);
					reportAction.ChaScore1 = realHealChaScore1;

					if (actor != target)
					{
						SM_STATIC_PARAMS_2(params2, STRING_MANAGER::entity, STRING_MANAGER::score);
						params2[0].setEIdAIAlias( target->getId(), CAIAliasTranslator::getInstance()->getAIAlias( target->getId()) );
						params2[1].Enum = SCORES::cha_score1;
						PHRASE_UTILITIES::sendDynamicSystemMessage(actor->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_ACTOR", params2);
					}

					SM_STATIC_PARAMS_1(params, STRING_MANAGER::score);
					params[0].Enum = SCORES::cha_score1;
					PHRASE_UTILITIES::sendDynamicSystemMessage(target->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_TARGET", params);
				}
				else
				{
					PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore1, realHealChaScore1, SCORES::cha_score1 , ACTNATURE::CURATIVE_MAGIC);
					reportAction.ChaScore1 = realHealChaScore1;
				}

				// update the report
				if (target->maxChaScore1())
				{
					float aggro = float(realHealChaScore1)/float(target->maxChaScore1());
					aiReport.AggroAdd = aggro;
					aiReport.addDelta(AI_EVENT_REPORT::ChaScore1, realHealChaScore1);
					CPhraseManager::getInstance().addAiEventReport(aiReport);
				}
			}

			if (_HealChaScore3 != 0)
			{
				sint32 & realHealChaScore3 = _ApplyTargets[i].HealChaScore3;
				SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score3];
				score.Current = score.Current + realHealChaScore3;
				if ( score.Current >= score.Max )
				{
					realHealChaScore3 += score.Max - score.Current;
					if ( realHealChaScore3 > 0)
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore3, realHealChaScore3, SCORES::cha_score3 , ACTNATURE::CURATIVE_MAGIC);
					score.Current = score.Max;
					reportAction.ChaScore3 = realHealChaScore3;

					if ( actor != target)
					{
						SM_STATIC_PARAMS_2(params2, STRING_MANAGER::entity, STRING_MANAGER::score);
						params2[0].setEIdAIAlias( target->getId(), CAIAliasTranslator::getInstance()->getAIAlias( target->getId()) );
						params2[1].Enum = SCORES::cha_score3;
						PHRASE_UTILITIES::sendDynamicSystemMessage(actor->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_ACTOR", params2);
					}

					SM_STATIC_PARAMS_1(params, STRING_MANAGER::score);
					params[0].Enum = SCORES::cha_score3;
					PHRASE_UTILITIES::sendDynamicSystemMessage(target->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_TARGET", params);
				}
				else
				{
					PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore3, realHealChaScore3, SCORES::cha_score3 , ACTNATURE::CURATIVE_MAGIC);
					reportAction.ChaScore3 = realHealChaScore3;
				}

				// update the report
				float aggro = float(realHealChaScore3)/float(score.Max);
				aiReport.AggroAdd = aggro;
				aiReport.addDelta(AI_EVENT_REPORT::ChaScore3, realHealChaScore3);
				CPhraseManager::getInstance().addAiEventReport(aiReport);
			}

			if ( _HealChaScore2 != 0 )
			{
				sint32 & realHealChaScore2 = _ApplyTargets[i].HealChaScore2;
				SCharacteristicsAndScores &score = target->getScores()._PhysicalScores[SCORES::cha_score2];
				score.Current = score.Current + realHealChaScore2;
				if ( score.Current >= score.Max )
				{
					realHealChaScore2 += score.Max - score.Current;
					if ( realHealChaScore2 > 0)
						PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore2, realHealChaScore2, SCORES::cha_score2 , ACTNATURE::CURATIVE_MAGIC);
					score.Current = score.Max;
					reportAction.ChaScore2 = realHealChaScore2;

					if (actor != target)
					{
						SM_STATIC_PARAMS_2(params2, STRING_MANAGER::entity, STRING_MANAGER::score);												
						params2[0].setEIdAIAlias( target->getId(), CAIAliasTranslator::getInstance()->getAIAlias( target->getId() ) );
						params2[1].Enum = SCORES::cha_score2;
						PHRASE_UTILITIES::sendDynamicSystemMessage(actor->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_ACTOR", params2);
					}

					SM_STATIC_PARAMS_1(params, STRING_MANAGER::score);
					params[0].Enum = SCORES::cha_score2;
					PHRASE_UTILITIES::sendDynamicSystemMessage(target->getEntityRowId(), "MAGIC_HEAL_FULL_SCORE_TARGET", params);
				}
				else
				{
					PHRASE_UTILITIES::sendScoreModifierSpellMessage( actor->getId(), target->getId(), realHealChaScore2, realHealChaScore2, SCORES::cha_score2 , ACTNATURE::CURATIVE_MAGIC);
					reportAction.ChaScore2 = realHealChaScore2;
				}

				// update the report
				float aggro = float(realHealChaScore2)/float(score.Max);
				aiReport.AggroAdd = aggro;
				aiReport.addDelta(AI_EVENT_REPORT::ChaScore2, realHealChaScore2);
				CPhraseManager::getInstance().addAiEventReport(aiReport);
			}

			if ( reportXp )
				actionReports.push_back(reportAction);
		}

		// report action and xp
		if (reportXp)
		{
			// send all reports
			for (uint i = 0 ; i < actionReports.size() ; ++i)
			{
				// only send report if healing succesful
				if (actionReports[i].ChaScore1 != 0 || actionReports[i].ChaScore3 != 0 || actionReports[i].ChaScore2 != 0 || actionReports[i].ChaScore4 != 0)
				{
					PROGRESSIONPVE::CCharacterProgressionPVE::getInstance()->actionReport(actionReports[i], (i==0));
					PROGRESSIONPVP::CCharacterProgressionPVP::getInstance()->reportAction(actionReports[i]);
				}
			}
		}
	}

	sint32						_HealChaScore1;
	sint32						_HealChaScore3;
	sint32						_HealChaScore2;

	/// targets that need to be treated by apply()
	std::vector<CTargetInfos>	_ApplyTargets;
};

BEGIN_MAGIC_ACTION_FACTORY(CMagicActionBasicHeal)
	ADD_MAGIC_ACTION_TYPE( "mdht" )	
END_MAGIC_ACTION_FACTORY(CMagicActionBasicHeal)

CMagicAiActionTFactory<CMagicActionBasicHeal> *CMagicActionHealAiFactoryInstance = new CMagicAiActionTFactory<CMagicActionBasicHeal>(AI_ACTION::HealSpell);
