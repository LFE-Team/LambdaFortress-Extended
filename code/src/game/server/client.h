//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef CLIENT_H
#define CLIENT_H

#ifdef _WIN32
#pragma once
#endif


class CCommand;
class CUserCmd;
class CBasePlayer;


void ClientActive( edict_t *pEdict, bool bLoadGame );
void ClientPutInServer( edict_t *pEdict, const char *playername );
void ClientCommand( CBasePlayer *pSender, const CCommand &args );
void ClientPrecache( void );
// Game specific precaches
void ClientGamePrecache( void );
const char *GetGameDescription( void );
void Host_Say( edict_t *pEdict, bool teamonly );

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
struct ClassNamePrefix_t
{
	ClassNamePrefix_t( const char *pszPrefix, bool bKeepPrefix ) : m_pszPrefix( pszPrefix ), m_bKeepPrefix( bKeepPrefix )
	{
		m_nLength = strlen(pszPrefix);
	}

	const char *m_pszPrefix;
	size_t m_nLength;
	bool m_bKeepPrefix;
};

static int StringSortFunc(const void *p1, const void *p2)
{
	const char *psz1 = (const char *)p1;
	const char *psz2 = (const char *)p2;

	return V_stricmp(psz1, psz2);
}

#endif		// CLIENT_H
