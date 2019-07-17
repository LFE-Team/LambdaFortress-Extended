//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "lfe_createmultiplayergamedialog.h"
#include "tf_mainmenu.h"

#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/QueryBox.h"
#include "controls/tf_advbutton.h"

#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"

#include "KeyValues.h"

#include "lfe_createmultiplayergameserverpage.h"
#include "lfe_createmultiplayergamegameplaypage.h"

//#include "EngineInterface.h"
#include "modinfo.h"
//#include "GameUI_Interface.h"

#include <stdio.h>

using namespace vgui;

#include "filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGameDialog::CTFCreateMultiplayerGameDialog(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName) // "CreateMultiplayerGameDialog"
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGameDialog::~CTFCreateMultiplayerGameDialog()
{
	if (m_pSavedData)
	{
		m_pSavedData->deleteThis();
		m_pSavedData = NULL;
	}
}

bool CTFCreateMultiplayerGameDialog::Init()
{
	BaseClass::Init();

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);

	m_pPanels.SetSize(SERVER_PANEL_COUNT);
	AddPanel(new CTFCreateMultiplayerGameServerPage(this, "ServerPage"), SERVER_PANEL_SERVER);
	AddPanel(new CTFCreateMultiplayerGameGameplayPage(this, "GameplayPage"), SERVER_PANEL_GAMEPLAY);
	m_pOptionsCurrent = SERVER_PANEL_SERVER;

	// create KeyValues object to load/save config options
	m_pSavedData = new KeyValues( "ServerConfig" );

	// load the config data
	if (m_pSavedData)
	{
		m_pSavedData->LoadFromFile( g_pFullFileSystem, "ServerConfig.vdf", "GAME" ); // this is game-specific data, so it should live in GAME, not CONFIG

		const char *startMap = m_pSavedData->GetString("map", "");
		if (startMap[0])
		{
			GetPanel(SERVER_PANEL_SERVER)->SetMap(startMap);
		}
	}

	return true;
}

void CTFCreateMultiplayerGameDialog::AddPanel(CTFDialogPanelBase *m_pPanel, int iPanel)
{
	m_pPanels[iPanel] = m_pPanel;
	m_pPanel->SetEmbedded(true);
	m_pPanel->Hide();
}

void CTFCreateMultiplayerGameDialog::SetCurrentPanel(ServerPanel pCurrentPanel)
{
	if (m_pOptionsCurrent == pCurrentPanel)
		return;
	GetPanel(m_pOptionsCurrent)->Hide();
	GetPanel(pCurrentPanel)->Show();
	m_pOptionsCurrent = pCurrentPanel;

	//dynamic_cast<CTFAdvButton *>(FindChildByName("Defaults"))->SetVisible((pCurrentPanel == PANEL_KEYBOARD));
}

CTFDialogPanelBase*	CTFCreateMultiplayerGameDialog::GetPanel(int iPanel)
{
	return m_pPanels[iPanel];
}

void CTFCreateMultiplayerGameDialog::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/CreateMultiplayerGameDialog.res");
	//dynamic_cast<CTFAdvButton *>(FindChildByName("Defaults"))->SetVisible(false);
}

void CTFCreateMultiplayerGameDialog::Show()
{
	BaseClass::Show();
	GetPanel(m_pOptionsCurrent)->Show();

	for (int i = 0; i < SERVER_PANEL_COUNT; i++)
		GetPanel(i)->OnCreateControls();
};

void CTFCreateMultiplayerGameDialog::Hide()
{
	BaseClass::Hide();
	GetPanel(m_pOptionsCurrent)->Hide();

	for (int i = 0; i < SERVER_PANEL_COUNT; i++)
		GetPanel(i)->OnDestroyControls();
};

void CTFCreateMultiplayerGameDialog::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		OnCancelPressed();
	}
	else if (!stricmp(command, "Ok"))
	{
		OnOkPressed();
	}
	else if (!stricmp(command, "Apply"))
	{
		OnApplyPressed();
	}
	else if (!Q_strcmp(command, "newcreateserver"))
	{
		SetCurrentPanel(SERVER_PANEL_SERVER);
	}
	else if (!Q_strcmp(command, "newcreateservergameplay"))
	{
		SetCurrentPanel(SERVER_PANEL_GAMEPLAY);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::OnOkPressed()
{
	// reset server enforced cvars
	g_pCVar->RevertFlaggedConVars( FCVAR_REPLICATED );	

	// Cheats were disabled; revert all cheat cvars to their default values.
	// This must be done heading into multiplayer games because people can play
	// demos etc and set cheat cvars with sv_cheats 0.
	g_pCVar->RevertFlaggedConVars( FCVAR_CHEAT );

	DevMsg( "FCVAR_CHEAT cvars reverted to defaults.\n" );

	// get these values from m_pServerPage and store them temporarily

	char szMapName[64], szHostName[64], szPassword[64];
	Q_strncpy(szMapName, GetPanel(SERVER_PANEL_SERVER)->GetMapName(), sizeof( szMapName ));
	Q_strncpy(szHostName, GetPanel(SERVER_PANEL_GAMEPLAY)->GetHostName(), sizeof( szHostName ));
	Q_strncpy(szPassword, GetPanel(SERVER_PANEL_GAMEPLAY)->GetPassword(), sizeof( szPassword ));

	// save the config data
	if (m_pSavedData)
	{
		if (GetPanel(SERVER_PANEL_SERVER)->IsRandomMapSelected())
		{
			// it's set to random map, just save an
			m_pSavedData->SetString("map", "");
		}
		else
		{
			m_pSavedData->SetString("map", szMapName);
		}

		// save config to a file
		m_pSavedData->SaveToFile( g_pFullFileSystem, "ServerConfig.vdf", "GAME" );
	}

	char szMapCommand[1024];

	// create the command to execute
	Q_snprintf(szMapCommand, sizeof( szMapCommand ), "disconnect\nwait\nwait\nsetmaster enable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\nprogress_enable\nmap %s\n",
		GetPanel(SERVER_PANEL_GAMEPLAY)->GetMaxPlayers(),
		szPassword,
		szHostName,
		szMapName
	);

	//char conReq[2];
	//conReq[0] = 'C';
	//conReq[1] = 'T';
	//steamapicontext->SteamNetworking()->SendP2PPacket( steamapicontext->SteamUser()->GetSteamID(), &conReq, 2, k_EP2PSendReliable );

	// exec
	engine->ClientCmd_Unrestricted(szMapCommand);

	OnApplyPressed();
	Hide();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::OnApplyPressed()
{
	for (int i = 0; i < SERVER_PANEL_COUNT; i++)
		GetPanel(i)->OnApplyChanges();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::OnCancelPressed()
{
	for (int i = 0; i < SERVER_PANEL_COUNT; i++)
		GetPanel(i)->OnResetData();

	Hide();
	MAINMENU_ROOT->ShowPanel( CURRENT_MENU );
}

//-----------------------------------------------------------------------------
// Purpose: Brings the dialog to the fore
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::Activate()
{
	//BaseClass::Activate();
	//EnableApplyButton(false);
}

//-----------------------------------------------------------------------------
// Purpose: Opens the dialog
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::Run()
{
	//SetTitle("#GameUI_Options", true);
	//Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Called when the GameUI is hidden
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameDialog::OnGameUIHidden()
{
	// tell our children about it
	for (int i = 0; i < GetChildCount(); i++)
	{
		Panel *pChild = GetChild(i);
		if (pChild)
		{
			PostMessage(pChild, new KeyValues("GameUIHidden"));
		}
	}
}

void CTFCreateMultiplayerGameDialog::OnKeyCodeTyped( vgui::KeyCode code )
{
	// force ourselves to be closed if the escape key it pressed
	if ( code == KEY_ESCAPE )
	{
		Hide();
		MAINMENU_ROOT->ShowPanel( CURRENT_MENU );
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}