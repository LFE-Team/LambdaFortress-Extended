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

extern int UI_IsDebug();

// Tip portraits
const char* g_pszTipsClassImages[] = 
{
	"class_portraits/all_class",
	"class_portraits/scout",
	"class_portraits/sniper",
	"class_portraits/soldier",
	"class_portraits/demoman",
	"class_portraits/medic",
	"class_portraits/heavy",
	"class_portraits/pyro",
	"class_portraits/spy",
	"class_portraits/engineer",
};

CTFLoadingProgress *g_pTFLoadingProgress = NULL;

ConVar lfe_ui_loadingprogress_statsummary( "lfe_ui_loadingprogress_statsummary", "0", FCVAR_ARCHIVE, "Show stats summary on loading screen" );
ConVar lfe_ui_loadingprogress_tips( "lfe_ui_loadingprogress_tips", "1", FCVAR_ARCHIVE, "Show stats summary on loading screen" );

CON_COMMAND( lfe_ui_reload_loadingprogress, "Reload Loading Progress")
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

	m_pTipImage = new CTFImagePanel( this, "TipImage" );
	m_pTipText = new vgui::Label( this, "TipText", "" );

	m_pNextTipButton = new vgui::Button( this, "NextTipButton", "" );	
	m_pCloseButton = new vgui::Button( this, "CloseButton", "" );	

	m_pMapInfo = dynamic_cast<EditablePanel *>( FindChildByName( "MapInfo" ) );

	m_pStatSummary = new CTFStatsSummaryPanel( this, "CTFStatsSummaryPanel" );

	ListenForGameEvent( "server_spawn" );
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
	}
	else if ( 0 == Q_stricmp( command, "nexttip" ) )
	{
		UpdateTip();
	}

	BaseClass::OnCommand( command );
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
	vgui::ImagePanel *pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName( "MainBackground" ) );
	if ( pImagePanel )
	{
		// determine if we're in widescreen or not and select the appropriate image
		int screenWide, screenTall;
		surface()->GetScreenSize( screenWide, screenTall );
		float aspectRatio = (float)screenWide/(float)screenTall;
		bool bIsWidescreen = aspectRatio >= 1.6f;

		char szBGName[128];
		char szImage[128];

		// get mainmenu background
		engine->GetMainMenuBackgroundName(szBGName, sizeof(szBGName));
		Q_snprintf(szImage, sizeof(szImage), "../console/%s", szBGName);
		if (bIsWidescreen)
			Q_strcat(szImage, "_widescreen", sizeof(szImage));

		pImagePanel->SetImage( szImage );
	}

	UpdateDialog();
	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFLoadingProgress::ClearMapLabel()
{
	SetDialogVariable( "maplabel", "" );
	SetDialogVariable( "maptype", "" );
	SetDialogVariable( "mapauthor", "" );

	CExLabel *pMapAuthorLabel = dynamic_cast<CExLabel *>( FindChildByName("MapAuthors") );
	if ( pMapAuthorLabel && pMapAuthorLabel->IsVisible() )
		pMapAuthorLabel->SetVisible( false );

	CExLabel *pLabel = dynamic_cast<CExLabel *>( FindChildByName( "OnYourWayLabel" ) );
	if ( pLabel && pLabel->IsVisible() )
		pLabel->SetVisible( false );

	CExLabel *pLabelShadow = dynamic_cast<CExLabel *>( FindChildByName( "OnYourWayLabelShadow" ) );
	if ( pLabelShadow && pLabelShadow->IsVisible() )
		pLabelShadow->SetVisible( false );

	vgui::ImagePanel *pMapBackground = dynamic_cast<ImagePanel *>( FindChildByName( "MapBackground" ) );
	if ( pMapBackground && pMapBackground->IsVisible() )
		pMapBackground->SetVisible( false );

	if ( m_pMapInfo && m_pMapInfo->IsVisible() )
		m_pMapInfo->SetVisible( false );

	vgui::ImagePanel *pStampBackground = dynamic_cast<ImagePanel *>( FindChildByName( "Background" ) );
	if ( pStampBackground && pStampBackground->IsVisible() )
		pStampBackground->SetVisible( false );

	vgui::ImagePanel *pMapImage = dynamic_cast<ImagePanel *>( FindChildByName( "MapImage" ) );
	if ( pMapImage && pMapImage->IsVisible() )
		pMapImage->SetVisible( false );
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

	m_pStatSummary->UpdateDialog();
}

//-----------------------------------------------------------------------------
// Purpose: Updates the tip
//-----------------------------------------------------------------------------
void CTFLoadingProgress::UpdateTip()
{
	int iClass = TF_CLASS_UNDEFINED;
	SetDialogVariable( "tiptext", g_TFTips.GetRandomTip( iClass ) );

	if ( m_pTipImage )
	{
		m_pTipImage->SetImage( g_pszTipsClassImages[iClass] );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Shows or hides controls
//-----------------------------------------------------------------------------
void CTFLoadingProgress::UpdateControls()
{
	// show or hide controls depending on what mode we're in
	//bool bShowPlayerData = ( m_bInteractive || m_iTotalSpawns > 0 );

	m_pTipText->SetVisible( lfe_ui_loadingprogress_tips.GetBool() );
	m_pTipImage->SetVisible( lfe_ui_loadingprogress_tips.GetBool() );

	m_pNextTipButton->SetVisible( m_bInteractive && lfe_ui_loadingprogress_tips.GetBool() );
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
extern const char *GetMapType( const char *mapName );
extern const char *GetMapAuthor( const char *mapName );

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
				if ( Q_stristr( pMapName, "background" ) )
				{
					ClearMapLabel();
				}
				else
				{
					// set the map name in the UI
					wchar_t wzMapName[255] = L"";
					g_pVGuiLocalize->ConvertANSIToUnicode( GetMapDisplayName( pMapName ), wzMapName, sizeof( wzMapName ) );

					SetDialogVariable( "maplabel", wzMapName );

					// set the map type in the UI
					const char *szMapType = GetMapType( pMapName );
					SetDialogVariable( "maptype", g_pVGuiLocalize->Find( szMapType ) );

					if ( m_pMapInfo && m_pMapInfo->IsVisible() )
						m_pMapInfo->SetVisible( true );

					// set the map author name in the UI
					const char *szMapAuthor = GetMapAuthor( pMapName );
					if ( szMapAuthor[0] != '\0' )
					{
						SetDialogVariable( "mapauthor", szMapAuthor );

						CExLabel *pMapAuthorLabel = dynamic_cast<CExLabel *>( FindChildByName( "MapAuthors" ) );
						if ( pMapAuthorLabel && !pMapAuthorLabel->IsVisible() )
							pMapAuthorLabel->SetVisible( false );
					}

					CExLabel *pLabel = dynamic_cast<CExLabel *>( FindChildByName( "OnYourWayLabel" ) );
					if ( pLabel && !pLabel->IsVisible() )
						pLabel->SetVisible( true );

					CExLabel *pLabelShadow = dynamic_cast<CExLabel *>( FindChildByName( "OnYourWayLabelShadow" ) );
					if ( pLabelShadow && !pLabelShadow->IsVisible() )
						pLabelShadow->SetVisible( true );

					// set the background image
					vgui::ImagePanel *pMapBackground = dynamic_cast<ImagePanel *>( FindChildByName( "MapBackground" ) );

					// determine if we're in widescreen or not and select the appropriate image
					int screenWide, screenTall;
					surface()->GetScreenSize( screenWide, screenTall );
					float aspectRatio = (float)screenWide/(float)screenTall;
					bool bIsWidescreen = aspectRatio >= 1.6f;

					char szMapBackground[ MAX_PATH ];
					Q_snprintf( szMapBackground, sizeof( szMapBackground ),  bIsWidescreen ? "VGUI/maps/menu_loading_%s_widescreen" : "VGUI/maps/menu_loading_%s", pMapName );
					Q_strlower( szMapBackground );
					IMaterial *pMapBGMaterial = materials->FindMaterial( szMapBackground, TEXTURE_GROUP_VGUI, false );
					if ( pMapBGMaterial && !IsErrorMaterial( pMapBGMaterial ) )
					{
						if ( !pMapBackground->IsVisible() )
							pMapBackground->SetVisible( true );

						// take off the vgui/ at the beginning when we set the image
						Q_snprintf( szMapBackground, sizeof( szMapBackground ), bIsWidescreen ? "maps/menu_loading_%s_widescreen" : "maps/menu_loading_%s", pMapName );
						Q_strlower( szMapBackground );

						pMapBackground->SetImage( szMapBackground );
					}
					else
					{
						if ( pMapBackground->IsVisible() )
							pMapBackground->SetVisible( false );

						vgui::ImagePanel *pStampBackground = dynamic_cast<ImagePanel *>( FindChildByName( "Background" ) );
						if ( pStampBackground && !pStampBackground->IsVisible() )
							pStampBackground->SetVisible( true );
					}

					// set the map photos
					vgui::ImagePanel *pMapImage = dynamic_cast<ImagePanel *>( FindChildByName( "MapImage" ) );

					char szMapImage[ MAX_PATH ];
					Q_snprintf( szMapImage, sizeof( szMapImage ),  "VGUI/maps/menu_photos_%s", pMapName );
					Q_strlower( szMapImage );
					IMaterial *pMapMaterial = materials->FindMaterial( szMapImage, TEXTURE_GROUP_VGUI, false );
					if ( pMapMaterial && !IsErrorMaterial( pMapMaterial ) )
					{
						if ( !pMapImage->IsVisible() )
							pMapImage->SetVisible( true );

						// take off the vgui/ at the beginning when we set the image
						Q_snprintf( szMapImage, sizeof( szMapImage ), "maps/menu_photos_%s", pMapName );
						Q_strlower( szMapImage );

						pMapImage->SetImage( szMapImage );
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
	if ( UI_IsDebug() )
		Msg( "[GAMEUI] MAIN_MENU -> LOADING_MENU\n");

	ClearMapLabel();

	UpdateDialog();

	m_pStatSummary->SetVisible( lfe_ui_loadingprogress_statsummary.GetBool() );

	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.4f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
}

//-----------------------------------------------------------------------------
// Purpose: Called when we are deactivated at end of level load
//-----------------------------------------------------------------------------
void CTFLoadingProgress::OnDeactivate()
{
	if ( UI_IsDebug() )
		Msg( "[GAMEUI] LOADING_MENU -> PAUSE_MENU\n");

	ClearMapLabel();
	m_pStatSummary->SetVisible( false );
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.4f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

CON_COMMAND( lfe_ui_show_loadingprogress, "Shows Loading Screen" )
{
	GLoadingProgress()->ShowModal();
}