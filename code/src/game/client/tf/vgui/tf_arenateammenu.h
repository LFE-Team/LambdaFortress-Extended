//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef TF_ARENATEAMMENU_H
#define TF_ARENATEAMMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_controls.h"
#include <teammenu.h>
#include "tf_teammenu.h"

class CTFTeamButton;

//-----------------------------------------------------------------------------
// Purpose: Displays the arena team menu
//-----------------------------------------------------------------------------
class CTFArenaTeamMenu : public CTeamMenu
{
private:
	DECLARE_CLASS_SIMPLE( CTFArenaTeamMenu, CTeamMenu );

public:
	CTFArenaTeamMenu( IViewPort *pViewPort );
	~CTFArenaTeamMenu();

	virtual const char *GetName(void) { return PANEL_ARENATEAMSELECT; }
	void Update();
	void ShowPanel(bool bShow);

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

	// command callbacks
	virtual void OnCommand(const char *command);

	virtual void LoadMapPage(const char *mapName);

	virtual void OnTick(void);

private:

	CTFTeamButton	*m_pAutoTeamButton;
	CTFTeamButton	*m_pSpecTeamButton;
	CExLabel		*m_pSpecLabel;
	CExButton		*m_pCancelButton;


private:
	enum { NUM_TEAMS = 3 };

	ButtonCode_t m_iTeamMenuKey;
};

#endif // TF_ARENATEAMMENU_H
