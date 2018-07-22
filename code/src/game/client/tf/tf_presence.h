//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// OLD Purpose: XBox Rich Presence support.
// NEW Purpose: Discord and Steam Rich Presence support.
//
//=============================================================================

#ifndef TF_PRESENCE_H
#define TF_PRESENCE_H
#ifdef _WIN32
#pragma once
#endif

#include "GameEventListener.h"
#include "hl2orange.spa.h"
#include "discord/discord-rpc.h"

#ifdef _X360
#include "basepresence.h"
#endif

#define DISCORD_FIELD_SIZE 128

class CTFRichPresence : public CGameEventListener
{
public:
	CTFRichPresence();
	~CTFRichPresence();
	void RunFrame();
	void Init();
	void LevelInit(const char *szMapname);
	void Reset();
	void Shutdown();

	static void OnReady();
	static void OnDiscordError(int errorCode, const char *szMessage);

	// IGameEventListener interface:
	virtual void FireGameEvent( IGameEvent *event);

private:
	void InitializeDiscord();
	bool NeedToUpdate();

	void UpdateRichPresence();
	void UpdatePlayerInfo();
	void UpdateNetworkInfo();
	void SetLogo();

	bool m_bErrored;
	bool m_bInitializeRequested;
	float m_flLastUpdatedTime;
	DiscordRichPresence m_sDiscordRichPresence;

	// scratch buffers to send in api struct. they need to persist
	// for a short duration after api call it seemed, it must be async
	// using a stack allocated would occassionally corrupt
	char m_szServerInfo[DISCORD_FIELD_SIZE];
	char m_szDetails[DISCORD_FIELD_SIZE];
	char m_szLatchedHostname[255];
	char m_szLatchedMapname[MAX_MAP_NAME];
	//HINSTANCE m_hDiscordDLL;
};

extern CTFRichPresence g_discordrpc;

/*

//-----------------------------------------------------------------------------
// Purpose: TF implementation for setting user contexts and properties.
//-----------------------------------------------------------------------------
class CTF_Presence : public CBasePresence, public CGameEventListener
{
public:
	// IGameEventListener Interface
	virtual void	FireGameEvent( IGameEvent * event );

	// CBaseGameSystemPerFrame overrides
	virtual bool		Init( void );
	virtual void		LevelInitPreEntity( void );

	// IPresence interface
	virtual void		SetupGameProperties( CUtlVector< XUSER_CONTEXT > &contexts, CUtlVector< XUSER_PROPERTY > &properties );
	virtual uint		GetPresenceID( const char *pIDName );
	virtual const char 	*GetPropertyIdString( const unsigned int id );
	virtual void		GetPropertyDisplayString( uint id, uint value, char *pOutput, int nBytes );
	virtual void		UploadStats();

private:
	bool	m_bIsInCommentary;

#if defined( _X360 )
	XSESSION_VIEW_PROPERTIES		m_ViewProperties[3];
#endif

};
*/
#endif // TF_PRESENCE_H