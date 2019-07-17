#ifndef TFCREDITSPANEL_H
#define TFCREDITSPANEL_H

#include "tf_menupanelbase.h"
#include "steam/steam_api.h"
#include "tf_dialogpanelbase.h"
#include "tf_controls.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFCreditsPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFCreditsPanel, CTFDialogPanelBase);

public:
	CTFCreditsPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFCreditsPanel();

	void Show();
	void Hide();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
protected:
	CExRichText			*m_pText;
};

#endif // TFCREDITSPANEL_H