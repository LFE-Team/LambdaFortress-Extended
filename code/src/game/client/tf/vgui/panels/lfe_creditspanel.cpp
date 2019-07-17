#include "cbase.h"
#include "lfe_creditspanel.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advslider.h"
#include "vgui_controls/SectionedListPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_notificationmanager.h"
#include "engine/IEngineSound.h"
#include "vgui_avatarimage.h"
#include "tf_gamerules.h"
#include <KeyValues.h>
#include "tier0/icommandline.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// LF:E Credits panel
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFCreditsPanel::CTFCreditsPanel( vgui::Panel* parent, const char *panelName ) : CTFDialogPanelBase( parent, panelName )
{
	m_pText = new CExRichText( this, "CreditText" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCreditsPanel::~CTFCreditsPanel()
{

}

void CTFCreditsPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/CreditsPanel.res");
}

void CTFCreditsPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand( command );
}


void CTFCreditsPanel::Show()
{
	BaseClass::Show();
	MAINMENU_ROOT->HidePanel( MAIN_MENU );
	MAINMENU_ROOT->HidePanel( NOTIFICATION_MENU );
	MAINMENU_ROOT->ShowPanel( SHADEBACKGROUND_MENU );
};

void CTFCreditsPanel::Hide()
{
	BaseClass::Hide();
	MAINMENU_ROOT->ShowPanel( MAIN_MENU );
	MAINMENU_ROOT->HidePanel( SHADEBACKGROUND_MENU );
};