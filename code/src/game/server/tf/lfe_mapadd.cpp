//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: The system for handling extra scripted entity in-game.
//
//=============================================================================//

#include "cbase.h"

#include "igamesystem.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "utldict.h"
#include "isaverestore.h"
#include "eventqueue.h"
#include "saverestore_utlvector.h"
#include "ai_basenpc.h"
#include "Sprite.h"
#include "datacache/imdlcache.h"
#include "lfe_mapadd.h"
#include "tf_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAPADD_SPAWNED_SEMAPHORE		"mapp_semaphore"

CTFMapAddSystem g_LFEMapAdd("CTFMapAddSystem");

bool g_bEnableMapAdd = false;

//===========================================================================================================
// LFE MAPPADD GAME SYSTEM
//===========================================================================================================
void CV_GlobalChange_LFEMapAdd( IConVar *var, const char *pOldString, float flOldValue );

BEGIN_DATADESC_NO_BASE( CTFMapAddSystem )
	//int m_afPlayersLastButtons;			DON'T SAVE
	//bool m_bMapAddConvarsChanging;	DON'T SAVE
	//int m_iClearPressedButtons;			DON'T SAVE

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
void CTFMapAddSystem::LevelInitPreEntity( void )
{
	m_bMapAddConvarsChanging = false;
	m_iClearPressedButtons = 0;

	g_bEnableMapAdd = TFGameRules()->IsMapAddAllowed();

	CalculateMapAddState();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::CalculateMapAddState( void )
{
	// Set the available cvar if we can find mapadd data for this level
	char szFullName[512];
	Q_snprintf(szFullName,sizeof(szFullName), "scripts/mapadd/%s.txt", STRING( gpGlobals->mapname) );
	if ( !filesystem->FileExists( szFullName ) )
	{
		//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Cannot find mapadd data: %s. \n", szFullName );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::LevelShutdownPreEntity( void )
{
	ShutDownMapAdd();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ParseEntityCreate( KeyValues *pSubKey )
{
	if ( pSubKey )
	{
		KeyValues *pCreateSub = pSubKey->GetFirstSubKey();

		while ( pCreateSub )
		{
			const char *pMapAddEntName = pSubKey->GetName();
			KeyValues *pClassname = pCreateSub->FindKey( "classname" );
			if ( pClassname )
			{
				// Use the classname instead
				pMapAddEntName = pClassname->GetString();
			}

			// Spawn the mapadd entity
			CBaseEntity *pMapAddEnt = CreateEntityByName( pMapAddEntName );
			if ( pMapAddEnt )
			{
				ParseEntityKVBlock( pMapAddEnt, pCreateSub );
				DispatchSpawn( pMapAddEnt );

				EHANDLE hHandle;
				hHandle = pMapAddEnt;
				m_hSpawnedEntities.AddToTail( hHandle );
				//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Creating %s. \n", pMapAddEntName );
			}
			else
			{
				Warning( "MapAdd: Failed to spawn entity, type: '%s'\n", pMapAddEntName );
			}

			// Move to next entity
			pCreateSub = pCreateSub->GetNextKey();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ParseEntityKVBlock( CBaseEntity *pNode, KeyValues *pkvNode )
{
	KeyValues *pkvNodeData = pkvNode->GetFirstSubKey();
	while ( pkvNodeData )
	{
		// Handle the connections block
		if ( !Q_strcmp(pkvNodeData->GetName(), "connections") )
		{
			ParseEntityKVBlock( pNode, pkvNodeData );
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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ParseEntityDelete( KeyValues *pSubKey )
{
	if ( pSubKey )
	{
		KeyValues *pDeleteSub = pSubKey->GetFirstSubKey();
		while ( pDeleteSub )
		{
			const char *pMapAddEntName = pSubKey->GetName();
			KeyValues *pClassname = pDeleteSub->FindKey( "targetname" );
			if ( pClassname )
			{
				// Use the classname instead
				pMapAddEntName = pClassname->GetString();
			}

			// Remove the mapadd entity
			CBaseEntity *pMapAddEnt = gEntList.FindEntityByName( NULL, pMapAddEntName );
			if ( pMapAddEnt )
			{
				UTIL_Remove( pMapAddEnt );
				//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Deleting %s. \n", pMapAddEntName );
			}
			else
			{
				Warning("MapAdd: Failed to remove entity, type: '%s'\n", pMapAddEntName );
			}

			// Move to next entity
			pDeleteSub = pDeleteSub->GetNextKey();
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ParseEntityEdit( KeyValues *pSubKey )
{
	if ( pSubKey )
	{
		KeyValues *pEditSub = pSubKey->GetFirstSubKey();
		while ( pEditSub )
		{
			const char *pMapAddEntName = pSubKey->GetName();
			KeyValues *pClassname = pEditSub->FindKey( "targetname" );
			if ( pClassname )
			{
				// Use the classname instead
				pMapAddEntName = pClassname->GetString();
			}

			KeyValues *pValues = pEditSub->FindKey( "values" );
			if ( pValues )
			{
				KeyValues *pValuesSub = pValues->GetFirstSubKey();
				while ( pValuesSub )
				{
					// Spawn the mapadd entity
					CBaseEntity *pMapAddEnt = gEntList.FindEntityByName( NULL, pMapAddEntName );
					if ( pMapAddEnt )
					{
						pMapAddEnt->KeyValue( pValuesSub->GetName(), pValuesSub->GetString() );
						//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Editing %s. \n", pMapAddEntName );
					}
					else
					{
						Warning( "MapAdd: Failed to edit entity, type: '%s'\n", pMapAddEntName );
					}

					// Move to next entity
					pValuesSub = pValuesSub->GetNextKey();
				}
			}

			// Move to next entity
			pEditSub = pEditSub->GetNextKey();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::SetMapAddConvarsChanging( bool bChanging )
{
	m_bMapAddConvarsChanging = bChanging;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ConvarChanged( IConVar *pConVar, const char *pOldString, float flOldValue )
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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::InitMapAdd( void )
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
	Q_snprintf(szFullName,sizeof(szFullName), "scripts/mapadd/%s.txt", STRING( gpGlobals->mapname ));
	KeyValues *pkvFile = new KeyValues( "MapAdd" );
	if ( pkvFile->LoadFromFile( filesystem, szFullName, "MOD" ) )
	{
		//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Loading mapadd data from %s. \n", szFullName );

		// Load each mapadd block, and spawn the entities
		KeyValues *pkvEnt = pkvFile->GetFirstSubKey();
		while ( pkvEnt )
		{
			// Get the name
			const char *pMapAddName = pkvEnt->GetName();

			// Skip the comment
			if ( !Q_strncmp( pMapAddName, "comment", 7 ) )
			{
				pkvEnt = pkvEnt->GetNextKey();
				continue;
			}

			if ( !Q_strncmp( pMapAddName, "entity", 6 ) )
			{
				// create
				ParseEntityCreate( pkvEnt->FindKey( "create" ) );

				// destory
				ParseEntityDelete( pkvEnt->FindKey( "delete" ) );

				// edit
				ParseEntityEdit( pkvEnt->FindKey( "edit" ) );

				// Move to next entity
				pkvEnt = pkvEnt->GetNextKey();
				continue;
			}

			// command
			if ( !Q_strncmp( pMapAddName, "console", 7 ) )
			{
				//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Executing commands. \n", NULL );
				KeyValues *pCmdSub = pkvEnt->GetFirstSubKey();
				while ( pCmdSub )
				{
					const char *pMapAddCmdName = pCmdSub->GetName();
					// Execute the command
					engine->ServerCommand( UTIL_VarArgs( "%s %d\n", pMapAddCmdName, pCmdSub->GetInt() ) );

					// Move to next command
					pCmdSub = pCmdSub->GetNextKey();
				}

				// Move to next entity
				pkvEnt = pkvEnt->GetNextKey();
				continue;
			}

			pkvFile->deleteThis();
			// Move to next entity
			pkvEnt = pkvEnt->GetNextKey();
		}

		// Then activate all the entities
		bool bAsyncAnims = mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, false );
		for ( int i = 0; i < m_hSpawnedEntities.Count(); i++ )
		{
			m_hSpawnedEntities[i]->Activate();
		}
		mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, bAsyncAnims );
	}
	else
	{
		//ConColorMsg( Color( 77, 116, 85, 255 ), "[MapAdd] Cannot find mapadd data '%s'. \n", szFullName );
	}

	engine->LockNetworkStringTables( oldLock );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::ShutDownMapAdd( void )
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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::SetMapAddMode( bool bEnableMapAdd )
{
	g_bEnableMapAdd = bEnableMapAdd;
	CalculateMapAddState();

	// If we're turning on mapadd, create all the entities.
	if ( bEnableMapAdd )
	{
		InitMapAdd();
	}
	else
	{
		ShutDownMapAdd();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMapAddSystem::OnRestore( void )
{
	cvar->RemoveGlobalChangeCallback( CV_GlobalChange_LFEMapAdd );

	if ( !TFGameRules()->IsMapAddAllowed() )
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