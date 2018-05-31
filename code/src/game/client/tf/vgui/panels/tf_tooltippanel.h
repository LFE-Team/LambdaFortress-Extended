#ifndef TF_MAINMENUTOOLTIPPANEL_H
#define TF_MAINMENUTOOLTIPPANEL_H

#include "tf_dialogpanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFToolTipPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFToolTipPanel, CTFMenuPanelBase);

public:
	CTFToolTipPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFToolTipPanel();
	virtual void PerformLayout( void );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void OnThink( void );
	virtual void Show( void );
	virtual void Hide( void );
	virtual void ShowToolTip( char *sText );
	virtual void HideToolTip( void );
	virtual void AdjustToolTipSize( void );

	virtual const char *GetResFilename( void ) { return "resource/UI/main_menu/ToolTipPanel.res"; }

protected:
	CExLabel	*m_pText;
};

#endif // TF_MAINMENUTOOLTIPPANEL_H