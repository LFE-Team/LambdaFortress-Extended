#include "cbase.h"
#include "basemodelpanel.h"
#include "tf_playermodelpanel.h"
#include "renderparm.h"
#include "animation.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

DECLARE_BUILD_FACTORY( CTFPlayerModelPanel );

CTFPlayerModelPanel::CTFPlayerModelPanel(vgui::Panel *parent, const char *name) : CBaseModelPanel(parent, name)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(true);
	SetVisible(true);
	m_bShouldPaint = true;
	m_iAnimationIndex = 0;
	m_pStudioHdr = NULL;
	m_pData = NULL;
}

CTFPlayerModelPanel::~CTFPlayerModelPanel()
{

}

//-----------------------------------------------------------------------------
// Purpose: Load in the model portion of the panel's resource file.
//-----------------------------------------------------------------------------
void CTFPlayerModelPanel::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	InvalidateLayout( false, true ); // force ApplySchemeSettings to run

	// Parse our resource file and apply all necessary updates to the MDL.
	for ( KeyValues *pData = inResourceData->GetFirstSubKey() ; pData != NULL ; pData = pData->GetNextKey() )
	{
		if ( !Q_stricmp( pData->GetName(), "customclassdata" ) )
		{
			if ( !Q_stricmp( pData->GetName(), "undefined" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Scout" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Sniper" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Soldier" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Demoman" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Medic" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Heavy" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Pyro" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Spy" ) )
			{
				ParseModelResInfo( pData );
			}
			else if ( !Q_stricmp( pData->GetName(), "Engineer" ) )
			{
				ParseModelResInfo( pData );
			}
		}
 	}
}


void CTFPlayerModelPanel::PerformLayout()
{
	BaseClass::PerformLayout();
}


void CTFPlayerModelPanel::SetModelName(const char* name, int skin)
{
	m_BMPResData.m_pszModelName = name;
	m_BMPResData.m_nSkin = skin;
}

void CTFPlayerModelPanel::SetParticleName(const char* name)
{
	m_bUseParticle = true;

	if (m_pData)
	{
		SafeDeleteParticleData(&m_pData);
	}
	m_pData = CreateParticleData(name);

	// We failed at creating that particle for whatever reason, bail (!)
	if (!m_pData) return;

	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if (!pStudioHdr)
		return;

	CStudioHdr studioHdr(pStudioHdr, g_pMDLCache);
	CUtlVector<int> vecAttachments;

	m_pData->UpdateControlPoints(&studioHdr, &m_RootMDL.m_MDLToWorld, vecAttachments);
	m_pData->m_bIsUpdateToDate = true;
}


void CTFPlayerModelPanel::Update()
{
	MDLHandle_t hSelectedMDL = g_pMDLCache->FindMDL( m_BMPResData.m_pszModelName );
	g_pMDLCache->PreloadModel( hSelectedMDL );
	SetMDL( hSelectedMDL );

	if ( m_iAnimationIndex < m_BMPResData.m_aAnimations.Size() )
	{
		SetModelAnim( m_iAnimationIndex );
	}

	SetSkin( m_BMPResData.m_nSkin );
}

void CTFPlayerModelPanel::OnThink()
{
	BaseClass::OnThink();
}


void CTFPlayerModelPanel::Paint()
{
	CMatRenderContextPtr pRenderContext( materials );

	// Turn off depth-write to dest alpha so that we get white there instead. The code that uses
	// the render target needs a mask of where stuff was rendered.
	pRenderContext->SetIntRenderingParameter( INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA, false );

	// Disable flashlights when drawing our model
	pRenderContext->SetFlashlightMode(false);

	if ( m_bShouldPaint )
	{
		SetupLights();

		BaseClass::Paint();
	}
}

void CTFPlayerModelPanel::SetBodygroup( int iGroup, int iValue )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	::SetBodygroup( &studioHdr, m_RootMDL.m_MDL.m_nBody, iGroup, iValue );
}


int CTFPlayerModelPanel::FindBodygroupByName( const char *name )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return -1;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	return ::FindBodygroupByName( &studioHdr, name );
}

int CTFPlayerModelPanel::GetNumBodyGroups( void )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return 0;

	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );

	return ::GetNumBodyGroups( &studioHdr );
}

const char *CTFPlayerModelPanel::GetBodygroupName( int index )
{
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if ( !pStudioHdr )
		return 0;
 	CStudioHdr studioHdr( pStudioHdr, g_pMDLCache );
 	return ::GetBodygroupName( &studioHdr, index );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPlayerModelPanel::PrePaint3D( IMatRenderContext *pRenderContext )
{
	if ( !m_bUseParticle )
		return;

	// mark all effects need to be updated
	FOR_EACH_VEC( m_particleList, i )
	{
		m_particleList[i]->m_bIsUpdateToDate = false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFPlayerModelPanel::PostPaint3D(IMatRenderContext *pRenderContext)
{
	if (!m_bUseParticle)
		return;

	// This needs calling to reset various counters.
	g_pParticleSystemMgr->SetLastSimulationTime(gpGlobals->curtime);

	// Render Particles
	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PushMatrix();
	pRenderContext->LoadIdentity();

	FOR_EACH_VEC(m_particleList, i)
	{
		m_particleList[i]->m_pParticleSystem->Simulate(gpGlobals->frametime, false);
		m_particleList[i]->m_pParticleSystem->Render(pRenderContext);
		m_particleList[i]->m_bIsUpdateToDate = false;
	}

	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PopMatrix();
}

/*
void SetBodygroup(CStudioHdr *pstudiohdr, int& body, int iGroup, int iValue)
{
	// Build list of submodels
	BodyPartInfo_t *pBodyPartInfo = (BodyPartInfo_t*)stackalloc(m_pStudioHdr->numbodyparts * sizeof(BodyPartInfo_t));
	for (int i = 0; i < m_pStudioHdr->numbodyparts; ++i)
	{
		pBodyPartInfo[i].m_nSubModelIndex = R_StudioSetupModel(i, body, &pBodyPartInfo[i].m_pSubModel, m_pStudioHdr);
	}
	R_StudioRenderFinal(pRenderContext, skin, m_pStudioHdr->numbodyparts, pBodyPartInfo,
		pEntity, ppMaterials, pMaterialFlags, boneMask, lod, pColorMeshes)

		m_VertexCache.SetBodyPart(i);
	m_VertexCache.SetModel(pBodyPartInfo[i].m_nSubModelIndex);

	numFacesRendered += R_StudioDrawPoints(pRenderContext, skin, pClientEntity,
		ppMaterials, pMaterialFlags, boneMask, lod, pColorMeshes);
}
*/

/*
void CTFPlayerModelPanel::SetupCustomLights( Color cAmbient, Color cKey, float fKeyBoost, Color cRim, float fRimBoost )
{
	memset( &m_LightingState, 0, sizeof(MaterialLightingState_t) );
	
	for ( int i = 0; i < 6; ++i )
	{
		m_LightingState.m_vecAmbientCube[0].Init( cAmbient[0]/255.0f  , cAmbient[1]/255.0f , cAmbient[2]/255.0f );
	}
	
	// set up the lighting
	//QAngle angDir = vec3_angle;
	//Vector vecPos = vec3_origin;
	matrix3x4_t lightMatrix;
	matrix3x4_t rimLightMatrix;

	m_LightingState.m_nLocalLightCount = 2;
	// key light settings
	GetAttachment( "attach_light", lightMatrix );
	m_LightingState.m_pLocalLightDesc[0].m_Type = MATERIAL_LIGHT_SPOT;
	m_LightingState.m_pLocalLightDesc[0].m_Color.Init( ( cKey[0]/255.0f )* fKeyBoost  , ( cKey[1]/255.0f )* fKeyBoost  , ( cKey[2]/255.0f ) * fKeyBoost );
	m_LightToWorld[0] = lightMatrix;
	m_LightingState.m_pLocalLightDesc[0].m_Falloff = asw_key_falloff.GetFloat();
	m_LightingState.m_pLocalLightDesc[0].m_Attenuation0 = asw_key_atten.GetFloat();
	m_LightingState.m_pLocalLightDesc[0].m_Attenuation1 = 0;
	m_LightingState.m_pLocalLightDesc[0].m_Attenuation2 = 0;
	m_LightingState.m_pLocalLightDesc[0].m_Theta = asw_key_innerCone.GetFloat();
	m_LightingState.m_pLocalLightDesc[0].m_Phi = asw_key_outerCone.GetFloat();
	m_LightingState.m_pLocalLightDesc[0].m_Range = asw_key_range.GetFloat();
	m_LightingState.m_pLocalLightDesc[0].RecalculateDerivedValues();


	// rim light settings
	GetAttachment( "attach_light_rim", rimLightMatrix );
	m_LightingState.m_pLocalLightDesc[1].m_Type = MATERIAL_LIGHT_SPOT;
	m_LightingState.m_pLocalLightDesc[1].m_Color.Init( ( cRim[0]/255.0f ) * fRimBoost , ( cRim[1]/255.0f ) * fRimBoost, ( cRim[2]/255.0f )* fRimBoost);
	m_LightToWorld[1] = rimLightMatrix;
	m_LightingState.m_pLocalLightDesc[1].m_Falloff = asw_rim_falloff.GetFloat();
	m_LightingState.m_pLocalLightDesc[1].m_Attenuation0 = asw_rim_atten.GetFloat();
	m_LightingState.m_pLocalLightDesc[1].m_Attenuation1 = 0;
	m_LightingState.m_pLocalLightDesc[1].m_Attenuation2 = 0;
	m_LightingState.m_pLocalLightDesc[1].m_Theta = asw_rim_innerCone.GetFloat();
	m_LightingState.m_pLocalLightDesc[1].m_Phi = asw_rim_outerCone.GetFloat();
	m_LightingState.m_pLocalLightDesc[1].m_Range = asw_rim_range.GetFloat();
	m_LightingState.m_pLocalLightDesc[1].RecalculateDerivedValues();
}
*/