#ifndef TF_PLAYERMODELPANEL_H
#define TF_PLAYERMODELPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include "basemodel_panel.h"

class CStudioHdr;

class CTFPlayerModelPanel : public CBaseModelPanel
{
	DECLARE_CLASS_SIMPLE(CTFPlayerModelPanel, CBaseModelPanel);
public:
	CTFPlayerModelPanel( vgui::Panel *parent, const char *name );
	virtual ~CTFPlayerModelPanel();

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void PerformLayout();
	virtual void OnThink();
	virtual void Paint();
	virtual void SetupLights() { }

	void	SetModelName(const char* name, int skin = 0);
	void	SetParticleName(const char* name);

	void	SetAnimationIndex(int iIndex) { m_iAnimationIndex = iIndex; };

	void	Update();

	// function to set up scene sets
	//void SetupCustomLights( Color cAmbient, Color cKey, float fKeyBoost, Color cRim, float fRimBoost );

	void SetBodygroup( int iGroup, int iValue );
	int FindBodygroupByName( const char *name );
	int GetNumBodyGroups();
	const char *GetBodygroupName( int index );
	CStudioHdr *GetModelPtr();
	

	bool m_bShouldPaint;

	virtual void	SetupFlexWeights( void ) { return; }
	virtual void	FireEvent( const char *pszEventName, const char *pszEventOptions ) { }
private:
	CStudioHdr* m_pStudioHdr;
	particle_data_t *m_pData;
	int		m_iAnimationIndex;

protected:
	virtual void PrePaint3D( IMatRenderContext *pRenderContext ) OVERRIDE;
	virtual void PostPaint3D( IMatRenderContext *pRenderContext ) OVERRIDE;
	virtual void RenderingRootModel( IMatRenderContext *pRenderContext, CStudioHdr *pStudioHdr, MDLHandle_t mdlHandle, matrix3x4_t *pWorldMatrix ) { };
	virtual void RenderingMergedModel( IMatRenderContext *pRenderContext, CStudioHdr *pStudioHdr, MDLHandle_t mdlHandle, matrix3x4_t *pWorldMatrix ) { };
};


#endif // TF_PLAYERMODELPANEL_H
