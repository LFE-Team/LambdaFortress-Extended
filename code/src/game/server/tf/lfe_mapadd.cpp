//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: The system for handling extra scripted entity in-game.
// aka: the edited commentary system
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#include "tier0/icommandline.h"
#include "igamesystem.h"
#include "filesystem.h"
#include <KeyValues.h>
#include "in_buttons.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "utldict.h"
#include "isaverestore.h"
#include "eventqueue.h"
#include "saverestore_utlvector.h"
#include "gamestats.h"
#include "ai_basenpc.h"
#include "Sprite.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAPADD_SPAWNED_SEMAPHORE		"mapp_semaphore"

// Convar restoration save/restore
#define MAPADD_MAX_MODIFIED_CONVAR_STRING		128
struct mapadd_modifiedconvars_t 
{
	DECLARE_SIMPLE_DATADESC();

	char pszConvar[MAPADD_MAX_MODIFIED_CONVAR_STRING];
	char pszCurrentValue[MAPADD_MAX_MODIFIED_CONVAR_STRING];
	char pszOrgValue[MAPADD_MAX_MODIFIED_CONVAR_STRING];
};

bool g_bEnableMapAdd = false;
bool IsMapAddAllowed( void )
{
	return g_bEnableMapAdd;
}


//===========================================================================================================
// LFE MAPPADD GAME SYSTEM
//===========================================================================================================
void CV_GlobalChange_LFEMapAdd( IConVar *var, const char *pOldString, float flOldValue );

//-----------------------------------------------------------------------------
// Purpose: Game system to kickstart the lfe's mapadd system
//-----------------------------------------------------------------------------
class CTFMapAddSystem : public CAutoGameSystemPerFrame
{
public:
	DECLARE_DATADESC();

	CTFMapAddSystem() : CAutoGameSystemPerFrame( "CTFMapAddSystem" )
	{
	}

	virtual void LevelInitPreEntity()
	{
		m_bMapAddConvarsChanging = false;
		m_iClearPressedButtons = 0;

		g_bEnableMapAdd = IsMapAddAllowed();

		CalculateMapAddState();
	}

	void CalculateMapAddState( void )
	{
		// Set the available cvar if we can find mapadd data for this level
		char szFullName[512];
		Q_snprintf(szFullName,sizeof(szFullName), "maps/mapadd/%s.txt", STRING( gpGlobals->mapname) );
		if ( filesystem->FileExists( szFullName ) )
		{
			// If the user wanted commentary, kick it on
			if ( IsMapAddAllowed() )
			{
				g_bEnableMapAdd = true;
			}
		}
		else
		{
			g_bEnableMapAdd = false;
		}
	}

	virtual void LevelShutdownPreEntity()
	{
		ShutDownMapAdd();
	}

	void ParseEntKVBlock( CBaseEntity *pNode, KeyValues *pkvNode )
	{
		KeyValues *pkvNodeData = pkvNode->GetFirstSubKey();
		while ( pkvNodeData )
		{
			// Handle the connections block
			if ( !Q_strcmp(pkvNodeData->GetName(), "connections") )
			{
				ParseEntKVBlock( pNode, pkvNodeData );
			}
			else
			{ 
				#define MAPPADD_STRING_LENGTH_MAX		1024

				const char *pszValue = pkvNodeData->GetString();
				Assert( Q_strlen(pszValue) < MAPPADD_STRING_LENGTH_MAX );
				if ( Q_strnchr(pszValue, '^', MAPPADD_STRING_LENGTH_MAX) )
				{
					// We want to support quotes in our strings so that we can specify multiple parameters in
					// an output inside our commentary files. We convert ^s to "s here.
					char szTmp[MAPPADD_STRING_LENGTH_MAX];
					Q_strncpy( szTmp, pszValue, MAPPADD_STRING_LENGTH_MAX );
					int len = Q_strlen( szTmp );
					for ( int i = 0; i < len; i++ )
					{
						if ( szTmp[i] == '^' )
						{
							szTmp[i] = '"';
						}
					}

					pNode->KeyValue( pkvNodeData->GetName(), szTmp );
				}
				else
				{
					pNode->KeyValue( pkvNodeData->GetName(), pszValue );
				}
			}

			pkvNodeData = pkvNodeData->GetNextKey();
		}
	}

	virtual void LevelInitPostEntity( void )
	{
		if ( !IsMapAddAllowed() )
			return;

		// Don't spawn commentary entities when loading a savegame
		if ( gpGlobals->eLoadType == MapLoad_LoadGame || gpGlobals->eLoadType == MapLoad_Background )
			return;

		m_bMapAddEnabledMidGame = false;
		InitMapAdd();
	}

	void PrePlayerRunCommand( CBasePlayer *pPlayer, CUserCmd *pUserCmds )
	{
		if ( !IsMapAddAllowed() )
			return;

		if ( pPlayer->IsFakeClient() )
			return;
	}

	bool MapAddConvarsChanging( void )
	{
		return m_bMapAddConvarsChanging;
	}

	void SetMapAddConvarsChanging( bool bChanging )
	{
		m_bMapAddConvarsChanging = bChanging;
	}

	void ConvarChanged( IConVar *pConVar, const char *pOldString, float flOldValue )
	{
		ConVarRef var( pConVar );

		// A convar has been changed by a commentary node. We need to store
		// the old state. If the engine shuts down, we need to restore any
		// convars that the commentary changed to their previous values.
		for ( int i = 0; i < m_ModifiedConvars.Count(); i++ )
		{
			// If we find it, just update the current value
			if ( !Q_strncmp( var.GetName(), m_ModifiedConvars[i].pszConvar, MAPADD_MAX_MODIFIED_CONVAR_STRING ) )
			{
				Q_strncpy( m_ModifiedConvars[i].pszCurrentValue, var.GetString(), MAPADD_MAX_MODIFIED_CONVAR_STRING );
				//Msg("    Updating Convar %s: value %s (org %s)\n", m_ModifiedConvars[i].pszConvar, m_ModifiedConvars[i].pszCurrentValue, m_ModifiedConvars[i].pszOrgValue );
				return;
			}
		}

		// We didn't find it in our list, so add it
		mapadd_modifiedconvars_t newConvar;
		Q_strncpy( newConvar.pszConvar, var.GetName(), MAPADD_MAX_MODIFIED_CONVAR_STRING );
		Q_strncpy( newConvar.pszCurrentValue, var.GetString(), MAPADD_MAX_MODIFIED_CONVAR_STRING );
		Q_strncpy( newConvar.pszOrgValue, pOldString, MAPADD_MAX_MODIFIED_CONVAR_STRING );
		m_ModifiedConvars.AddToTail( newConvar );

		/*
		Msg(" Commentary changed '%s' to '%s' (was '%s')\n", var->GetName(), var->GetString(), pOldString );
		Msg(" Convars stored: %d\n", m_ModifiedConvars.Count() );
		for ( int i = 0; i < m_ModifiedConvars.Count(); i++ )
		{
			Msg("    Convar %d: %s, value %s (org %s)\n", i, m_ModifiedConvars[i].pszConvar, m_ModifiedConvars[i].pszCurrentValue, m_ModifiedConvars[i].pszOrgValue );
		}
		*/
	}

	void InitMapAdd( void )
	{
		// Install the global cvar callback
		cvar->InstallGlobalChangeCallback( CV_GlobalChange_LFEMapAdd );

		// If we find the mapadd semaphore, the mapadd entities already exist.
		// This occurs when you transition back to a map that has saved mapadd nodes in it.
		if ( gEntList.FindEntityByName( NULL, MAPADD_SPAWNED_SEMAPHORE ) )
			return;

		// Spawn the mapadd semaphore entity
		CBaseEntity *pSemaphore = CreateEntityByName( "info_target" );
		pSemaphore->SetName( MAKE_STRING(MAPADD_SPAWNED_SEMAPHORE) );

		bool oldLock = engine->LockNetworkStringTables( false );

		// Find the mapadd file
		char szFullName[512];
		Q_snprintf(szFullName,sizeof(szFullName), "maps/mapadd/%s.txt", STRING( gpGlobals->mapname ));
		KeyValues *pkvFile = new KeyValues( "MapAdd" );
		if ( pkvFile->LoadFromFile( filesystem, szFullName, "MOD" ) )
		{
			Msg( "MapAdd: Loading mapadd data from %s. \n", szFullName );

			// Load each mapadd block, and spawn the entities
			KeyValues *pkvEnt = pkvFile->GetFirstSubKey();
			while ( pkvEnt )
			{
				// Get node name
				const char *pMapAddEntName = pkvEnt->GetName();

				// Skip the trackinfo
				if ( !Q_strncmp( pMapAddEntName, "trackinfo", 9 ) )
				{
					pkvEnt = pkvEnt->GetNextKey();
					continue;
				}

				KeyValues *pClassname = pkvEnt->FindKey( "classname" );
				if ( pClassname )
				{
					// Use the classname instead
					pMapAddEntName = pClassname->GetString();
				}

				// Spawn the mapadd entity
				CBaseEntity *pMapAddEnt = CreateEntityByName( pMapAddEntName );
				if ( pMapAddEnt )
				{
					ParseEntKVBlock( pMapAddEnt, pkvEnt );
					DispatchSpawn( pMapAddEnt );

					EHANDLE hHandle;
					hHandle = pMapAddEnt;
					m_hSpawnedEntities.AddToTail( hHandle );
				}
				else
				{
					Warning("MapAdd: Failed to spawn entity, type: '%s'\n", pMapAddEntName );
				}

				// Move to next entity
				pkvEnt = pkvEnt->GetNextKey();
			}

			// Then activate all the entities
			for ( int i = 0; i < m_hSpawnedEntities.Count(); i++ )
			{
				m_hSpawnedEntities[i]->Activate();
			}
		}
		else
		{
			Msg( "MapAdd: Could not find mapadd data file '%s'. \n", szFullName );
		}

		engine->LockNetworkStringTables( oldLock );
	}

	void ShutDownMapAdd( void )
	{
		// Destroy all the entities created by mapadd
		for ( int i = m_hSpawnedEntities.Count()-1; i >= 0; i-- )
		{
			if ( m_hSpawnedEntities[i] )
			{
				UTIL_Remove( m_hSpawnedEntities[i] );
			}
		}
		m_hSpawnedEntities.Purge();

		// Remove the mapadd semaphore
		CBaseEntity *pSemaphore = gEntList.FindEntityByName( NULL, MAPADD_SPAWNED_SEMAPHORE );
		if ( pSemaphore )
		{
			UTIL_Remove( pSemaphore );
		}

		// Remove our global convar callback
		cvar->RemoveGlobalChangeCallback( CV_GlobalChange_LFEMapAdd );

		// Reset any convars that have been changed by the mapadd
		for ( int i = 0; i < m_ModifiedConvars.Count(); i++ )
		{
			ConVar *pConVar = (ConVar *)cvar->FindVar( m_ModifiedConvars[i].pszConvar );
			if ( pConVar )
			{
				pConVar->SetValue( m_ModifiedConvars[i].pszOrgValue );
			}
		}
		m_ModifiedConvars.Purge();
	}

	void SetMapAddMode( bool bEnableMapAdd )
	{
		g_bEnableMapAdd = bEnableMapAdd;
		CalculateMapAddState();

		// If we're turning on mapadd, create all the entities.
		if ( IsMapAddAllowed() )
		{
			m_bMapAddEnabledMidGame = true;
			InitMapAdd();
		}
		else
		{
			ShutDownMapAdd();
		}
	}

	void OnRestore( void )
	{
		cvar->RemoveGlobalChangeCallback( CV_GlobalChange_LFEMapAdd );

		if ( !IsMapAddAllowed() )
			return;

		// Set any convars that have already been changed by the mapadd before the save
		for ( int i = 0; i < m_ModifiedConvars.Count(); i++ )
		{
			ConVar *pConVar = (ConVar *)cvar->FindVar( m_ModifiedConvars[i].pszConvar );
			if ( pConVar )
			{
				//Msg("    Restoring Convar %s: value %s (org %s)\n", m_ModifiedConvars[i].pszConvar, m_ModifiedConvars[i].pszCurrentValue, m_ModifiedConvars[i].pszOrgValue );
				pConVar->SetValue( m_ModifiedConvars[i].pszCurrentValue );
			}
		}

		// Install the global cvar callback
		cvar->InstallGlobalChangeCallback( CV_GlobalChange_LFEMapAdd );
	}

	bool MapAddWasEnabledMidGame( void ) 
	{
		return m_bMapAddEnabledMidGame;
	}

private:
	int		m_afPlayersLastButtons;
	bool	m_bMapAddConvarsChanging;
	int		m_iClearPressedButtons;
	bool	m_bMapAddEnabledMidGame;

	CUtlVector< mapadd_modifiedconvars_t > m_ModifiedConvars;
	CUtlVector<EHANDLE>				m_hSpawnedEntities;
};

CTFMapAddSystem	g_LFEMapAdd;

void MapAdd_PePlayerRunCommand( CBasePlayer *player, CUserCmd *ucmd )
{
	g_LFEMapAdd.PrePlayerRunCommand( player, ucmd );
}

BEGIN_DATADESC_NO_BASE( CTFMapAddSystem )
	//int m_afPlayersLastButtons;			DON'T SAVE
	//bool m_bMapAddConvarsChanging;	DON'T SAVE
	//int m_iClearPressedButtons;			DON'T SAVE

	DEFINE_FIELD( m_bMapAddEnabledMidGame, FIELD_BOOLEAN ),

	DEFINE_UTLVECTOR( m_ModifiedConvars, FIELD_EMBEDDED ),
	DEFINE_UTLVECTOR( m_hSpawnedEntities, FIELD_EHANDLE ),
END_DATADESC()

BEGIN_SIMPLE_DATADESC( mapadd_modifiedconvars_t )
	DEFINE_ARRAY( pszConvar, FIELD_CHARACTER, MAPADD_MAX_MODIFIED_CONVAR_STRING ),
	DEFINE_ARRAY( pszCurrentValue, FIELD_CHARACTER, MAPADD_MAX_MODIFIED_CONVAR_STRING ),
	DEFINE_ARRAY( pszOrgValue, FIELD_CHARACTER, MAPADD_MAX_MODIFIED_CONVAR_STRING ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CC_MapAddChanged( IConVar *pConVar, const char *pOldString, float flOldValue )
{
	ConVarRef var( pConVar );
 	if ( var.GetBool() != g_bEnableMapAdd )
	{
		g_LFEMapAdd.SetMapAddMode( var.GetBool() );
	}
}

ConVar lfe_mapadd_system_enable("lfe_mapadd_system_enable", "1", FCVAR_NONE, "Enables mapadd file to generate scripted stuff if it is available for the current map.", CC_MapAddChanged );

//-----------------------------------------------------------------------------
// Purpose: We need to revert back any convar changes that are made by the
//			mapadd system during mapadd. This code stores convar changes
//			made by the mapadd system, and reverts them when finished.
//-----------------------------------------------------------------------------
void CV_GlobalChange_LFEMapAdd( IConVar *var, const char *pOldString, float flOldValue )
{
	if ( !g_LFEMapAdd.MapAddConvarsChanging() )
	{
		// A convar has changed, but not due to mapadd nodes. Ignore it.
		return;
	}

	g_LFEMapAdd.ConvarChanged( var, pOldString, flOldValue );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CC_MapAddNotChanging( void )
{
	g_LFEMapAdd.SetMapAddConvarsChanging( false );
}
static ConCommand lfe_mapadd_cvarsnotchanging("lfe_mapadd_cvarsnotchanging", CC_MapAddNotChanging, 0 );

//-----------------------------------------------------------------------------
// MAPPADD SAVE / RESTORE
//-----------------------------------------------------------------------------
static short MAPADD_SAVE_RESTORE_VERSION = 2;

class CTFMapAdd_SaveRestoreBlockHandler : public CDefSaveRestoreBlockHandler
{
public:
	const char *GetBlockName()
	{
		return "MapAdd";
	}

	//---------------------------------

	void Save( ISave *pSave )
	{
		pSave->WriteBool( &g_bEnableMapAdd );
		if ( IsMapAddAllowed() )
		{
			pSave->WriteAll( &g_LFEMapAdd, g_LFEMapAdd.GetDataDescMap() );
			pSave->WriteInt( &CAI_BaseNPC::m_nDebugBits );
		}
	}

	//---------------------------------

	void WriteSaveHeaders( ISave *pSave )
	{
		pSave->WriteShort( &MAPADD_SAVE_RESTORE_VERSION );
	}

	//---------------------------------

	void ReadRestoreHeaders( IRestore *pRestore )
	{
		// No reason why any future version shouldn't try to retain backward compatability. The default here is to not do so.
		short version;
		pRestore->ReadShort( &version );
		m_fDoLoad = ( version == MAPADD_SAVE_RESTORE_VERSION );
	}

	//---------------------------------

	void Restore( IRestore *pRestore, bool createPlayers )
	{
		if ( m_fDoLoad )
		{
			pRestore->ReadBool( &g_bEnableMapAdd );
			if ( g_bEnableMapAdd )
			{
				pRestore->ReadAll( &g_LFEMapAdd, g_LFEMapAdd.GetDataDescMap() );
				CAI_BaseNPC::m_nDebugBits = pRestore->ReadInt();
			}

			// Force the mapadd convar to match the saved game state
			lfe_mapadd_system_enable.SetValue( g_bEnableMapAdd );
		}
	}

private:
	bool m_fDoLoad;
};

//-----------------------------------------------------------------------------

CTFMapAdd_SaveRestoreBlockHandler g_MapAdd_SaveRestoreBlockHandler;

//-------------------------------------

ISaveRestoreBlockHandler *GetMapAddSaveRestoreBlockHandler()
{
	return &g_MapAdd_SaveRestoreBlockHandler;
}

//-----------------------------------------------------------------------------
// Purpose: MapAdd specific logic_auto replacement.
//			Fires outputs based upon how mapadd mode has been activated.
//-----------------------------------------------------------------------------
class CTFMapAddAuto : public CBaseEntity
{
	DECLARE_CLASS( CTFMapAddAuto, CBaseEntity );
public:
	DECLARE_DATADESC();

	void Spawn(void);
	void Think(void);

	void InputMultiplayerSpawned( inputdata_t &inputdata );

private:
	// fired if mapadd started due to new map
	COutputEvent m_OnMapAddNewGame;

	// fired if commentary was turned on in the middle of a map
	COutputEvent m_OnMapAddMidGame;

	// fired when the player spawns in a multiplayer game
	COutputEvent m_OnMapAddMultiplayerSpawn;
};

LINK_ENTITY_TO_CLASS( mapadd_auto, CTFMapAddAuto );

BEGIN_DATADESC( CTFMapAddAuto )
	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "MultiplayerSpawned", InputMultiplayerSpawned ),

	// Outputs
	DEFINE_OUTPUT(m_OnMapAddNewGame, "OnMapAddNewGame"),
	DEFINE_OUTPUT(m_OnMapAddMidGame, "OnMapAddMidGame"),
	DEFINE_OUTPUT(m_OnMapAddMultiplayerSpawn, "OnMapAddMultiplayerSpawn"),
END_DATADESC()

//------------------------------------------------------------------------------
// Purpose : Fire my outputs here if I fire on map reload
//------------------------------------------------------------------------------
void CTFMapAddAuto::Spawn(void)
{
	BaseClass::Spawn();
	SetNextThink( gpGlobals->curtime + 0.1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapAddAuto::Think(void)
{
	if ( g_LFEMapAdd.MapAddWasEnabledMidGame() )
	{
		m_OnMapAddMidGame.FireOutput(NULL, this);
	}
	else
	{
		m_OnMapAddNewGame.FireOutput(NULL, this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapAddAuto::InputMultiplayerSpawned( inputdata_t &inputdata )
{
	m_OnMapAddMultiplayerSpawn.FireOutput( NULL, this );
}