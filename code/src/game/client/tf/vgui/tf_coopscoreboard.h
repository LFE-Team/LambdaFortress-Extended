//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_COOPSCOREBOARD_H
#define TF_COOPSCOREBOARD_H
#ifdef _WIN32
#pragma once
#endif

#include "hud.h"
#include "hudelement.h"
#include "tf_hud_playerstatus.h"
#include "clientscoreboarddialog.h"
#include "tf_clientscoreboard.h"

//-----------------------------------------------------------------------------
// Purpose: displays the MapInfo menu
//-----------------------------------------------------------------------------

class CTFCoOpScoreBoardDialog : public CClientScoreBoardDialog
{
private:
	DECLARE_CLASS_SIMPLE(CTFCoOpScoreBoardDialog, CClientScoreBoardDialog);

public:
	CTFCoOpScoreBoardDialog(IViewPort *pViewPort);
	virtual ~CTFCoOpScoreBoardDialog();

	virtual const char *GetName(void) { return PANEL_COOPSCOREBOARD; }
	virtual void Reset();
	virtual void Update();
	virtual void ShowPanel( bool bShow );

protected:
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void PostApplySchemeSettings( vgui::IScheme *pScheme ) {};

private:
	void InitPlayerList( vgui::SectionedListPanel *pPlayerList );
	void SetPlayerListImages( vgui::SectionedListPanel *pPlayerList );
	void UpdateTeamInfo();
	void UpdatePlayerList();
	void UpdateSpectatorList();
	void UpdatePlayerDetails();
	void ClearPlayerDetails();
	bool ShouldShowAsSpectator( int iPlayerIndex );
	bool ShouldShowAsUnassigned( int iPlayerIndex );

	virtual void FireGameEvent( IGameEvent *event );

	static bool TFPlayerSortFunc( vgui::SectionedListPanel *list, int itemID1, int itemID2 );

	vgui::SectionedListPanel *GetSelectedPlayerList( void );

	vgui::SectionedListPanel	*m_pPlayerListBlue;
	vgui::SectionedListPanel	*m_pPlayerListRed;
	CExLabel					*m_pLabelPlayerName;
	CExLabel					*m_pLabelMapName;
	vgui::ImagePanel			*m_pImagePanelHorizLine;
	CTFClassImage				*m_pClassImage;
	EditablePanel				*m_pLocalPlayerStatsPanel;
	EditablePanel				*m_pLocalPlayerDuelStatsPanel;
	CExLabel					*m_pSpectatorsInQueue;
	CExLabel					*m_pServerTimeLeftValue;
	vgui::HFont					m_hTimeLeftFont;
	vgui::HFont					m_hTimeLeftNotSetFont;

	int							m_iImageDead;
	int							m_iImageDominated;
	int							m_iImageNemesis;
	int							m_iClassEmblem[TF_CLASS_COUNT_ALL];
	int							m_iClassEmblemDead[TF_CLASS_COUNT_ALL];
	
	CPanelAnimationVarAliasType( int, m_iStatusWidth, "status_width", "12", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iNemesisWidth, "nemesis_width", "20", "proportional_int" );
};

const wchar_t *GetPointsString( int iPoints );

#endif // TF_COOPSCOREBOARD_H
