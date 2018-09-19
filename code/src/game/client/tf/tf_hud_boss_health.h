//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_BOSS_HEALTH_H
#define TF_HUD_BOSS_HEALTH_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_controls.h"
#include "tf_imagepanel.h"
#include "GameEventListener.h"


//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CHudBossHealthMeter : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudBossHealthMeter, vgui::EditablePanel );

public:

	CHudBossHealthMeter( const char *pElementName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout( void );
	virtual bool ShouldDraw( void );
	void OnThink( void );
};

//-----------------------------------------------------------------------------
// Purpose:  Clips the health image to the appropriate percentage
//-----------------------------------------------------------------------------
class CTFHealthPanel : public vgui::Panel
{
public:
	DECLARE_CLASS_SIMPLE( CTFHealthPanel, vgui::Panel );

	CTFHealthPanel( vgui::Panel *parent, const char *name );
	virtual void Paint();
	void SetHealth( float flHealth ){ m_flHealth = ( flHealth <= 1.0 ) ? flHealth : 1.0f; }

private:

	float	m_flHealth; // percentage from 0.0 -> 1.0
	int		m_iMaterialIndex;
	int		m_iDeadMaterialIndex;
};

//-----------------------------------------------------------------------------
// Purpose:  Displays player health data
//-----------------------------------------------------------------------------
class CHudBossHealthMeter : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudBossHealthMeter, EditablePanel );

public:

	CHudBossHealthMeter( Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void FireGameEvent( IGameEvent * event );
	virtual void Reset();

	void	SetHealth( int iNewHealth, int iMaxHealth );
	void	HideHealthBonusImage( void );

protected:

	virtual void OnThink();

protected:
	float				m_flNextThink;

private:
	CTFHealthPanel		*m_pHealthImage;
	vgui::ImagePanel	*m_pHealthBonusImage;
	vgui::ImagePanel	*m_pHealthImageBG;

	int					m_iBossHealthPercentageByte;
	//int					m_iBossStunPercentageByte;
	int					m_nMaxHealth;

	//int					m_iSkillShotCompleteCount;
	//int					m_fSkillShotComboEndTime;
	//int					m_iBossState;
};

#endif	// TF_HUD_BOSS_HEALTH_H
