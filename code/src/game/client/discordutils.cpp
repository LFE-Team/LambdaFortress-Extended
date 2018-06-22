#include "cbase.h"
#include "discord/discord-rpc.h"
#include "discord/discordutils.h"
#include "presenceupdater.h"
#include <time.h>

CPresenceUpdater* pUpdater;

static int64_t StartTime;

void updateDiscordPresence(const char* mapname, int curPlayers, int maxPlayers, int endTime, bool resetTime) {
	if (resetTime) {
		StartTime = time(0);
	}

	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = "In a server";
	sprintf(buffer, "Map: %s", mapname);
	discordPresence.details = buffer;
	discordPresence.startTimestamp = StartTime;
	discordPresence.endTimestamp = endTime;
	discordPresence.largeImageKey = "exampleasset"; //You can put an Asset in the Discord Bot and replace "exampleasset" with what your asset is called.
	discordPresence.partyId = "memelord4123"; //obviously change this
	discordPresence.partySize = curPlayers;
	discordPresence.partyMax = maxPlayers;
	discordPresence.instance = 0;
	Discord_UpdatePresence(&discordPresence);
}

static void handleDiscordReady() {
	ConColorMsg(COLOR_YELLOW, "Discord: ready!\n");
}

static void handleDiscordDisconnected(int errcode, const char* message) {
	ConColorMsg(COLOR_YELLOW, "Discord: disconnected (%d: %s)\n", errcode, message);
}

static void handleDiscordError(int errcode, const char* message) {
	Warning("Discord: error (%d: %s\n", errcode, message);
}

static void handleDiscordJoin(const char* secret) {
	ConColorMsg(COLOR_YELLOW, "Discord: join (%s)\n", secret);
}

static void handleDiscordSpectate(const char* secret) {
	ConColorMsg(COLOR_YELLOW, "Discord: spectate (%s)\n", secret);
}

static void handleDiscordJoinRequest(const DiscordJoinRequest* request) {
	ConColorMsg(COLOR_YELLOW, "Discord: join request from %s\n", request->username);
	ConColorMsg(COLOR_YELLOW, "Responding with no by default\n");
	Discord_Respond(request->userId, DISCORD_REPLY_NO);
}

void discordInit() {
	ConColorMsg(COLOR_YELLOW, "Discord: Initting!\n");
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
	Discord_Initialize(APPLICATION_ID, &handlers, 1, null);
	if (pUpdater == NULL) pUpdater = new CPresenceUpdater;
}

void discordShutdown() {
	ConColorMsg(COLOR_YELLOW, "Discord: Shutting down!\n");
	delete pUpdater;
	pUpdater = 0;
	Discord_Shutdown();
}