//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: The system for handling extra scripted entity in-game.
//
//=============================================================================//

#ifndef LFE_MAPADD_H
#define LFE_MAPADD_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/icommandline.h"
#include "filesystem.h"
#include <KeyValues.h>
#include "gamestats.h"

// Convar restoration save/restore
#define MAPADD_MAX_MODIFIED_CONVAR_STRING		128
struct mapadd_modifiedconvars_t 
{
	DECLARE_SIMPLE_DATADESC();

	char pszConvar[MAPADD_MAX_MODIFIED_CONVAR_STRING];
	char pszCurrentValue[MAPADD_MAX_MODIFIED_CONVAR_STRING];
	char pszOrgValue[MAPADD_MAX_MODIFIED_CONVAR_STRING];
};

//-----------------------------------------------------------------------------
// Purpose: Game system to kickstart the lfe's mapadd system
//-----------------------------------------------------------------------------
class CTFMapAddSystem : public CAutoGameSystemPerFrame
{
public:
	DECLARE_DATADESC();

	CTFMapAddSystem( char const *name ) : CAutoGameSystemPerFrame( name ) {}

	virtual void LevelInitPreEntity();

	void CalculateMapAddState( void );

	virtual void LevelShutdownPreEntity();

	void	ParseEntityCreate( KeyValues *pSubKey );
	void	ParseEntityKVBlock( CBaseEntity *pNode, KeyValues *pkvNode );
	void	ParseEntityDelete( KeyValues *pSubKey );
	void	ParseEntityEdit( KeyValues *pSubKey );

	bool MapAddConvarsChanging( void ) { return m_bMapAddConvarsChanging; }

	void SetMapAddConvarsChanging( bool bChanging );

	void ConvarChanged( IConVar *pConVar, const char *pOldString, float flOldValue );

	void InitMapAdd( void );

	void ShutDownMapAdd( void );

	void SetMapAddMode( bool bEnableMapAdd );

	void OnRestore( void );

	bool MapAddWasEnabledMidGame( void ) { return m_bMapAddEnabledMidGame; }

private:
	int		m_afPlayersLastButtons;
	bool	m_bMapAddConvarsChanging;
	int		m_iClearPressedButtons;
	bool	m_bMapAddEnabledMidGame;

	CUtlVector< mapadd_modifiedconvars_t > m_ModifiedConvars;
	CUtlVector<EHANDLE>				m_hSpawnedEntities;
};

extern CTFMapAddSystem g_LFEMapAdd;

inline CTFMapAddSystem* TFMapAddSystem( void )
{
	return &g_LFEMapAdd;
}

#endif // LFE_MAPADD_H
