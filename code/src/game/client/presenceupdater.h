#pragma once

#include <GameEventListener.h>

class CPresenceUpdater : public CGameEventListener {
public:
	CPresenceUpdater();

	void FireGameEvent(IGameEvent* pEvent);
	int GetConnectedPlayers();
private:
	bool ShouldGetPlayerCount = true;
};