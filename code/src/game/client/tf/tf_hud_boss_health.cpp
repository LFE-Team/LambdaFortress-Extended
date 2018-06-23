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

#include "tf_hud_betastamp.h"
#include "tf_gamerules.h"
#include "tf_hud_freezepanel.h"
#include "tf_mainmenu.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudBossHealthMeter );
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudBossHealthMeter::CHudBossHealthMeter( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudBossHealth" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
}

void CHudBossHealthMeter::PerformLayout( void )
{
	BaseClass::PerformLayout();



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::OnThink()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudBossHealthMeter::ShouldDraw( void )
{
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBossHealthMeter::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/HudBossHealth.res" );
}