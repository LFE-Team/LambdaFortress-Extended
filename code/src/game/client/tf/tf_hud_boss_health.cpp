//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
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

#include "tf_gamerules.h"
#include "tf_hud_freezepanel.h"
#include "tf_mainmenu.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudBossHealthMeter );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudBossHealthMeter::CHudBossHealthMeter( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pHealthImage = new EditablePanel( this, "BarImage" );	
	m_pHealthImageBG = new ImagePanel( this, "BorderImage" );

	m_flNextThink = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
	m_iBossHealthPercentageByte = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudBossHealth.res" );

	m_flNextThink = 0.0f;

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::SetHealth( int iNewHealth, int iMaxHealth )
{
	int nPrevHealth = m_iBossHealthPercentageByte;

	// set our health
	m_iBossHealthPercentageByte = iNewHealth;
	m_nMaxHealth = iMaxHealth;
	m_pHealthImage->SetHealth( (float)(m_iBossHealthPercentageByte) / (float)(m_nMaxHealth) );

	if ( m_pHealthImage )
	{
		m_pHealthImage->SetFgColor( Color( 255, 255, 255, 255 ) );
	}

	// are we close to dying?
	/*if ( m_iBossHealthPercentageByte < m_nMaxHealth * m_flHealthDeathWarning )
	{
		if ( m_pHealthImage )
		{
			m_pHealthImage->SetFgColor( m_clrHealthDeathWarningColor );
		}
	}*/

	// set our health display value
	/*if ( nPrevHealth != m_iBossHealthPercentageByte )
	{
		if ( m_iBossHealthPercentageByte > 0 )
		{
			SetDialogVariable( "Health", m_iBossHealthPercentageByte );
		}
		else
		{
			SetDialogVariable( "Health", "" );
		}

		// are we lower than max?
		if ( m_iBossHealthPercentageByte < m_nMaxHealth )
		{
			SetDialogVariable( "MaxHealth", m_nMaxHealth );
		}
		else
		{
			SetDialogVariable( "MaxHealth", "" );
		}
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::OnThink()
{
	if ( m_flNextThink < gpGlobals->curtime )
	{
		m_iBossIndex = event->GetInt( "entindex" );
		C_BaseEntity *pBoss =  ClientEntityList().GetBaseEntity( m_iBossIndex );
		C_AI_BaseNPC *pNPC = dynamic_cast<C_AI_BaseNPC *>( pBoss );

		if ( pNPC )
		{
			SetHealth( pNPC->GetHealth(), pNPC->GetMaxHealth() );
		}

		m_flNextThink = gpGlobals->curtime + 0.05f;
	}
}

void CHudBossHealthMeter::FireGameEvent( IGameEvent * event )
{
	const char *pEventName = event->GetName();

	if ( Q_strcmp( "antlionguard_spawned", pEventName ) == 0 )
	{
		if ( !m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( true );
		}
	}
	else if ( Q_strcmp( "antlionguard_killed", pEventName ) == 0 )
	{
		if ( m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( false );
		}
	}
	else if ( Q_strcmp( "helicopter_spawned", pEventName ) == 0 )
	{
		if ( !m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( true );
		}
	}
	else if ( Q_strcmp( "helicopter_destoryed", pEventName ) == 0 )
	{
		if ( m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( false );
		}
	}
	else if ( Q_strcmp( "gunship_spawned", pEventName ) == 0 )
	{
		if ( !m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( true );
		}
	}
	else if ( Q_strcmp( "gunship_destoryed", pEventName ) == 0 )
	{
		if ( m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( false );
		}
	}
	else if ( Q_strcmp( "strider_spawned", pEventName ) == 0 )
	{
		if ( !m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( true );
		}
	}
	else if ( Q_strcmp( "strider_killed", pEventName ) == 0 )
	{
		if ( m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( false );
		}
	}
}