//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_PLAYERSTATUS_H
#define TF_HUD_PLAYERSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/ImagePanel.h>
#include "tf_controls.h"
#include "tf_imagepanel.h"
#include "GameEventListener.h"

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFClassImage : public vgui::ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFClassImage, vgui::ImagePanel );

	CTFClassImage( vgui::Panel *parent, const char *name ) : ImagePanel( parent, name )
	{
	}

	void SetClass( int iTeam, int iClass, int iCloakstate );
};

//-----------------------------------------------------------------------------
// Purpose:  Displays player class data
//-----------------------------------------------------------------------------
class CTFHudPlayerClass : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerClass, EditablePanel );

public:

	CTFHudPlayerClass( Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

public: // IGameEventListener Interface
	virtual void FireGameEvent( IGameEvent * event );

protected:

	virtual void OnThink();

private:

	float				m_flNextThink;

	CTFClassImage		*m_pClassImage;
	CTFImagePanel		*m_pClassImageBG;
	CTFImagePanel		*m_pSpyImage; // used when spies are disguised
	CTFImagePanel		*m_pSpyOutlineImage;
	CTFImagePanel		*m_pClassModelPanelBG;

	int					m_nTeam;
	int					m_nClass;
	int					m_nDisguiseTeam;
	int					m_nDisguiseClass;
	int					m_nCloakLevel;
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
class CTFHudPlayerHealth : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerHealth, EditablePanel );

public:

	CTFHudPlayerHealth( Panel *parent, const char *name );

	virtual const char *GetResFilename( void ) { return "resource/UI/HudPlayerHealth.res"; }
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

	void	SetHealth( int iNewHealth, int iMaxHealth, int iMaxBuffedHealth );
	void	HideHealthBonusImage( void );

protected:

	virtual void OnThink();

protected:
	float				m_flNextThink;

private:
	CTFHealthPanel		*m_pHealthImage;
	vgui::ImagePanel	*m_pHealthBonusImage;
	vgui::ImagePanel	*m_pHealthImageBG;
	vgui::ImagePanel	*m_pHealthImageBuildingBG;

	vgui::ImagePanel	*m_pHealthBleedImage;
	vgui::ImagePanel	*m_pHealthHookBleedImage;
	vgui::ImagePanel	*m_pHealthMilkImage;
	vgui::ImagePanel	*m_pHealthGasImage;
	vgui::ImagePanel	*m_pHealthMiniCritImage;
	vgui::ImagePanel	*m_pHealthMiniCritSlientImage;
	vgui::ImagePanel	*m_pHealthMedicBulletResist;
	vgui::ImagePanel	*m_pHealthMedicBlastResist;
	vgui::ImagePanel	*m_pHealthMedicFireResist;
	vgui::ImagePanel	*m_pHealthMedicSmallBulletResist;
	vgui::ImagePanel	*m_pHealthMedicSmallBlastResist;
	vgui::ImagePanel	*m_pHealthMedicSmallFireResist;
	vgui::ImagePanel	*m_pHealthWheelOfDoom;
	vgui::ImagePanel	*m_pHealthSoldierOffenseBuff;
	vgui::ImagePanel	*m_pHealthSoldierDefenseBuff;
	vgui::ImagePanel	*m_pHealthSoldierHealOnHitBuff;
	vgui::ImagePanel	*m_pHealthSpyMarked;
	vgui::ImagePanel	*m_pHealthParachute;
	vgui::ImagePanel	*m_pHealthRuneStrength;
	vgui::ImagePanel	*m_pHealthRuneHaste;
	vgui::ImagePanel	*m_pHealthRuneRegen;
	vgui::ImagePanel	*m_pHealthRuneResist;
	vgui::ImagePanel	*m_pHealthRuneVampire;
	vgui::ImagePanel	*m_pHealthRuneReflect;
	vgui::ImagePanel	*m_pHealthRunePrecision;
	vgui::ImagePanel	*m_pHealthRuneAgility;
	vgui::ImagePanel	*m_pHealthRuneKnockout;
	vgui::ImagePanel	*m_pHealthRuneKing;
	vgui::ImagePanel	*m_pHealthRunePlague;
	vgui::ImagePanel	*m_pHealthRuneSupernova;
	vgui::ImagePanel	*m_pHealthSlowed;

	int					m_nHealth;
	int					m_nMaxHealth;

	int					m_nBonusHealthOrigX;
	int					m_nBonusHealthOrigY;
	int					m_nBonusHealthOrigW;
	int					m_nBonusHealthOrigH;

	CPanelAnimationVar( int, m_nHealthBonusPosAdj, "HealthBonusPosAdj", "25" );
	CPanelAnimationVar( float, m_flHealthDeathWarning, "HealthDeathWarning", "0.49" );
	CPanelAnimationVar( Color, m_clrHealthDeathWarningColor, "HealthDeathWarningColor", "HUDDeathWarning" );
};

//-----------------------------------------------------------------------------
// Purpose:  Parent panel for the player class/health displays
//-----------------------------------------------------------------------------
class CTFHudPlayerStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudPlayerStatus, vgui::EditablePanel );

public:
	CTFHudPlayerStatus( const char *pElementName );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void Reset();

private:

	CTFHudPlayerClass	*m_pHudPlayerClass;
	CTFHudPlayerHealth	*m_pHudPlayerHealth;
};

#endif	// TF_HUD_PLAYERSTATUS_H