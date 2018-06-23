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

#endif	// TF_HUD_BOSS_HEALTH_H
