//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CREATEMULTIPLAYERGAME_H
#define CREATEMULTIPLAYERGAMEGAMEPLAYPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_dialogpanelbase.h"
#include "controls/tf_scriptobject.h"

class CDescription;
class mpcontrol_t;

//-----------------------------------------------------------------------------
// Purpose: Holds all the game option pages
//-----------------------------------------------------------------------------
class CTFCreateMultiplayerGame : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFCreateMultiplayerGame, CTFDialogPanelBase);
	friend class CTFCreateMultiplayerGameServerPanel;
public:
	CTFCreateMultiplayerGame(vgui::Panel *parent, const char *panelName);
	~CTFCreateMultiplayerGame();
	bool Init();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void Run();
	virtual void Activate();
	virtual void OnCommand(const char *command);
	virtual void Show();
	virtual void Hide();
	MESSAGE_FUNC(OnCancelPressed, "CancelPressed");
	MESSAGE_FUNC(OnOkPressed, "OkPressed");
	MESSAGE_FUNC(OnApplyPressed, "OnApplyPressed");	
	MESSAGE_FUNC(OnDefaultPressed, "OnDefaultPressed");
	MESSAGE_FUNC(OnGameUIHidden, "GameUIHidden");	// called when the GameUI is hidden

	CTFCreateMultiplayerGameServerPanel *m_pGameplayPage;
private:

	/*
	class CTFCreateMultiplayerGameServerPanel		*m_pOptionsSubMultiplayer;
	*/
	// for loading/saving game config
	KeyValues *m_pSavedData;
};

struct CreateServerData_t;

//-----------------------------------------------------------------------------
// Purpose: class for loading/saving server config file
//-----------------------------------------------------------------------------
class CServerDescription : public CDescription
{
public:
	CServerDescription( CPanelListPanel *panel );

	void WriteScriptHeader( FileHandle_t fp );
	void WriteFileHeader( FileHandle_t fp ); 
};

//-----------------------------------------------------------------------------
// Purpose: Displays a game-specific list of options
//-----------------------------------------------------------------------------
class CTFCreateMultiplayerGameServerPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFCreateMultiplayerGameServerPanel, CTFDialogPanelBase);

public:
	CTFCreateMultiplayerGameServerPanel(vgui::Panel *parent, const char *panelName);
	~CTFCreateMultiplayerGameServerPanel();
	bool Init();
	void OnCommand(const char *command);
	void OnKeyCodeTyped(vgui::KeyCode code);
	void OnApplyChanges();

	void SetMap(const char *name);
	bool IsRandomMapSelected();
	const char *GetMapName();

	// returns currently entered information about the server
	int GetMaxPlayers();
	const char *GetPassword();
	const char *GetHostName();
	const char *GetValue(const char *cvarName, const char *defaultValue) const;
	void LoadMaps(const char *pszPathID) const;
protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void CreateControls();
	void DestroyControls();
	void GatherCurrentValues();
	void SaveValues();
	void LoadMapList();

	CServerDescription *m_pDescription;

	mpcontrol_t *m_pList;

	vgui::ComboBox *m_pMapList;

	enum { DATA_STR_LENGTH = 64 };
	char m_szMapName[DATA_STR_LENGTH];
};

#endif // CREATEMULTIPLAYERGAMEGAMEPLAYPAGE_H