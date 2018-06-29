//===================== 2018 ISPuddy reversed engineering =====================//
// Reverse Engie Training Annotations
// An in-world location-specific information bubble HUD.
//=============================================================================//

#ifndef TF_HUD_ANNOTATIONSPANEL_H
#define TF_HUD_ANNOTATIONSPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include <game/client/iviewport.h>
#include <vgui/IScheme.h>
#include "hud.h"
#include "hudelement.h"
#include "tf_imagepanel.h"
#include "tf_hud_playerstatus.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAnnotationsPanelCallout : public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFAnnotationsPanelCallout, EditablePanel );
public:
	CTFAnnotationsPanelCallout( Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	void	UpdateText( void );

private:
	vgui::Label	*m_pDisplayTextLabel;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAnnotationsPanel : public EditablePanel, public CHudElement
{
private:
	DECLARE_CLASS_SIMPLE( CTFAnnotationsPanel, EditablePanel );

public:
	CTFAnnotationsPanel( const char *pElementName );

	virtual void Reset();
	virtual void Init();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void FireGameEvent( IGameEvent * event );

	void UpdateCallout( void );
	void ShowCalloutsIn( float flTime );
	void Show();
	void Hide();
	virtual bool ShouldDraw( void );
	void OnThink( void );

protected:
	CTFAnnotationsPanelCallout *TestAndAddCallout( Vector &origin, Vector &vMins, Vector &vMaxs, CUtlVector<Vector> *vecCalloutsTL, 
		CUtlVector<Vector> *vecCalloutsBR, Vector &vecFreezeTL, Vector &vecFreezeBR, Vector &vecStatTL, Vector &vecStatBR, int *iX, int *iY );

private:

	CUtlVector<CTFAnnotationsPanelCallout*>	m_pCalloutPanels;
	float					m_flShowCalloutsAt;
};

#endif // TF_HUD_ANNOTATIONSPANEL_H
