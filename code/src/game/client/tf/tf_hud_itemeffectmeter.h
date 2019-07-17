//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_ITEMEFFECTMETER_H
#define TF_HUD_ITEMEFFECTMETER_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ProgressBar.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include "iclientmode.h"
#include "ienginevgui.h"
#include "tf_controls.h"
#include "c_tf_player.h"
#include "hud.h"
#include "hudelement.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudItemEffectMeter : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudItemEffectMeter, vgui::EditablePanel );

public:
	CHudItemEffectMeter( Panel *pParent, const char *pElementName );

	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	PerformLayout( void );
	virtual void	LevelInit( void );

	virtual void	UpdateStatus( void );

	int				GetSlot( void ) { return m_iSlot; }
	void			SetSlot( int iSlot ) { m_iSlot = iSlot; }
	void			SetWeapon( C_TFWeaponBase *pWeapon );

	const char		*GetBeepSound( void ) { return "TFPlayer.Recharged"; }
private:
	vgui::ContinuousProgressBar *m_pEffectMeter;
	CExLabel *m_pEffectMeterLabel;

	bool m_bPlayingAnim;
	int m_iSlot;
	CHandle<C_TFWeaponBase> m_hWeapon;
	float m_flOldCharge;
};

class CHudItemEffects : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudItemEffects, vgui::EditablePanel );

public:
	CHudItemEffects( const char *pElementName );
	~CHudItemEffects();

	virtual void PerformLayout( void );
	virtual bool ShouldDraw( void );
	virtual void OnTick( void );
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

private:
	CUtlVector<CHudItemEffectMeter *> m_pEffectBars;
	
	CPanelAnimationVarAliasType( int, m_iXOffset, "x_offset", "50", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iYOffset, "y_offset", "0", "proportional_int" );
};

#endif	// TF_HUD_ITEMEFFECTMETER_H
