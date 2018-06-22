#include "cbase.h"
#include "presenceupdater.h"
#include "discord/discordutils.h"
#include "convar.h"
#include <time.h>

#include "tf/tf_gamerules.h"

#include <globalvars_base.h>
#include <igameresources.h>

CPresenceUpdater::CPresenceUpdater()
{
	ListenForGameEvent( "player_death" );
	ListenForGameEvent( "game_newmap" );
	ListenForGameEvent( "server_cvar" );
	ListenForGameEvent( "player_connect_client" );
}

// Ran when we get an event
void CPresenceUpdater::FireGameEvent( IGameEvent* pEvent )
{
	const char* eventName = pEvent->GetName();
	ConColorMsg(COLOR_GREEN, "Event fired: %s\n", eventName);
	if (FStrEq(eventName, "game_newmap")) {
		ShouldGetPlayerCount = true; // HACK
		// Map loaded, update our Rich Presence
		updateDiscordPresence(pEvent->GetString("mapname"), 0, gpGlobals->maxClients, 
			mp_timelimit.GetInt() > 0 ? time(0) + (int)TFGameRules()->GetMapRemainingTime() : 0, true);
		// wow that line is long
	}
	else if (FStrEq(eventName, "server_cvar"))
	{
		// server cvar has changed, check if it's the timelimit that changed
		if (FStrEq(pEvent->GetString("cvarname"), "mp_timelimit"))
		{
			updateDiscordPresence(g_pGameRules->MapName(), GetConnectedPlayers(), gpGlobals->maxClients,
				mp_timelimit.GetInt() > 0 ? time(0) + (int)TFGameRules()->GetMapRemainingTime() : 0,
				mp_timelimit.GetInt() > 0 ? true : false);
			// Do not update the start time.
		}
	}
	else if (FStrEq(eventName, "player_death"))
	{
		// HACK: Can't get player count when game_newmap is fired, so we are doing this instead
		if (ShouldGetPlayerCount) {
			updateDiscordPresence(g_pGameRules->MapName(), GetConnectedPlayers(), gpGlobals->maxClients, 
				mp_timelimit.GetInt() > 0 ? time(0) + (int)TFGameRules()->GetMapRemainingTime() : 0, 
				mp_timelimit.GetInt() <= 0 ? false : true);
			ShouldGetPlayerCount = false;
		}
	}
	else if (FStrEq(eventName, "player_connect_client"))
	{
		// Fired when a player connects to a server we are connected to
		updateDiscordPresence(g_pGameRules->MapName(), GetConnectedPlayers(), gpGlobals->maxClients,
			mp_timelimit.GetInt() > 0 ? time(0) + (int)TFGameRules()->GetMapRemainingTime() : 0,
			mp_timelimit.GetInt() <= 0 ? false : true);
		ShouldGetPlayerCount = false; // we should have the current amount of players
	}
}

// Gets the amount of players connected to the server
int CPresenceUpdater::GetConnectedPlayers()
{
	int players = 0;
	for (int i = 1; i <= gpGlobals->maxClients; ++i) {
		IGameResources *gr = GameResources();

		if (gr && gr->IsConnected(i)) {
			Msg("Got a player bois\n");
			players++;
		}
	}
	Msg("Got %d players!\n", players);
	return players;
}

/*
void CC_DiscordTest(void) {
	Msg("Time left: %d\n", (int)TFGameRules()->GetMapRemainingTime());
	Msg("Time(0) left: %d\n", time(0) + (int)TFGameRules()->GetMapRemainingTime());
	return;
}
static ConCommand discord_test("discord_test", CC_DiscordTest);
*/