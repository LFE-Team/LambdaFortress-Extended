//============== Copyright LFE-TEAM Not All rights reserved. ==================
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/QueryBox.h>
#include <vgui_controls/ProgressBar.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/AnimationController.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include "ienginevgui.h"
#include <game/client/iviewport.h>
#include "tf_tips.h"

#include "lfe_loadingprogress.h"
#include <convar.h>
#include "fmtstr.h"

using namespace vgui;


CTFLoadingProgress *g_pTFLoadingProgress = NULL;

CON_COMMAND( lfe_hud_reload_loadingprogress, "Reload Loading Progress")
{
	g_pTFLoadingProgress->InvalidateLayout( true, true );
}

//-----------------------------------------------------------------------------
// Purpose: Returns the global loading progress panel
//-----------------------------------------------------------------------------
CTFLoadingProgress *GLoadingProgress()
{
	if ( NULL == g_pTFLoadingProgress )
	{
		g_pTFLoadingProgress = new CTFLoadingProgress();
	}
	return g_pTFLoadingProgress;
}

//-----------------------------------------------------------------------------
// Purpose: Destroys the global loading progress panel
//-----------------------------------------------------------------------------
void DestroyLoadingProgress()
{
	if ( NULL != g_pTFLoadingProgress )
	{
		delete g_pTFLoadingProgress;
		g_pTFLoadingProgress = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFLoadingProgress::CTFLoadingProgress() : vgui::EditablePanel( NULL, "TFLoadingProgress", 
	vgui::scheme()->LoadSchemeFromFile( "Resource/ClientScheme.res", "ClientScheme" ) )
{
	m_bControlsLoaded = false;
	m_bInteractive = false;

	m_pTipLabel = new vgui::Label( this, "TipLabel", "" );
	m_pTipText = new vgui::Label( this, "TipText", "" );

	m_pNextTipButton = new vgui::Button( this, "NextTipButton", "" );	
	m_pCloseButton = new vgui::Button( this, "CloseButton", "" );	

	ListenForGameEvent( "server_spawn" );

	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Shows this dialog as a modal dialog
//-----------------------------------------------------------------------------
void CTFLoadingProgress::ShowModal()
{
	// we are in interactive mode, enable controls
	m_bInteractive = true;

	SetParent( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );
	UpdateDialog();
	SetVisible( true );
	MoveToFront();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLoadingProgress::PerformLayout()
{
	BaseClass::PerformLayout();

	if ( m_pTipLabel && m_pTipText )
	{
		m_pTipLabel->SizeToContents();
		int width = m_pTipLabel->GetWide();

		int x, y, w, t;
		m_pTipText->GetBounds( x, y, w, t );
		m_pTipText->SetBounds( x + width, y, w - width, t );
		m_pTipText->InvalidateLayout( false, true ); // have it re-layout the contents so it's wrapped correctly now that we've changed the size
	}

	if ( m_pNextTipButton )
	{
		m_pNextTipButton->SizeToContents();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Command handler
//-----------------------------------------------------------------------------
void CTFLoadingProgress::OnCommand( const char *command )
{
	if ( 0 == Q_stricmp( command, "vguicancel" ) )
	{
		m_bInteractive = false;
		UpdateDialog();
		SetVisible( false );
		SetParent( (VPANEL) NULL );
		SetDefaultSelections();
	}
	else if ( 0 == Q_stricmp( command, "nexttip" ) )
	{
		UpdateTip();
	}

	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: Resets the dialog
//-----------------------------------------------------------------------------
void CTFLoadingProgress::Reset()
{
	SetDefaultSelections();
}

//-----------------------------------------------------------------------------
// Purpose: Sets all user-controllable dialog settings to default values
//-----------------------------------------------------------------------------
void CTFLoadingProgress::SetDefaultSelections()
{

}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void CTFLoadingProgress::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetProportional( true );
	LoadControlSettings( "Resource/UI/LoadingProgress.res" );
	m_bControlsLoaded = true;

	// Set the PRE-Map-Load Background image
	if ( IsPC() )
	{
		ImagePanel *pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "MainBackground" ) );
		if ( pImagePanel )
		{
			// determine if we're in widescreen or not and select the appropriate image
			int screenWide, screenTall;
			surface()->GetScreenSize( screenWide, screenTall );
			float aspectRatio = (float)screenWide/(float)screenTall;
			bool bIsWidescreen = aspectRatio >= 1.6f;

			pImagePanel->SetImage( bIsWidescreen ? "../console/background01_widescreen" : "../console/background01" );
		}
	}

	SetDefaultSelections();
	UpdateDialog();
	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFLoadingProgress::ClearMapLabel()
{
	SetDialogVariable( "maplabel", "" );

	vgui::Label *pLabel = dynamic_cast<Label *>( FindChildByName( "OnYourWayLabel" ) );
	if ( pLabel && pLabel->IsVisible() )
	{
		pLabel->SetVisible( false );
	}

	vgui::Label *pLabelShadow = dynamic_cast<Label *>( FindChildByName( "OnYourWayLabelShadow" ) );
	if ( pLabelShadow && pLabelShadow->IsVisible() )
	{
		pLabelShadow->SetVisible( false );
	}

	vgui::ImagePanel *pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "MapBackground" ) );
	if ( pImagePanel && pImagePanel->IsVisible() )
	{
		pImagePanel->SetVisible( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFLoadingProgress::UpdateDialog()
{
	RandomSeed( Plat_MSTime() );

	ClearMapLabel();

	// update the tip
	UpdateTip();
	// show or hide controls depending on if we're interactive or not
	UpdateControls();		
}

//-----------------------------------------------------------------------------
// Purpose: Updates the tip
//-----------------------------------------------------------------------------
void CTFLoadingProgress::UpdateTip()
{
	SetDialogVariable( "tiptext", g_TFTips.GetRandomTip() );
}

//-----------------------------------------------------------------------------
// Purpose: Shows or hides controls
//-----------------------------------------------------------------------------
void CTFLoadingProgress::UpdateControls()
{
	// show or hide controls depending on what mode we're in
	bool bShowPlayerData = ( m_bInteractive || m_iTotalSpawns > 0 );

	m_pTipText->SetVisible( bShowPlayerData );
	m_pTipLabel->SetVisible( bShowPlayerData );

	m_pNextTipButton->SetVisible( m_bInteractive );
	m_pCloseButton->SetVisible( m_bInteractive );
}
//-----------------------------------------------------------------------------
// Purpose: Calculates a fraction and guards from divide by 0.  (Returns 0 if 
//			denominator is 0.)
//-----------------------------------------------------------------------------
float CTFLoadingProgress::SafeCalcFraction( float flNumerator, float flDemoninator )
{
	if ( 0 == flDemoninator )
		return 0;
	return flNumerator / flDemoninator;
}

extern const char *GetMapDisplayName( const char *mapName );

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFLoadingProgress::FireGameEvent( IGameEvent *event )
{
	const char *pEventName = event->GetName();

	// when we are changing levels and 
	if ( 0 == Q_strcmp( pEventName, "server_spawn" ) )
	{
		if ( !m_bInteractive )
		{
			const char *pMapName = event->GetString( "mapname" );
			if ( pMapName )
			{
				// If we're loading a background map, don't display anything
				// HACK: Client doesn't get gpGlobals->eLoadType, so just do string compare for now.
				if ( Q_stristr( pMapName, "background") )
				{
					ClearMapLabel();
				}
				else
				{
					// set the map name in the UI
					wchar_t wzMapName[255]=L"";
					g_pVGuiLocalize->ConvertANSIToUnicode( GetMapDisplayName( pMapName ), wzMapName, sizeof( wzMapName ) );

					SetDialogVariable( "maplabel", wzMapName );

					vgui::Label *pLabel = dynamic_cast<Label *>( FindChildByName( "OnYourWayLabel" ) );
					if ( pLabel && !pLabel->IsVisible() )
					{
						pLabel->SetVisible( true );
					}

					vgui::Label *pLabelShadow = dynamic_cast<Label *>( FindChildByName( "OnYourWayLabelShadow" ) );
					if ( pLabelShadow && !pLabelShadow->IsVisible() )
					{
						pLabelShadow->SetVisible( true );
					}
					
					// set the background image
					vgui::ImagePanel *pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "MapBackground" ) );

					char szMapImage[ MAX_PATH ];
					Q_snprintf( szMapImage, sizeof( szMapImage ), "VGUI/maps/menu_loading_%s", pMapName );
					Q_strlower( szMapImage );
					IMaterial *pMapMaterial = materials->FindMaterial( szMapImage, TEXTURE_GROUP_VGUI, false );
					if ( pMapMaterial && !IsErrorMaterial( pMapMaterial ) )
					{
						if ( !pImagePanel->IsVisible() )
						{
							pImagePanel->SetVisible( true );
						}

						// take off the vgui/ at the beginning when we set the image
						Q_snprintf( szMapImage, sizeof( szMapImage ), "maps/menu_loading_%s", pMapName );
						Q_strlower( szMapImage );

						pImagePanel->SetImage( szMapImage );
					}
				}
			}	
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when we are activated during level load
//-----------------------------------------------------------------------------
void CTFLoadingProgress::OnActivate()
{
	ClearMapLabel();

	UpdateDialog();

	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.4f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
}

//-----------------------------------------------------------------------------
// Purpose: Called when we are deactivated at end of level load
//-----------------------------------------------------------------------------
void CTFLoadingProgress::OnDeactivate()
{
	ClearMapLabel();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.4f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}