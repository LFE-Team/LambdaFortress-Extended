//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include <game/client/iviewport.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include <filesystem.h>
#include "IGameUIFuncs.h" // for key bindings

#ifdef _WIN32
#include "winerror.h"
#endif
#include "ixboxsystem.h"
#include "tf_gamerules.h"
#include "tf_controls.h"
#include "tf_shareddefs.h"
#include "tf_mapinfomenu.h"

using namespace vgui;

const char *GetMapDisplayName( const char *mapName );
const char *GetMapType( const char *mapName );
const char *GetMapAuthor( const char *mapName );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMapInfoMenu::CTFMapInfoMenu(IViewPort *pViewPort) : Frame(NULL, PANEL_MAPINFO)
{
	m_pViewPort = pViewPort;

	// load the new scheme early!!
	SetScheme( "ClientScheme" );

	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( false );
	SetProportional( true );
	SetVisible( false );
	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );

	m_pTitle = new CExLabel(this, "MapInfoTitle", " ");

	m_pContinue = new CExButton( this, "MapInfoContinue", "#TF_Continue" );
	m_pBack = new CExButton( this, "MapInfoBack", "#TF_Back" );
	m_pIntro = new CExButton( this, "MapInfoWatchIntro", "#TF_WatchIntro" );

	// info window about this map
	m_pMapInfo = new CExRichText( this, "MapInfoText" );
	m_pMapImage = new ImagePanel( this, "MapImage" );

	m_szMapName[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMapInfoMenu::~CTFMapInfoMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings("Resource/UI/MapInfoMenu.res");

	CheckIntroState();
	CheckBackContinueButtons();

	char mapname[MAX_MAP_NAME];

	Q_FileBase( engine->GetLevelName(), mapname, sizeof(mapname) );

	// Save off the map name so we can re-load the page in ApplySchemeSettings().
	Q_strncpy( m_szMapName, mapname, sizeof( m_szMapName ) );
	Q_strupr( m_szMapName );

	LoadMapPage( m_szMapName );
	SetMapTitle();

	if ( m_pContinue )
	{
		m_pContinue->RequestFocus();
	}

	if ( GameRules() )
	{
		SetDialogVariable( "gamemode", g_pVGuiLocalize->Find( GameRules()->GetGameTypeName() ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::ShowPanel( bool bShow )
{
	if ( IsVisible() == bShow )
		return;

	m_KeyRepeat.Reset();

	if ( bShow )
	{
		Activate();
		SetMouseInputEnabled( true );
		CheckIntroState();
	}
	else
	{
		SetVisible( false );
		SetMouseInputEnabled( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFMapInfoMenu::CheckForIntroMovie()
{
	if ( g_pFullFileSystem->FileExists(TFGameRules()->GetVideoFileForMap() ) )
		return true;

	return false;
}

const char *COM_GetModDirectory();

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFMapInfoMenu::HasViewedMovieForMap()
{
	return ( UTIL_GetMapKeyCount( "viewed" ) > 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::CheckIntroState()
{
	if ( CheckForIntroMovie() && HasViewedMovieForMap() )
	{
		if ( m_pIntro && !m_pIntro->IsVisible() )
		{
			m_pIntro->SetVisible( true );
		}
	}
	else
	{
		if ( m_pIntro && m_pIntro->IsVisible() )
		{
			m_pIntro->SetVisible( false );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::CheckBackContinueButtons()
{
	if ( m_pBack && m_pContinue )
	{
		if ( GetLocalPlayerTeam() == TEAM_UNASSIGNED )
		{
			m_pBack->SetVisible( true );
			m_pContinue->SetText( "#TF_Continue" );
		}
		else
		{
			m_pBack->SetVisible( false );
			m_pContinue->SetText( "#TF_Close" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::OnCommand( const char *command )
{
	m_KeyRepeat.Reset();

	if ( !Q_strcmp( command, "back" ) )
	{
		 // only want to go back to the Welcome menu if we're not already on a team
		if ( !IsX360() && ( GetLocalPlayerTeam() == TEAM_UNASSIGNED ) )
		{
			m_pViewPort->ShowPanel( this, false );
			m_pViewPort->ShowPanel( PANEL_INFO, true );
		}
	}
	else if ( !Q_strcmp( command, "continue" ) )
	{
		m_pViewPort->ShowPanel( this, false );

		if ( CheckForIntroMovie() && !HasViewedMovieForMap() )
		{
			m_pViewPort->ShowPanel( PANEL_INTRO, true );

			UTIL_IncrementMapKey( "viewed" );
		}
		else
		{
			if ( TFGameRules() && TFGameRules()->IsCoOp() )
			{
				// Send the player straight to RED in co-op.
				engine->ClientCmd( VarArgs( "jointeam %s", g_aTeamNames[TF_STORY_TEAM] ) );
			}
			if ( TFGameRules() && TFGameRules()->IsBluCoOp() )
			{
				// Send the player straight to RED in co-op.
				engine->ClientCmd( VarArgs( "jointeam %s", g_aTeamNames[TF_COMBINE_TEAM] ) );
			}
			else if ( GetLocalPlayerTeam() == TEAM_UNASSIGNED )
			{
				m_pViewPort->ShowPanel(PANEL_TEAM, true);
			}

			UTIL_IncrementMapKey( "viewed" );
		}
	}
	else if ( !Q_strcmp( command, "intro" ) )
	{
		m_pViewPort->ShowPanel( this, false );

		if ( CheckForIntroMovie() )
		{
			m_pViewPort->ShowPanel( PANEL_INTRO, true );
		}
		else
		{
			m_pViewPort->ShowPanel( PANEL_TEAM, true );
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::Update()
{
	InvalidateLayout( false, true );
}

//-----------------------------------------------------------------------------
// Purpose: chooses and loads the text page to display that describes mapName map
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::LoadMapPage( const char *mapName )
{
	// load the map image (if it exists for the current map)
	char szMapImage[ MAX_PATH ];
	Q_snprintf( szMapImage, sizeof( szMapImage ), "VGUI/maps/menu_photos_%s", mapName );
	Q_strlower( szMapImage );

	IMaterial *pMapMaterial = materials->FindMaterial( szMapImage, TEXTURE_GROUP_VGUI, false );
	if ( pMapMaterial && !IsErrorMaterial( pMapMaterial ) )
	{
		if ( m_pMapImage )
		{
			if ( !m_pMapImage->IsVisible() )
			{
				m_pMapImage->SetVisible( true );
			}

			// take off the vgui/ at the beginning when we set the image
			Q_snprintf( szMapImage, sizeof( szMapImage ), "maps/menu_photos_%s", mapName );
			Q_strlower( szMapImage );

			m_pMapImage->SetImage( szMapImage );
		}
	}
	else
	{
		if ( m_pMapImage && m_pMapImage->IsVisible() )
		{
			m_pMapImage->SetVisible( false );
		}
	}

	// load the map description files
	char mapLocalizationString[ MAX_PATH ];

	Q_snprintf( mapLocalizationString, sizeof( mapLocalizationString ), "%s_description", mapName );

	// if no map specific description exists, load default text
	if ( !g_pVGuiLocalize->Find( mapLocalizationString ) )
	{
		if ( TFGameRules() )
		{
			const char *pszGameTypeAbbreviation = "";
			if ( TFGameRules()->GetGameType() == TF_GAMETYPE_CTF )
			{
				pszGameTypeAbbreviation = "ctf";
			}
			else if ( TFGameRules()->GetGameType() == TF_GAMETYPE_CP )
			{
				if ( TFGameRules()->IsInKothMode() )
					pszGameTypeAbbreviation = "koth";
				else
					pszGameTypeAbbreviation = "cp";
			}
			else if ( TFGameRules()->GetGameType() == TF_GAMETYPE_ARENA )
			{
				pszGameTypeAbbreviation = "arena";
			}
			else if ( TFGameRules()->GetGameType() == TF_GAMETYPE_ESCORT )
			{
				pszGameTypeAbbreviation = "payload";
			}
			else if ( TFGameRules()->GetGameType() == TF_GAMETYPE_VS )
			{
				pszGameTypeAbbreviation = "vs";
			}

			Q_snprintf( mapLocalizationString, sizeof( mapLocalizationString ), "default_%s_description", pszGameTypeAbbreviation );
		}
	}

	if ( g_pVGuiLocalize->Find( mapLocalizationString ) )
	{
		m_pMapInfo->SetText( g_pVGuiLocalize->Find( mapLocalizationString ) );
	}
	else
	{
		m_pMapInfo->SetText( "" );
	}

	// we haven't loaded a valid map image for the current map
	if ( m_pMapImage && !m_pMapImage->IsVisible() )
	{
		if ( m_pMapInfo )
		{
			m_pMapInfo->SetWide( m_pMapInfo->GetWide() + ( m_pMapImage->GetWide() * 0.75 ) ); // add in the extra space the images would have taken 
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::SetMapTitle()
{
	SetDialogVariable( "mapname", GetMapDisplayName( m_szMapName ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::OnKeyCodePressed( KeyCode code )
{
	m_KeyRepeat.KeyDown( code );

	if ( code == KEY_XBUTTON_A )
	{
		OnCommand( "continue" );
	}
	else if ( code == KEY_XBUTTON_Y )
	{
		OnCommand( "intro" );
	}
	else if( code == KEY_XBUTTON_UP || code == KEY_XSTICK1_UP )
	{
		// Scroll class info text up
		if ( m_pMapInfo )
		{
			PostMessage( m_pMapInfo, new KeyValues("MoveScrollBarDirect", "delta", 1) );
		}
	}
	else if( code == KEY_XBUTTON_DOWN || code == KEY_XSTICK1_DOWN )
	{
		// Scroll class info text up
		if ( m_pMapInfo )
		{
			PostMessage( m_pMapInfo, new KeyValues("MoveScrollBarDirect", "delta", -1) );
		}
	}
	else
	{
		BaseClass::OnKeyCodePressed( code );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::OnKeyCodeReleased( vgui::KeyCode code )
{
	m_KeyRepeat.KeyUp( code );

	BaseClass::OnKeyCodeReleased( code );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMapInfoMenu::OnThink()
{
	vgui::KeyCode code = m_KeyRepeat.KeyRepeated();
	if ( code )
	{
		OnKeyCodePressed( code );
	}

	BaseClass::OnThink();
}

struct s_MapInfo
{
	const char	*pDiskName;
	const char	*pDisplayName;
	const char	*pGameType;
	const char	*pAuthor;
};

struct s_MapTypeInfo
{
	const char	*pDiskPrefix;
	int			iLength;
	const char	*pGameType;
};

static s_MapInfo s_Maps[] = {
	//---------------------- CTF maps ----------------------
	"ctf_2fort",			"2Fort",			"#Gametype_CTF",					"Valve",
	"ctf_well",				"Well",				"#Gametype_CTF",					"Valve",
	"ctf_doublecross",		"Double Cross",		"#Gametype_CTF",					"Valve",
	"ctf_turbine",			"Turbine",			"#Gametype_CTF",					"Flobster",
	"ctf_landfall",			"Landfall",			"#Gametype_CTF",					"Dr. Spud",
	//---------------------- CP maps ----------------------
	"cp_dustbowl",			"Dustbowl",			"#Gametype_AttackDefense",			"Valve",
	"cp_gravelpit",			"Gravel Pit",		"#Gametype_AttackDefense",			"Valve",
	"cp_gorge",				"Gorge",			"#Gametype_AttackDefense",			"Valve",
	"cp_mountainlab",		"Mountain Lab",		"#Gametype_AttackDefense",			"Valve, 3Dnj",
	"cp_granary",			"Granary",			"#Gametype_CP",						"Valve",
	"cp_well",				"Well",				"#Gametype_CP",						"Valve",
	"cp_foundry",			"Foundry",			"#Gametype_CP",						"Valve",
	"cp_badlands",			"Badlands",			"#Gametype_CP",						"Valve",
	"cp_powerhouse",		"Powerhouse",		"#Gametype_CP",						"Valve",
	//---------------------- TC maps ----------------------
	"tc_hydro",				"Hydro",			"#TF_TerritoryControl",				"Valve",
	//---------------------- PL maps ----------------------
	"pl_goldrush",			"Gold Rush",		"#Gametype_Escort",					"Valve",
	"pl_badwater",			"Badwater Basin",	"#Gametype_Escort",					"Valve",
	"pl_thundermountain",	"Thunder Mountain",	"#Gametype_Escort",					"Valve",
	"pl_barnblitz",			"Barnblitz",		"#Gametype_Escort",					"Valve",
	"pl_upward",			"Upward",			"#Gametype_EscortRace",				"Valve",
	"plr_pipeline",			"Pipeline",			"#Gametype_EscortRace",				"Valve",
	"plr_hightower",		"Hightower",		"#Gametype_EscortRace",				"Valve",
	//---------------------- KOTH maps ----------------------
	"koth_king",			"Kong King",		"#Gametype_Koth",					"3Dnj",
	"koth_nucleus",			"Nucleus",			"#Gametype_Koth",					"Valve",
	"koth_sawmill",			"Sawmill (KOTH)",	"#Gametype_Koth",					"Valve",
	//---------------------- ARENA maps ----------------------
	"arena_sawmill",		"Sawmill (Arena)",	"#Gametype_Arena",					"Valve",
	//---------------------- HL1 maps ----------------------
	"t0a0",					"Hazard Course (Part 1)",		"#Gametype_Training",		"Valve",
	"t0a0a",				"Hazard Course (Part 2)",		"#Gametype_Training",		"Valve",
	"t0a0b",				"Hazard Course (Part 3",		"#Gametype_Training",		"Valve",
	"t0a0b1",				"Hazard Course (Part 4)",		"#Gametype_Training",		"Valve",
	"t0a0b2",				"Hazard Course (Part 5)",		"#Gametype_Training",		"Valve",
	"t0a0c",				"Hazard Course (Part 6)",		"#Gametype_Training",		"Valve",
	"t0a0d",				"Hazard Course (Part 7)",		"#Gametype_Training",		"Valve",
	"c0a0",					"Black Mesa Inbound (Part 1)",	"#Gametype_CoOp",		"Valve",
	"c0a0a",				"Black Mesa Inbound (Part 2)",	"#Gametype_CoOp",		"Valve",
	"c0a0b",				"Black Mesa Inbound (Part 3)",	"#Gametype_CoOp",		"Valve",
	"c0a0c",				"Black Mesa Inbound (Part 4)",	"#Gametype_CoOp",		"Valve",
	"c0a0d",				"Black Mesa Inbound (Part 5)",	"#Gametype_CoOp",		"Valve",
	"c0a0e",				"Black Mesa Inbound (Part 6)",	"#Gametype_CoOp",		"Valve",
	"c1a0",					"Anomalous Materials (Part 1)",	"#Gametype_CoOp",		"Valve",
	"c1a0d",				"Anomalous Materials (Part 2)",	"#Gametype_CoOp",		"Valve",
	"c1a0a",				"Anomalous Materials (Part 3)",	"#Gametype_CoOp",		"Valve",
	"c1a0b",				"Anomalous Materials (Part 4)",	"#Gametype_CoOp",		"Valve",
	"c1a0e",				"Anomalous Materials (Part 5)",	"#Gametype_CoOp",		"Valve",
	"c1a1a",				"Unforseen Consequences (Part 1)",	"#Gametype_CoOp",	"Valve",
	"c1a1f",				"Unforseen Consequences (Part 2)",	"#Gametype_CoOp",	"Valve",
	"c1a1b",				"Unforseen Consequences (Part 3)",	"#Gametype_CoOp",	"Valve",
	"c1a1c",				"Unforseen Consequences (Part 4)",	"#Gametype_CoOp",	"Valve",
	"c1a1d",				"Unforseen Consequences (Part 5)",	"#Gametype_CoOp",	"Valve",
	"c1a2",					"Office Complex (Part 1)",		"#Gametype_CoOp",		"Valve",
	"c1a2a",				"Office Complex (Part 2)",		"#Gametype_CoOp",		"Valve",
	"c1a2b",				"Office Complex (Part 3)",		"#Gametype_CoOp",		"Valve",
	"c1a2c",				"Office Complex (Part 4)",		"#Gametype_CoOp",		"Valve",
	"c1a2d",				"Office Complex (Part 5)",		"#Gametype_CoOp",		"Valve",
	"c1a3",					"We've Got Hostiles (Part 1)",	"#Gametype_CoOp",		"Valve",
	"c1a3a",				"We've Got Hostiles (Part 2)",	"#Gametype_CoOp",		"Valve",
	"c1a3b",				"We've Got Hostiles (Part 3)",	"#Gametype_CoOp",		"Valve",
	"c1a3c",				"We've Got Hostiles (Part 4)",	"#Gametype_CoOp",		"Valve",
	"c1a3d",				"We've Got Hostiles (Part 5)",	"#Gametype_CoOp",		"Valve",
	"c1a4",					"Blast Pit (Part 1)",			"#Gametype_CoOp",		"Valve",
	"c1a4k",				"Blast Pit (Part 2)",			"#Gametype_CoOp",		"Valve",
	"c1a4b",				"Blast Pit (Part 3)",			"#Gametype_CoOp",		"Valve",
	"c1a4f",				"Blast Pit (Part 4)",			"#Gametype_CoOp",		"Valve",
	"c1a4d",				"Blast Pit (Part 5)",			"#Gametype_CoOp",		"Valve",
	"c1a4e",				"Blast Pit (Part 6)",			"#Gametype_CoOp",		"Valve",
	"c1a4i",				"Blast Pit (Part 7)",			"#Gametype_CoOp",		"Valve",
	"c1a4g",				"Blast Pit (Part 8)",			"#Gametype_CoOp",		"Valve",
	"c1a4j",				"Blast Pit (Part 9)",			"#Gametype_CoOp",		"Valve",
	"c2a1",					"Power Up (Part 1)",			"#Gametype_CoOp",		"Valve",
	"c2a1a",				"Power Up (Part 2)",			"#Gametype_CoOp",		"Valve",
	"c2a1b",				"Power Up (Part 3)",			"#Gametype_CoOp",		"Valve",
	"c2a2",					"On A Rail (Part 1)",			"#Gametype_CoOp",		"Valve",
	"c2a2a",				"On A Rail (Part 2)",			"#Gametype_CoOp",		"Valve",
	"c2a2b1",				"On A Rail (Part 3)",			"#Gametype_CoOp",		"Valve",
	"c2a2b2",				"On A Rail (Part 4)",			"#Gametype_CoOp",		"Valve",
	"c2a2c",				"On A Rail (Part 5)",			"#Gametype_CoOp",		"Valve",
	"c2a2d",				"On A Rail (Part 6)",			"#Gametype_CoOp",		"Valve",
	"c2a2f",				"On A Rail (Part 7)",			"#Gametype_CoOp",		"Valve",
	"c2a2g",				"On A Rail (Part 8)",			"#Gametype_CoOp",		"Valve",
	"c2a2h",				"On A Rail (Part 9)",			"#Gametype_CoOp",		"Valve",
	"c2a3",					"Apprehension (Part 1)",		"#Gametype_CoOp",		"Valve",
	"c2a3a",				"Apprehension (Part 2)",		"#Gametype_CoOp",		"Valve",
	"c2a3b",				"Apprehension (Part 3)",		"#Gametype_CoOp",		"Valve",
	"c2a3c",				"Apprehension (Part 4)",		"#Gametype_CoOp",		"Valve",
	"c2a3d",				"Apprehension (Part 5)",		"#Gametype_CoOp",		"Valve",
	"c2a3e",				"Apprehension (Part 6)",		"#Gametype_CoOp",		"Valve",
	"c2a4",					"Residue Processing (Part 1)",	"#Gametype_CoOp",		"Valve",
	"c2a4a",				"Residue Processing (Part 2)",	"#Gametype_CoOp",		"Valve",
	"c2a4b",				"Residue Processing (Part 3)",	"#Gametype_CoOp",		"Valve",
	"c2a4c",				"Residue Processing (Part 4)",	"#Gametype_CoOp",		"Valve",
	"c2a4d",				"Questionable Ethics (Part 1)",	"#Gametype_CoOp",		"Valve",
	"c2a4e",				"Questionable Ethics (Part 2)",	"#Gametype_CoOp",		"Valve",
	"c2a4f",				"Questionable Ethics (Part 3)",	"#Gametype_CoOp",		"Valve",
	"c2a4g",				"Questionable Ethics (Part 4)",	"#Gametype_CoOp",		"Valve",
	"c2a5",					"Surface Tension (Part 1)",		"#Gametype_CoOp",		"Valve",
	"c2a5w",				"Surface Tension (Part 2)",		"#Gametype_CoOp",		"Valve",
	"c2a5x",				"Surface Tension (Part 3)",		"#Gametype_CoOp",		"Valve",
	"c2a5a",				"Surface Tension (Part 4)",		"#Gametype_CoOp",		"Valve",
	"c2a5b",				"Surface Tension (Part 5)",		"#Gametype_CoOp",		"Valve",
	"c2a5c",				"Surface Tension (Part 6)",		"#Gametype_CoOp",		"Valve",
	"c2a5d",				"Surface Tension (Part 7)",		"#Gametype_CoOp",		"Valve",
	"c2a5e",				"Surface Tension (Part 8)",		"#Gametype_CoOp",		"Valve",
	"c2a5f",				"Surface Tension (Part 9)",		"#Gametype_CoOp",		"Valve",
	"c2a5g",				"Surface Tension (Part 10)",	"#Gametype_CoOp",		"Valve",
	"c3a1",					"Forget About Red Team! (Part 1)",	"#Gametype_CoOp",	"Valve",
	"c3a1a",				"Forget About Red Team! (Part 2)",	"#Gametype_CoOp",	"Valve",
	"c3a1b",				"Forget About Red Team! (Part 3)",	"#Gametype_CoOp",	"Valve",
	"c3a2e",				"Lambda Core (Part 1)",			"#Gametype_CoOp",		"Valve",
	"c3a2",					"Lambda Core (Part 2)",			"#Gametype_CoOp",		"Valve",
	"c3a2a",				"Lambda Core (Part 3)",			"#Gametype_CoOp",		"Valve",
	"c3a2b",				"Lambda Core (Part 4)",			"#Gametype_CoOp",		"Valve",
	"c3a2c",				"Lambda Core (Part 5)",			"#Gametype_CoOp",		"Valve",
	"c3a2d",				"Lambda Core (Part 6)",			"#Gametype_CoOp",		"Valve",
	"c3a2f",				"Lambda Core (Part 7)",			"#Gametype_CoOp",		"Valve",
	"c4a1",					"Xen",			"				#Gametype_CoOp",		"Valve",
	"c4a2",					"Gonarch's Lair (Part 1)",		"#Gametype_CoOp",		"Valve",
	"c4a2a",				"Gonarch's Lair (Part 2)",		"#Gametype_CoOp",		"Valve",
	"c4a2b",				"Gonarch's Lair (Part 3)",		"#Gametype_CoOp",		"Valve",
	"c4a1a",				"Interloper (Part 1)",			"#Gametype_CoOp",		"Valve",
	"c4a1b",				"Interloper (Part 2)",			"#Gametype_CoOp",		"Valve",
	"c4a1c",				"Interloper (Part 3)",			"#Gametype_CoOp",		"Valve",
	"c4a1d",				"Interloper (Part 4)",			"#Gametype_CoOp",		"Valve",
	"c4a1e",				"Interloper (Part 5)",			"#Gametype_CoOp",		"Valve",
	"c4a1f",				"Interloper (Part 6)",			"#Gametype_CoOp",		"Valve",
	"c4a3",					"Nihilanth (Finale)",			"#Gametype_CoOp",		"Valve",
	"c5a1",					"Outro (Ending)",				"#Gametype_CoOp",		"Valve",
	//---------------------- HL2 maps ----------------------
	"d1_trainstation_01",	"Point Insertion (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_trainstation_02",	"Point Insertion (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_trainstation_03",	"Point Insertion (Part 3)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_trainstation_04",	"Point Insertion (Part 4)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_trainstation_05",	"A Red Letter Day (Part 1)", "#Gametype_CoOp",			"Train, Valve",
	"d1_trainstation_06",	"A Red Letter Day (Part 2)", "#Gametype_CoOp",			"Train, Valve",
	"d1_canals_01",			"Route Kanal (Part 1)",		"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_01a",		"Route Kanal (Part 2)",		"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_02",			"Route Kanal (Part 3)",		"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_03",			"Route Kanal (Part 4)",		"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_04",			"What the FUCK?",			"#Gametype_CoOp",			"what?",
	"d1_canals_05",			"Route Kanal (Part 5)",		"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_06",			"Water Hazard (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_07",			"Water Hazard (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_08",			"Water Hazard (Part 3)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_09",			"Water Hazard (Part 4)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_10",			"Water Hazard (Part 5)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_11",			"Water Hazard (Part 6)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_12",			"Water Hazard (Part 7)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_canals_13",			"Water Hazard (Part 8)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_eli_01",			"Black Mesa East (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_eli_02",			"Black Mesa East (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d1_town_01",			"We don't go to Ravenholm (Part 1)", "#Gametype_CoOp",	"Train, Valve",
	"d1_town_01a",			"We don't go to Ravenholm (Part 2)", "#Gametype_CoOp",	"Train, Valve",
	"d1_town_02",			"We don't go to Ravenholm (Part 3)", "#Gametype_CoOp",	"Train, Valve",
	"d1_town_03",			"We don't go to Ravenholm (Part 4)", "#Gametype_CoOp",	"Train, Valve",
	"d1_town_02_p2",        "We don't go to Ravenholm (Part 5)", "#Gametype_CoOp",  "Train, Valve",
	"d1_town_02a",          "We don't go to Ravenholm (Part 6)", "#Gametype_CoOp",  "Train, Valve",
	"d1_town_04",			"We don't go to Ravenholm (Part 7)", "#Gametype_CoOp",	"Train, Valve",
	"d1_town_05",			"We don't go to Ravenholm (Part 8)", "#Gametype_CoOp",	"Train, Valve",
	"d2_coast_01",			"Highway 17 (Part 1)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_lostcoast",			"Lost Coast",				"#Gametype_CoOp",			"Valve",
	"d2_coast_02",			"What the FUCK?",			"#Gametype_CoOp",			"This is very wrong.",
	"d2_coast_03",			"Highway 17 (Part 2)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_04",			"Highway 17 (Part 3)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_05",			"Highway 17 (Part 4)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_06",			"What the FUCK?",			"#Gametype_CoOp",			"AAAAAAAAA no.",
	"d2_coast_07",			"Highway 17 (Part 5)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_08",			"Highway 17 (Part 6)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_07_p2",       "Highway 17 (Part 7)",      "#Gametype_CoOp",           "Train, Valve",
	"d2_coast_09",			"Sandtraps (Part 1)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_10",			"Sandtraps (Part 2)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_11",			"Sandtraps (Part 3)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_coast_12",			"Sandtraps (Part 4)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_01",			"Sandtraps (Part 5)",		"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_02",			"Nova Prospekt (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_03",			"Nova Prospekt (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_04",			"Nova Prospekt (Part 3)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_05",			"Nova Prospekt (Part 4)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_06",			"Entanglement (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_07",			"Entanglement (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d2_prison_08",			"Entanglement (Part 3)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_01",			"Entanglement (Part 4)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_02",			"Anticitizen One (Part 1)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_03",			"Anticitizen One (Part 2)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_04",			"Anticitizen One (Part 3)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_05",			"Anticitizen One (Part 4)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_06a",			"Anticitizen One (Part 5)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_06b",			"Anticitizen One (Part 6)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_07",			"Anticitizen One (Part 7)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_08",			"Anticitizen One (Part 8)",	"#Gametype_CoOp",			"Train, Valve",
	"d3_c17_09",			"Follow Freeman! (Part 1)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_10a",			"Follow Freeman! (Part 2)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_10b",			"Follow Freeman! (Part 3)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_11",			"Follow Freeman! (Part 4)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_12",			"Follow Freeman! (Part 5)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_12b",			"Follow Freeman! (Part 6)", "#Gametype_CoOp",			"Train, Valve",
	"d3_c17_13",			"Follow Freeman! (Part 7)", "#Gametype_CoOp",			"Train, Valve",
	"d3_citadel_01",		"Our Benefactors (Part 1)", "#Gametype_CoOp",			"Train, Valve",
	"d3_citadel_02",		"Our Benefactors (Part 2)", "#Gametype_CoOp",			"Train, Valve",
	"d3_citadel_03",		"Our Benefactors (Part 3)", "#Gametype_CoOp",			"Train, Valve",
	"d3_citadel_04",		"Our Benefactors (Part 4)", "#Gametype_CoOp",			"Train, Valve",
	"d3_citadel_05",		"Our Benefactors (Part 5)", "#Gametype_CoOp",			"Train, Valve",
	"d3_breen_01",			"Dark Energy (Finale)",		"#Gametype_CoOp",			"Train, Valve",
	//---------------------- EP1 maps ----------------------
	"ep1_citadel_00",       "Undue Alarm (Part 1)",     "#Gametype_CoOp",           "Train, Valve",
	"ep1_citadel_01",       "Undue Alarm (Part 2)",     "#Gametype_CoOp",           "Train, Valve",
	"ep1_citadel_02",       "Undue Alarm (Part 3)",     "#Gametype_CoOp",           "Train, Valve",
	"ep1_citadel_02b",      "Undue Alarm (Part 4)",     "#Gametype_CoOp",           "Train, Valve",
	"ep1_citadel_03",       "Direct Intervention (Part 1)", "#Gametype_CoOp",       "Train, Valve",
	"ep1_citadel_04",       "Direct Intervention (Part 2)", "#Gametype_CoOp",       "Train, Valve",
	"ep1_c17_00",           "Lowlife (Part 1)",         "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_00a",          "Lowlife (Part 2)",         "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_01",           "Urban Flight (Part 1)",    "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_01a",          "Urban Flight (Part 2)",    "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_02",           "Urban Flight (Part 3)",    "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_02b",          "Urban Flight (Part 4)",    "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_02a",          "Urban Flight (Part 5)",    "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_05",           "Exit 17 (Part 1)",         "#Gametype_CoOp",           "Train, Valve",
	"ep1_c17_06",           "Exit 17 (Finale)",         "#Gametype_CoOp",           "Train, Valve",
	//---------------------- EP2 maps ----------------------
	"ep2_outland_01",		"To The White Forest (Part 1)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_01a",		"To The White Forest (Part 1)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_02",		"This Vortal Coil (Part 1)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_02_p2",	"This Vortal Coil (Part 3.5)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_03",		"This Vortal Coil (Part 2)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_04",		"This Vortal Coil (Part 3)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_05",		"Freeman Pontifex (Part 1)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_06",		"Freeman Pontifex (Part 2)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_06a",		"Riding Shotgun (Part 1)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_07",		"Riding Shotgun (Part 2)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_08",		"Riding Shotgun (Part 3)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_09",		"Under The Radar (Part 1)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_10",		"Under The Radar (Part 2)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_10a",		"Under The Radar (Part 3)",		"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_11",		"Our Mutual Fiend (Part 1)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_11a",		"Our Mutual Fiend (Part 2)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_11b",		"Our Mutual Fiend (Part 3)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_12",		"Our Mutual Fiend (Part 4)",	"#Gametype_CoOp",		"Train, Valve",
	"ep2_outland_12a",		"T-Minus One (Finale)",			"#Gametype_CoOp",		"Train, Valve",
	//---------------------- some random maps i found in discord ----------------------
	"lfe_dust2",			"Dust2",	"are we playing css now?",					"Banniflower, Valve",
};

static s_MapTypeInfo s_MapTypes[] = {
	"cp_",		3, "#Gametype_CP",
	"ctf_",		4, "#Gametype_CTF",
	"pl_",		3, "#Gametype_Escort",
	"plr_",		4, "#Gametype_EscortRace",
	"koth_",	5, "#Gametype_Koth",
	"arena_",	6, "#Gametype_Arena",
	"tr_",		3, "#Gametype_Training",
	"tc_",		3, "#TF_TerritoryControl",
	"d1_",		3, "#Gametype_CoOp",
	"d2_",		3, "#Gametype_CoOp",
	"d3_",		3, "#Gametype_CoOp",
	"ep1_",		3, "#Gametype_CoOp",
	"ep2_",		3, "#Gametype_CoOp",
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *GetMapDisplayName( const char *mapName )
{
	static char szDisplayName[256];
	char szTempName[256];
	const char *pszSrc = NULL;

	szDisplayName[0] = '\0';

	if ( !mapName )
		return szDisplayName;

	/*
	// check the worldspawn entity to see if the map author has specified a name
	if ( GetClientWorldEntity() )
	{
	const char *pszMapDescription = GetClientWorldEntity()->m_iszMapDescription;
	if ( Q_strlen( pszMapDescription ) > 0 )
	{
	Q_strncpy( szDisplayName, pszMapDescription, sizeof( szDisplayName ) );
	Q_strupr( szDisplayName );

	return szDisplayName;
	}
	}
	*/
	// check our lookup table
	Q_strncpy( szTempName, mapName, sizeof( szTempName ) );
	Q_strlower( szTempName );

	for ( int i = 0; i < ARRAYSIZE(s_Maps); ++i )
	{
		if ( !Q_stricmp( s_Maps[i].pDiskName, szTempName ) )
		{
			return s_Maps[i].pDisplayName;
		}
	}

	// we haven't found a "friendly" map name, so let's just clean up what we have
	pszSrc = szTempName;

	for ( int i = 0; i < ARRAYSIZE(s_MapTypes); ++i )
	{
		if ( !Q_strncmp( mapName, s_MapTypes[i].pDiskPrefix, s_MapTypes[i].iLength ) )
		{
			pszSrc = szTempName + s_MapTypes[i].iLength;
			break;
		}
	}

	Q_strncpy( szDisplayName, pszSrc, sizeof(szDisplayName) );
	Q_strupr( szDisplayName );

	return szDisplayName;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *GetMapType( const char *mapName )
{
	if ( mapName )
	{
		// Have we got a registered map named that?
		for ( int i = 0; i < ARRAYSIZE(s_Maps); ++i )
		{
			if ( !Q_stricmp(s_Maps[i].pDiskName, mapName) )
			{
				// If so, return the registered gamemode
				return s_Maps[i].pGameType;
			}
		}
		// If not, see what the prefix is and try and guess from that
		for ( int i = 0; i < ARRAYSIZE(s_MapTypes); ++i )
		{
			if ( !Q_strncmp( mapName, s_MapTypes[i].pDiskPrefix, s_MapTypes[i].iLength ) )
				return s_MapTypes[i].pGameType;
		}
	}

	return "";
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *GetMapAuthor( const char *mapName )
{
	if ( mapName )
	{
		// Have we got a registered map named that?
		for ( int i = 0; i < ARRAYSIZE(s_Maps); ++i )
		{
			if ( !Q_stricmp(s_Maps[i].pDiskName, mapName) )
			{
				// If so, return the registered author
				return s_Maps[i].pAuthor;
			}
		}
	}

	return ""; // Otherwise, return NULL
}