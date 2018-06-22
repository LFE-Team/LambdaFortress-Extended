#pragma once
#include "discord-rpc.h"

static const char* APPLICATION_ID = "457617962254467092";
//if you want to change what discord says you are playing, you will need to do these steps below:
//First, make a discord bot
//Then, enable Rich Presence on it
//Then copy the Client ID number at the top of the Discord Bot settings, and Replace the Current APPLICATION_ID above with that number

void updateDiscordPresence(const char* mapname, int curPlayers, int maxPlayers, int endTime, bool resetTime);

static void handleDiscordReady();
static void handleDiscordDisconnected(int errcode, const char* message);
static void handleDiscordError(int errcode, const char* message);
static void handleDiscordJoin(const char* secret);
static void handleDiscordSpectate(const char* secret);
static void handleDiscordJoinRequest(const DiscordJoinRequest* request);

void discordInit();

void discordShutdown();