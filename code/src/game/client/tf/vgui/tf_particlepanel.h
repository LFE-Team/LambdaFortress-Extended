//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_PARTICLEPANEL_H
#define TF_PARTICLEPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui/KeyCode.h>
#include <KeyValues.h>
#include <vgui/IVGui.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CParticlePanelInfo
{
public:
	CParticlePanelInfo()
	{
		m_pszParticleName = NULL;
		m_vecOriginOffset.Init();
	}

	~CParticlePanelInfo()
	{
		if ( m_pszParticleName && m_pszParticleName[0] )
		{
			delete [] m_pszParticleName;
			m_pszParticleName = NULL;
		}
	}

public:
	enum 
	{
		kMAXCONTROLPOINTSPANEL = 63 
	};

	const char	*m_pszParticleName;
	const char	*m_pszControlPointNames[kMAXCONTROLPOINTSPANEL];
	bool		m_bStartActive;
	Vector		m_vecOriginOffset;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFParticlePanel : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFParticlePanel, vgui::EditablePanel );

	CTFParticlePanel( Panel *parent, const char *panelName );
	virtual ~CTFParticlePanel();

	virtual void	ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void	ApplySettings( KeyValues *pResourceData );
	virtual void	Paint( void );
	virtual void	PaintBackground( void );
	virtual void	OnTick( void );
	virtual void	OnCommand( const char* command );

	CParticlePanelInfo			*m_pParticleInfo;
protected:
	CParticleCollection	*m_pParticleSystem;
};
#endif // TF_PARTICLEPANEL_H
