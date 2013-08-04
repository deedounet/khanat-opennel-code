// NeLNS - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
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

#ifndef NL_LOGIN_SERVICE_H
#define NL_LOGIN_SERVICE_H

// we have to include windows.h because mysql.h uses it but not include it
#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <winsock2.h>
#	include <windows.h>
typedef unsigned long ulong;
#endif

#include <mysql.h>

#include "nel/misc/types_nl.h"

#include "nel/misc/debug.h"
#include "nel/misc/config_file.h"
#include "nel/misc/displayer.h"
#include "nel/misc/log.h"

#include "nel/net/service.h"

using namespace std;
using namespace NLMISC;
using namespace NLNET;

// Structures

struct CFrontEnd
{
	CFrontEnd(TServiceId sid, bool patching, const std::string& patchingURI) : SId(sid), Patching(patching), PatchURI(patchingURI)	{}

	TServiceId	SId;		// Service Id of the frontend on the remote shard
	bool		Patching;	// Is FS in patching mode
	std::string	PatchURI;	// Patch URI
};

struct CShard
{
	CShard (uint32 shardId, TServiceId sid) : ShardId(shardId), NbPlayers(0), SId(sid) {}
	
	sint32	ShardId;	// unique shard id generated by the database, used to identify a WS
	uint32	NbPlayers;	// number of player for *this* LS, the total number of player for a shard is in the database
	TServiceId	SId;		// unique service id used to identify the connection when a callback happen

	std::vector<CFrontEnd>	FrontEnds;	// frontends on the shard
};


// Variables

extern CLog*				Output;

extern std::vector<CShard>	Shards;

// Functions

sint findShardWithSId (NLNET::TServiceId sid);
sint findShard (sint32 shardId);

void displayShards ();
void displayUsers ();
sint findUser (uint32 Id);
void beep (uint freq = 400, uint nb = 2, uint beepDuration = 100, uint pauseDuration = 100);

/*
 * disconnectClient is true if we need to disconnect the client on connected on the login system (during the login process)
 * disconnectShard is true if we need to send a message to the shard to disconnect the client from the shard
 */
//void disconnectClient (CUser &user, bool disconnectClient, bool disconnectShard);



#endif // NL_LOGIN_SERVICE_H

/* End of login_service.h */
