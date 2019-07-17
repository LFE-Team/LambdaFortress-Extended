//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//
#ifndef TF_HUD_SQUADSTATUS_H
#define TF_HUD_SQUADSTATUS_H

#include "hudelement.h"
#include "vgui_controls/Panel.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/TextEntry.h>
#include "tf_controls.h"

//-----------------------------------------------------------------------------
// Purpose: Shows the squad status
//-----------------------------------------------------------------------------
class CTFHudSquadStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudSquadStatus, vgui::EditablePanel );

public:
	CTFHudSquadStatus( const char *pElementName );
	virtual void Init( void );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset( void );
	virtual void OnThink( void );
	bool ShouldDraw();

	void MsgFunc_SquadMemberDied(bf_read &msg);

	virtual void Paint();

private:
	CExLabel		*m_pSquadStatusLabel;
	CTFImagePanel	*m_pSquadStatusBG;

	CPanelAnimationVar( vgui::HFont, m_hIconFont, "IconFont", "HudNumbers" );
	CPanelAnimationVarAliasType( float, m_flIconInsetX, "IconInsetX", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flIconInsetY, "IconInsetY", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flIconGap, "IconGap", "20", "proportional_float" );

	CPanelAnimationVar( Color, m_SquadIconColor, "SquadIconColor", "255 220 0 160" );
	CPanelAnimationVar( Color, m_LastMemberColor, "LastMemberColor", "255 220 0 0" );
	
	int m_iSquadMembers;
	int m_iSquadMedics;
	bool m_bSquadMembersFollowing;
	bool m_bSquadMemberAdded;
	bool m_bSquadMemberJustDied;
};

#endif //TF_HUD_SQUADSTATUS_H