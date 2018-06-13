//========= Copyright � 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "tf_controls.h"
#include "c_tf_playerresource.h"
#include "c_tf_player.h"
#include "tf_hud_deathmatchstatus.h"
#include "tf_hud_freezepanel.h"


using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudDeathMatchObjectives::CTFHudDeathMatchObjectives( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_flNextThink = 0.0f;

	ListenForGameEvent( "player_death" );
	ListenForGameEvent( "teamplay_setup_finished" );
	ListenForGameEvent( "teamplay_update_timer" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudDeathMatchObjectives::IsVisible( void )
{
	if ( IsTakingAFreezecamScreenshot() )
		return false;

	if ( GetLocalPlayerTeam() < FIRST_GAME_TEAM )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudObjectiveDeathMatchPanel.res" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::SetPlayingToLabelVisible( bool bVisible )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::OnThink( void )
{
	if ( IsVisible() && m_flNextThink < gpGlobals->curtime )
	{
		UpdateStatus();
		m_flNextThink = gpGlobals->curtime + 0.5f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::UpdateStatus( void )
{
	C_TF_PlayerResource *tf_PR = GetTFPlayerResource();
	if ( !tf_PR )
		return;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	int iLocalIndex = GetLocalPlayerIndex();
	int iLocalScore = tf_PR->GetTotalScore( iLocalIndex );

	SetDialogVariable( "playerscore", iLocalScore );

	int iBestIndex = 0;
	int iBestScore = 0;

	for ( int playerIndex = 1; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if ( g_PR->IsConnected( playerIndex ) )
		{
			int score = tf_PR->GetTotalScore( playerIndex );

			if ( playerIndex != iLocalIndex && score >= iBestScore )
			{
				iBestScore = score;
				iBestIndex = playerIndex;
			}
		}
	}

	if ( iBestIndex )
	{
		SetDialogVariable( "bestplayername", tf_PR->GetPlayerName( iBestIndex ) );
		SetDialogVariable( "bestscore", iBestScore );
	}
	else
	{
		SetDialogVariable( "bestplayername", "" );
		SetDialogVariable( "bestscore", "" );
	}


	int iDeltaScore = iLocalScore - iBestScore;
	char scoreString[64];
	Color szColor;
	if ( iDeltaScore > 0 )
	{
		Q_snprintf( scoreString, sizeof( scoreString ), "+%i", iDeltaScore );
		szColor = scheme()->GetIScheme( GetScheme() )->GetColor( "StoreGreen", Color( 0, 255, 0, 255 ) );
	}
	else if ( iDeltaScore < 0 )
	{
		Q_snprintf( scoreString, sizeof( scoreString ), "%i", iDeltaScore );
		szColor = scheme()->GetIScheme( GetScheme() )->GetColor( "LowHealthRed", Color( 0, 255, 0, 255 ) );
	}
	else
	{
		Q_snprintf( scoreString, sizeof( scoreString ), "%i", iDeltaScore );
		szColor = scheme()->GetIScheme( GetScheme() )->GetColor( "HudBlack", Color( 0, 255, 0, 255 ) );
	}
	dynamic_cast<CExLabel *>( FindChildByName( "DeltaScore" ) )->SetFgColor( szColor );
	SetDialogVariable( "deltascore", iDeltaScore );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathMatchObjectives::FireGameEvent( IGameEvent *event )
{
	if ( IsVisible() )
	{
		m_flNextThink = gpGlobals->curtime;
	}
}