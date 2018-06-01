//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include <time.h>
#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/QueryBox.h"
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include <vgui/IInput.h>
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"

#include "KeyValues.h"
#include "lf_createmultiplayergame.h"
#include "tf_mainmenu.h"
#include "tf_menupanelbase.h"
#include "controls/tf_advslider.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_advcheckbutton.h"
#include "controls/tf_advbutton.h"
#include "filesystem.h"

#include <tier0/vcrmode.h>

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define OPTIONS_DIR "cfg"
#define DEFAULT_OPTIONS_FILE OPTIONS_DIR "/settings_default.scr"
#define OPTIONS_FILE OPTIONS_DIR "/settings.scr"
#define RANDOM_MAP "#GameUI_RandomMap"

//-----------------------------------------------------------------------------
// Purpose: Basic help dialog
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGame::CTFCreateMultiplayerGame(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGame::~CTFCreateMultiplayerGame()
{
	if (m_pSavedData)
	{
		m_pSavedData->deleteThis();
		m_pSavedData = NULL;
	}
}


bool CTFCreateMultiplayerGame::Init()
{
	BaseClass::Init();

	// create KeyValues object to load/save config options
	m_pSavedData = new KeyValues( "ServerConfig" );

	// load the config data
	if (m_pSavedData)
	{
		m_pSavedData->LoadFromFile( g_pFullFileSystem, "ServerConfig.vdf", "GAME" ); // this is game-specific data, so it should live in GAME, not CONFIG

		const char *startMap = m_pSavedData->GetString("map", "");
		if (startMap[0])
		{
			m_pGameplayPage->SetMap(startMap);
		}
	}
	return true;
}

void CTFCreateMultiplayerGame::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/CreateServer.res");
	dynamic_cast<CTFAdvButton *>(FindChildByName("Defaults"))->SetVisible(false);
	dynamic_cast<CTFAdvButton *>(FindChildByName("Defaults"))->SetVisible(m_pGameplayPage);
}

void CTFCreateMultiplayerGame::Show()
{
	BaseClass::Show();
};

void CTFCreateMultiplayerGame::Hide()
{
	BaseClass::Hide();
};

void CTFCreateMultiplayerGame::OnCommand(const char* command)
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
	else if (!stricmp(command, "DefaultsOK"))
	{
		OnDefaultPressed();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::OnOkPressed()
{
	OnApplyPressed();
	Hide();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::OnApplyPressed()
{
	// get these values from m_pServerPage and store them temporarily
	char szMapName[64], szHostName[64], szPassword[64];
	Q_strncpy(szMapName, m_pGameplayPage->GetMapName(), sizeof(szMapName));
	Q_strncpy(szHostName, m_pGameplayPage->GetHostName(), sizeof(szHostName));
	Q_strncpy(szPassword, m_pGameplayPage->GetPassword(), sizeof(szPassword));

	// save the config data
	if (m_pSavedData)
	{
		if (m_pGameplayPage->IsRandomMapSelected())
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
	Q_snprintf(szMapCommand, sizeof( szMapCommand ), "disconnect\nwait\nwait\nsv_lan 1\nsetmaster enable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\nprogress_enable\nmap %s\n",
		m_pGameplayPage->GetMaxPlayers(),
		szPassword,
		szHostName,
		szMapName
	);

	// exec
	engine->ClientCmd_Unrestricted(szMapCommand);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::OnCancelPressed()
{
	Hide();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::OnDefaultPressed()
{
}

//-----------------------------------------------------------------------------
// Purpose: Brings the dialog to the fore
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::Activate()
{
	//BaseClass::Activate();
	//EnableApplyButton(false);
}

//-----------------------------------------------------------------------------
// Purpose: Opens the dialog
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::Run()
{
	//SetTitle("#GameUI_Options", true);
	//Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Called when the GameUI is hidden
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGame::OnGameUIHidden()
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

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGameServerPanel::CTFCreateMultiplayerGameServerPanel(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFCreateMultiplayerGameServerPanel::~CTFCreateMultiplayerGameServerPanel()
{
	delete m_pDescription;
}

bool CTFCreateMultiplayerGameServerPanel::Init()
{
	BaseClass::Init();

	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	m_pList = NULL;

	m_pDescription = new CServerDescription(m_pListPanel);
	m_pDescription->InitFromFile( DEFAULT_OPTIONS_FILE );
	m_pDescription->InitFromFile( OPTIONS_FILE );
	m_pDescription->TransferCurrentValues(NULL);

	// we can use this if we decide we want to put "listen server" at the end of the game name
	m_pMapList = new ComboBox(this, "MapList", 12, false);

	LoadMapList();
	m_szMapName[0]  = 0;

	return true;
}

void CTFCreateMultiplayerGameServerPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/CreateServerServerPanel.res");
}

void CTFCreateMultiplayerGameServerPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFCreateMultiplayerGameServerPanel::OnKeyCodeTyped(KeyCode code)
{
	// force ourselves to be closed if the escape key it pressed
	if (code == KEY_ESCAPE)
	{
		Hide();
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::GatherCurrentValues()
{
	if (!m_pDescription)
		return;

	// OK
	CTFAdvCheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;
	CTFAdvSlider *pScroll;

	mpcontrol_t *pList;

	CScriptObject *pObj;
	CScriptListItem *pItem;

	char szValue[256];

	pList = m_pList;
	while (pList)
	{
		pObj = pList->pScrObj;

		if (!pList->pControl)
		{
			pObj->SetCurValue(pObj->curValue);
			pList = pList->next;
			continue;
		}

		switch (pObj->type)
		{
		case O_BOOL:
			pBox = (CTFAdvCheckButton *)pList->pControl;
			sprintf(szValue, "%d", pBox->IsSelected() ? 1 : 0);
			break;
		case O_NUMBER:
			pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText( szValue, sizeof( szValue ) );
			break;
		case O_SLIDER:
			pScroll = (CTFAdvSlider *)pList->pControl;
			V_strncpy( szValue, pScroll->GetFinalValue(), sizeof( szValue ) );
			break;
		case O_STRING:
			pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText(szValue, sizeof(szValue));
			break;
		case O_CATEGORY:
			break;
		case O_LIST:
			pCombo = (ComboBox *)pList->pControl;
			pCombo->GetText( szValue, sizeof( szValue ) );
			int activeItem = pCombo->GetActiveItem();

			pItem = pObj->pListItems;
			//			int n = (int)pObj->fcurValue;

			while (pItem)
			{
				if (!activeItem--)
					break;

				pItem = pItem->pNext;
			}

			if (pItem)
			{
				sprintf(szValue, "%s", pItem->szValue);
			}
			else  // Couln't find index
			{
				//assert(!("Couldn't find string in list, using default value"));
				sprintf(szValue, "%s", pObj->curValue);
			}
			break;
		}

		// Remove double quotes and % characters
		UTIL_StripInvalidCharacters(szValue, sizeof(szValue));

		pObj->SetCurValue( szValue );

		pList = pList->next;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::CreateControls()
{
	BaseClass::CreateControls();

	// Go through desciption creating controls
	CScriptObject *pObj;
	pObj = m_pDescription->pObjList;
	
	mpcontrol_t	*pCtrl;
	CTFAdvCheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;
	CTFAdvSlider *pScroll;
	Label *pTitle;
	CScriptListItem *pListItem;
	
	HFont hFont = GETSCHEME()->GetFont( m_pListPanel->GetFontString(), true );

	Panel *objParent = m_pListPanel;
	while (pObj)
	{
		//Msg("\nAdded: %s %s %f %f %i\n", pObj->prompt, pObj->cvarname, pObj->fcurValue, pObj->fcurValue, pObj->type);
		
		if (pObj->type == O_OBSOLETE)
		{
			pObj = pObj->pNext;
			continue;
		}

		pCtrl = new mpcontrol_t(objParent, "mpcontrol_t");
		pCtrl->type = pObj->type;
	
		switch (pCtrl->type)
		{
		case O_BOOL:
			pBox = new CTFAdvCheckButton(pCtrl, "DescCheckButton", pObj->prompt);
			pBox->MakeReadyForUse();

			pBox->SetSelected(pObj->fcurValue != 0.0f ? true : false);
			pBox->SetCommandString(pObj->cvarname);
			pBox->GetButton()->SetFont( hFont );

			pBox->SetToolTip( pObj->tooltip );

			pCtrl->pControl = pBox;
			break;
		case O_STRING:
		case O_NUMBER:
			pEdit = new TextEntry(pCtrl, "DescTextEntry");
			pEdit->MakeReadyForUse();

			pEdit->InsertString(pObj->curValue);

			pCtrl->pControl = pEdit;
			break;
		case O_SLIDER:
			pScroll = new CTFAdvSlider(pCtrl, "DescScrollEntry", pObj->prompt);
			pScroll->MakeReadyForUse();

			pScroll->SetCommandString( pObj->cvarname );
			pScroll->SetValue( pObj->fcurValue );
			pScroll->SetMinMax( pObj->fMin, pObj->fMax );
			pScroll->SetFont( hFont );
			pScroll->SetToolTip( pObj->tooltip );

			pCtrl->pControl = pScroll;
			break;
		case O_LIST:
			pCombo = new ComboBox(pCtrl, "DescComboBox", 5, false);
			pCombo->MakeReadyForUse();
			pCombo->SetFont( hFont );

			pListItem = pObj->pListItems;
			while (pListItem)
			{
				pCombo->AddItem(pListItem->szItemText, NULL);
				pListItem = pListItem->pNext;
			}

			pCombo->ActivateItemByRow((int)pObj->fcurValue);

			pCtrl->pControl = pCombo;
			break;
		case O_CATEGORY:
			pTitle = new Label( pCtrl, "DescTextTitle", pObj->prompt );
			pTitle->MakeReadyForUse();

			pTitle->SetBorder( GETSCHEME()->GetBorder( "AdvSettingsTitleBorder" ) );
			pTitle->SetFont( GETSCHEME()->GetFont( "MenuSmallFont", true ) );
			pTitle->SetFgColor( GETSCHEME()->GetColor( DEFAULT_COLOR, COLOR_WHITE ) );

			pCtrl->pControl = pTitle;
			break;
		default:
			break;
		}

		if (pCtrl->type != O_BOOL && pCtrl->type != O_SLIDER && pCtrl->type != O_CATEGORY)
		{
			pCtrl->pPrompt = new Label(pCtrl, "DescLabel", "");
			pCtrl->pPrompt->MakeReadyForUse();

			pCtrl->pPrompt->SetFont( hFont );
			pCtrl->pPrompt->SetContentAlignment(vgui::Label::a_west);
			pCtrl->pPrompt->SetTextInset(5, 0);
			pCtrl->pPrompt->SetText(pObj->prompt);
		}

		pCtrl->pScrObj = pObj;
		int h = m_pListPanel->GetTall() / 13.0; //(float)GetParent()->GetTall() / 15.0;
		pCtrl->SetSize(800, h);
		//pCtrl->SetBorder( scheme()->GetBorder(1, "DepressedButtonBorder") );
		m_pListPanel->AddItem(pCtrl);

		// Link it in
		if (!m_pList)
		{
			m_pList = pCtrl;
			pCtrl->next = NULL;
		}
		else
		{
			mpcontrol_t *p;
			p = m_pList;
			while (p)
			{
				if (!p->next)
				{
					p->next = pCtrl;
					pCtrl->next = NULL;
					break;
				}
				p = p->next;
			}
		}
		
		pObj = pObj->pNext;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::DestroyControls()
{
	BaseClass::DestroyControls();
	m_pList = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::SaveValues()
{
	// Get the values from the controls:
	GatherCurrentValues();

	// Create the game.cfg file
	if (m_pDescription)
	{
		FileHandle_t fp;

		// Add settings to config.cfg
		m_pDescription->WriteToConfig();

		g_pFullFileSystem->CreateDirHierarchy(OPTIONS_DIR);
		fp = g_pFullFileSystem->Open(OPTIONS_FILE, "wb");
		if (fp)
		{
			m_pDescription->WriteToScriptFile(fp);
			g_pFullFileSystem->Close(fp);
		}
	}
}

void CTFCreateMultiplayerGameServerPanel::OnApplyChanges()
{
	BaseClass::OnApplyChanges();
	KeyValues *kv = m_pMapList->GetActiveItemUserData();
	Q_strncpy(m_szMapName, kv->GetString("mapname", ""), DATA_STR_LENGTH);
	SaveValues();
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::LoadMaps(const char *pszPathID) const
{
	FileFindHandle_t findHandle = NULL;

	//KeyValues *hiddenMaps = ModInfo().GetHiddenMaps();

	const char *pszFilename = g_pFullFileSystem->FindFirst( "maps/*.bsp", &findHandle );
	while ( pszFilename )
	{
		char mapname[256];

		// FindFirst ignores the pszPathID, so check it here
		// TODO: this doesn't find maps in fallback dirs
		Q_snprintf( mapname, sizeof(mapname), "maps/%s", pszFilename );
		if ( !g_pFullFileSystem->FileExists( mapname, pszPathID ) )
		{
			goto nextFile;
		}

		// remove the text 'maps/' and '.bsp' from the file name to get the map name
		
		const char *str = Q_strstr( pszFilename, "maps" );
		if ( str )
		{
			Q_strncpy( mapname, str + 5, sizeof(mapname) - 1 );	// maps + \\ = 5
		}
		else
		{
			Q_strncpy( mapname, pszFilename, sizeof(mapname) - 1 );
		}
		char *ext = Q_strstr( mapname, ".bsp" );
		if ( ext )
		{
			*ext = 0;
		}

		//!! hack: strip out single player HL maps
		// this needs to be specified in a seperate file
		/*
		if ( !stricmp( ModInfo().GetGameName(), "Half-Life" ) && ( mapname[0] == 'c' || mapname[0] == 't') && mapname[2] == 'a' && mapname[1] >= '0' && mapname[1] <= '5' )
		{
			goto nextFile;
		}
		*/
		/*
		// strip out maps that shouldn't be displayed
		if ( hiddenMaps )
		{
			if ( hiddenMaps->GetInt( mapname, 0 ) )
			{
				goto nextFile;
			}
		}
		*/
		// add to the map list
		m_pMapList->AddItem( mapname, new KeyValues( "data", "mapname", mapname ) );

		// get the next file
	nextFile:
		pszFilename = g_pFullFileSystem->FindNext( findHandle );
	}
	g_pFullFileSystem->FindClose( findHandle );
}



//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();

	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem( RANDOM_MAP, new KeyValues( "data", "mapname", RANDOM_MAP ) );

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	const char *pathID = "MOD";
	/*
	if ( !stricmp(ModInfo().GetGameName(), "Half-Life" ) ) 
	{
		pathID = NULL; // hl is the base dir
	}
	*/
	// Load the GameDir maps
	LoadMaps( pathID ); 

	// If we're not the Valve directory and we're using a "fallback_dir" in gameinfo.txt then include those maps...
	// (pathID is NULL if we're "Half-Life")
	/*
	const char *pszFallback = ModInfo().GetFallbackDir();
	if ( pathID && pszFallback[0] )
	{
		LoadMaps( "GAME_FALLBACK" );
	}
	*/
	// set the first item to be selected
	m_pMapList->ActivateItem( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFCreateMultiplayerGameServerPanel::IsRandomMapSelected()
{
	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!stricmp( mapname, RANDOM_MAP ))
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFCreateMultiplayerGameServerPanel::GetMapName()
{
	int count = m_pMapList->GetItemCount();

	// if there is only one entry it's the special "select random map" entry
	if( count <= 1 )
		return NULL;

	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!strcmp( mapname, RANDOM_MAP ))
	{
		int which = RandomInt( 1, count - 1 );
		mapname = m_pMapList->GetItemUserData( which )->GetString("mapname");
	}

	return mapname;
}

//-----------------------------------------------------------------------------
// Purpose: Sets currently selected map in the map combobox
//-----------------------------------------------------------------------------
void CTFCreateMultiplayerGameServerPanel::SetMap(const char *mapName)
{
	/*
	for (int i = 0; i < m_pMapList->GetItemCount(); i++)
	{
		
		if (!m_pMapList->IsItemIDValid(i))
			continue;
		
		if (!stricmp(m_pMapList->GetItemUserData(i)->GetString("mapname"), mapName))
		{
			m_pMapList->MakeReadyForUse();
			//break;
		}
	}
	*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFCreateMultiplayerGameServerPanel::GetMaxPlayers()
{
	return atoi(GetValue("maxplayers", "32"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFCreateMultiplayerGameServerPanel::GetPassword()
{
	return GetValue("sv_password", "");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFCreateMultiplayerGameServerPanel::GetHostName()
{
	return GetValue("hostname", "Lambda-Fortress");	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFCreateMultiplayerGameServerPanel::GetValue(const char *cvarName, const char *defaultValue) const
{
	for (mpcontrol_t *mp = m_pList; mp != NULL; mp = mp->next)
	{
		Panel *control = mp->pControl;
		if (control && !Q_stricmp(mp->GetName(), cvarName))
		{
			KeyValues *data = new KeyValues("GetText");
			static char buf[128];
			if (control && control->RequestInfo(data))
			{
				Q_strncpy(buf, data->GetString("text", defaultValue), sizeof(buf) - 1);
			}
			else
			{
				// no value found, copy in default text
				Q_strncpy(buf, defaultValue, sizeof(buf) - 1);
			}

			// ensure null termination of string
			buf[sizeof(buf) - 1] = 0;

			// free
			data->deleteThis();
			return buf;
		}

	}

	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor, load/save client settings object
//-----------------------------------------------------------------------------
CServerDescription::CServerDescription(CPanelListPanel *panel)
	: CDescription(panel)
{
	setHint( "// NOTE:  THIS FILE IS AUTOMATICALLY REGENERATED, \r\n"
			 "//DO NOT EDIT THIS HEADER, YOUR COMMENTS WILL BE LOST IF YOU DO\r\n"
			 "// Multiplayer options script\r\n"
			 "//\r\n"
			 "// Format:\r\n"
			 "//  Version [float]\r\n"
			 "//  Options description followed by \r\n"
			 "//  Options defaults\r\n"
			 "//\r\n"
			 "// Option description syntax:\r\n"
			 "//\r\n"
			 "//  \"cvar\" { \"Prompt\" { type [ type info ] } { default } }\r\n"
			 "//\r\n"
			 "//  type = \r\n"
			 "//   BOOL   (a yes/no toggle)\r\n"
			 "//   STRING\r\n"
			 "//   NUMBER\r\n"
			 "//   LIST\r\n"
			 "//\r\n"
			 "// type info:\r\n"
			 "// BOOL                 no type info\r\n"
			 "// NUMBER       min max range, use -1 -1 for no limits\r\n"
			 "// STRING       no type info\r\n"
			 "// LIST         "" delimited list of options value pairs\r\n"
			 "//\r\n"
			 "//\r\n"
			 "// default depends on type\r\n"
			 "// BOOL is \"0\" or \"1\"\r\n"
			 "// NUMBER is \"value\"\r\n"
			 "// STRING is \"value\"\r\n"
			 "// LIST is \"index\", where index \"0\" is the first element of the list\r\n\r\n\r\n" );

	setDescription("SERVER_OPTIONS");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CServerDescription::WriteScriptHeader(FileHandle_t fp)
{
	char am_pm[] = "AM";
	tm newtime;
	VCRHook_LocalTime(&newtime);

	g_pFullFileSystem->FPrintf(fp, (char *)getHint());

	// Write out the comment and Cvar Info:
	g_pFullFileSystem->FPrintf( fp, "// Source Engine Server Configuration Layout Script (stores last settings chosen, too)\r\n" );
	g_pFullFileSystem->FPrintf( fp, "// File generated:  %.19s %s\r\n", asctime( &newtime ), am_pm );
	g_pFullFileSystem->FPrintf( fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n" );
	g_pFullFileSystem->FPrintf( fp, "VERSION %.1f\r\n\r\n", SCRIPT_VERSION );
	g_pFullFileSystem->FPrintf( fp, "DESCRIPTION SERVER_OPTIONS\r\n{\r\n" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CServerDescription::WriteFileHeader(FileHandle_t fp)
{
	char am_pm[] = "AM";
	tm newtime;
	VCRHook_LocalTime(&newtime);

	g_pFullFileSystem->FPrintf( fp, "// Source Engine Server Configuration Settings\r\n" );
	g_pFullFileSystem->FPrintf( fp, "// DO NOT EDIT, GENERATED BY HALF-LIFE\r\n" );
	g_pFullFileSystem->FPrintf( fp, "// File generated:  %.19s %s\r\n", asctime( &newtime ), am_pm );
	g_pFullFileSystem->FPrintf( fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n" );
}