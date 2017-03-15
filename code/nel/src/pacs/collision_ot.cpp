// NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
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

#include "stdpacs.h"

#include "nel/pacs/collision_ot.h"
#include "nel/pacs/move_primitive.h"


namespace NLPACS
{

// ***************************************************************************

// Remove the collision from the primitives
void CCollisionOTDynamicInfo::removeFromPrimitives ()
{
	_FirstPrimitive->removeCollisionOTInfo (this);
	_SecondPrimitive->removeCollisionOTInfo (this);
}

// ***************************************************************************

// Remove the collision from the primitives
void CCollisionOTStaticInfo::removeFromPrimitives ()
{
	_Primitive->removeCollisionOTInfo (this);
}

// ***************************************************************************

} // NLPACS
