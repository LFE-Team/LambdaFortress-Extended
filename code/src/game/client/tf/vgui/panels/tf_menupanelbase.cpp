#include "cbase.h"
#include "tf_menupanelbase.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "controls/lfe_flyoutmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMenuPanelBase::CTFMenuPanelBase(vgui::Panel* parent, const char *panelName) : EditablePanel(NULL, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(true);
	SetVisible(true);
	Init();
	
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMenuPanelBase::~CTFMenuPanelBase()
{

}

bool CTFMenuPanelBase::Init()
{
	int x, y;
	int width, height;
	//surface()->GetScreenSize(width, height);
	GetParent()->GetBounds(x, y, width, height);
	SetSize(width, height);
	SetPos(0, 0);
	bInGame = false;
	bInMenu = false;
	bInGameLayout = false;
	bShowSingle = false;
	return true;
}

void CTFMenuPanelBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMenuPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
};

void CTFMenuPanelBase::OnCommand(const char* command)
{
	bool bOpeningFlyout = false;

	const char *pchCommand = command;

	// does this command match a flyout menu?
	FlyoutMenu *flyout = dynamic_cast< FlyoutMenu* >( FindChildByName( pchCommand ) );
	if ( flyout )
	{
		bOpeningFlyout = true;

		// If so, enumerate the buttons on the menu and find the button that issues this command.
		// (No other way to determine which button got pressed; no notion of "current" button on PC.)
		for ( int iChild = 0; iChild < GetChildCount(); iChild++ )
		{
			CTFAdvButton *pTFButton = dynamic_cast<CTFAdvButton *>( GetChild( iChild ) );
			if ( pTFButton && pTFButton->GetCommandString() && !Q_strcmp( pTFButton->GetCommandString(), command ) )
			{
				pTFButton->NavigateFrom();
				// open the menu next to the button that got clicked
				flyout->OpenMenu( pTFButton );
				flyout->SetListener( this );
				break;
			}
		}
	}
	else
	{
		engine->ExecuteClientCmd(command);
	}

	if( !bOpeningFlyout )
	{
		FlyoutMenu::CloseActiveMenu(); //due to unpredictability of mouse navigation over keyboard, we should just close any flyouts that may still be open anywhere.
	}
}

void CTFMenuPanelBase::OnTick()
{
	BaseClass::OnTick();
};

void CTFMenuPanelBase::OnThink()
{
	BaseClass::OnThink();
};

void CTFMenuPanelBase::SetShowSingle(bool ShowSingle)
{
	bShowSingle = ShowSingle;
}

void CTFMenuPanelBase::Show()
{
	SetVisible(true);
};

void CTFMenuPanelBase::Hide()
{
	SetVisible(false);
};

void CTFMenuPanelBase::DefaultLayout()
{
	bInGameLayout = false;
	if (bInMenu || bInGame)
		(bInMenu ? Show() : Hide());
	//SetVisible(bInMenu);
};


void CTFMenuPanelBase::GameLayout()
{
	bInGameLayout = true;
	if (bInMenu || bInGame)
		(bInGame ? Show() : Hide());
	//SetVisible(bInGame);
};

void CTFMenuPanelBase::PaintBackground()
{
	SetPaintBackgroundType(0);
	BaseClass::PaintBackground();
}

bool CTFMenuPanelBase::InGame()
{
	return MAINMENU_ROOT->InGame();
}


CTFMenuPanelBase* CTFMenuPanelBase::GetMenuPanel(int iPanel)
{
	return MAINMENU_ROOT->GetMenuPanel(iPanel);
}