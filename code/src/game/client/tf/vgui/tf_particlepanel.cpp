//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"

#include <vgui_controls/ScrollBarSlider.h>
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "tf_particlepanel.h"


using namespace vgui;

DECLARE_BUILD_FACTORY( CTFParticlePanel );

//-----------------------------------------------------------------------------
// Purpose: Reverse engie live TF2 CTFParticlePanel
//-----------------------------------------------------------------------------
CTFParticlePanel::CTFParticlePanel( Panel *parent, const char *panelName ) : BaseClass( parent, panelName ) 
{
	m_pParticleInfo = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFParticlePanel::~CTFParticlePanel()
{
	if ( m_pParticleInfo )
	{
		delete m_pParticleInfo;
		m_pParticleInfo = NULL;
	}

	if ( m_pParticleSystem )
	{
		delete m_pParticleSystem;
		m_pParticleSystem = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFParticlePanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFParticlePanel::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

 	for ( KeyValues *pData = inResourceData->GetFirstSubKey() ; pData != NULL ; pData = pData->GetNextKey() )
 	{
 		if ( !Q_stricmp( pData->GetName(), "ParticleEffects" ) )
 		{
	 		if ( m_pParticleInfo )
	 		{
	 			delete m_pParticleInfo;
	 			m_pParticleInfo = NULL;
	 		}

	 		m_pParticleInfo = new CParticlePanelInfo;

	 		if ( !m_pParticleInfo )
	 			return;

	 		m_pParticleInfo->m_pszParticleName = ReadAndAllocStringValue( inResourceData, "particleName" );
			m_pParticleSystem = g_pParticleSystemMgr->CreateParticleCollection( m_pParticleInfo->m_pszParticleName );

	 		m_pParticleInfo->m_vecOriginOffset.Init( inResourceData->GetFloat( "particle_xpos", 110.0 ), inResourceData->GetFloat( "particle_ypos", 5.0 ) );
 		}
 	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFParticlePanel::PaintBackground( void )
{
	BaseClass::PaintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFParticlePanel::Paint( void )
{

}

void CTFParticlePanel::OnTick()
{
}

void CTFParticlePanel::OnCommand(const char* command)
{
	BaseClass::OnCommand( command );
}