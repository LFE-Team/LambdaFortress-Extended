//========= Copyright Š 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef CREATEMULTIPLAYERGAMEDIALOG_H
#define CREATEMULTIPLAYERGAMEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_dialogpanelbase.h"
#include <vgui/KeyCode.h>

enum ServerPanel
{
	SERVER_PANEL_SERVER,
	SERVER_PANEL_GAMEPLAY,

	SERVER_PANEL_COUNT
};

class CTFCreateMultiplayerGameServerPage;
class CTFCreateMultiplayerGameGameplayPage;

//-----------------------------------------------------------------------------
// Purpose: dialog for launching a listenserver
//-----------------------------------------------------------------------------
class CTFCreateMultiplayerGameDialog : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE( CTFCreateMultiplayerGameDialog, CTFDialogPanelBase );

public:
	CTFCreateMultiplayerGameDialog(vgui::Panel *parent, const char *panelName);
	~CTFCreateMultiplayerGameDialog();

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
	//MESSAGE_FUNC(OnDefaultPressed, "OnDefaultPressed");
	MESSAGE_FUNC(OnGameUIHidden, "GameUIHidden");	// called when the GameUI is hidden

	virtual void OnKeyCodeTyped(vgui::KeyCode code);
private:
	void SetCurrentPanel(ServerPanel pCurrentPanel);
	void AddPanel(CTFDialogPanelBase *m_pPanel, int iPanel);
	CTFDialogPanelBase*				GetPanel(int iPanel);
	CUtlVector<CTFDialogPanelBase*>	m_pPanels;
	ServerPanel							m_pOptionsCurrent;

	/*CTFCreateMultiplayerGameServerPage *m_pServerPage;
	CTFCreateMultiplayerGameGameplayPage *m_pGameplayPage;*/

	// for loading/saving game config
	KeyValues *m_pSavedData;
};


#endif // CREATEMULTIPLAYERGAMEDIALOG_H