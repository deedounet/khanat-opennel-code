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




//----------------------------------------------------------------------------

#include "stdpch.h"
#include "msg_ai_service.h"

using namespace NLMISC;
using namespace std;

CQueryEgs::TFuns CQueryEgs::_Funs;

void CQueryEgs::init()
{
#define InsertFun(X) _Funs.insert( std::make_pair(std::string(#	X), X));
	InsertFun(Name);
	InsertFun(ChaScore1); InsertFun(MaxChaScore1); InsertFun(RatioChaScore1);
	InsertFun(ChaScore3); InsertFun(MaxChaScore3); InsertFun(RatioChaScore3);
	InsertFun(ChaScore2); InsertFun(MaxChaScore2); InsertFun(RatioChaScore2);
	InsertFun(ChaScore4); InsertFun(MaxChaScore4); InsertFun(RatioChaScore4);
	InsertFun(BestSkillLevel);
	InsertFun(Target);
	InsertFun(IsInInventory); InsertFun(KnowBrick);
#undef InsertFun
}



CQueryEgs::TFunEnum CQueryEgs::getFunEnum(const std::string& funName) const
{
	// lazy intialisation so function is const but can call non const fun
	if (_Funs.empty())
	{
		const_cast<CQueryEgs*>(this)->init();
	}

	TFuns::const_iterator found = _Funs.find(funName);
	if ( found != _Funs.end())
	{
		return found->second;
	}
	return Undef;
}
