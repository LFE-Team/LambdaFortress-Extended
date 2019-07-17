#ifndef TFGENERICCONFIRMATION_H
#define TFGENERICCONFIRMATION_H

#include "tf_dialogpanelbase.h"

class CCvarToggleCheckButton;
class CTFAdvButton;
class CExLabel;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFGenericConfirmation : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFGenericConfirmation, CTFDialogPanelBase);

public:
	CTFGenericConfirmation(vgui::Panel* parent, const char *panelName);
	virtual ~CTFGenericConfirmation();

	void Show();
	void Hide();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void LoadLayout();

	typedef void (*Callback_t)(void);

	struct Data_t
	{
		const char* pWindowTitle;
		const char* pMessageText;
		
		bool        bOkButtonEnabled;
		Callback_t	pfnOkCallback;
		const char* pOkButtonText;

		bool        bCancelButtonEnabled;
		Callback_t	pfnCancelCallback;
		const char* pCancelButtonText;

		bool		bCheckBoxEnabled;
		const char *pCheckBoxLabelText;
		const char *pCheckBoxCvarName;

		Data_t();
	};

	int  SetUsageData( const Data_t & data );     // returns the usageId, different number each time this is called
	int  GetUsageId() const { return m_usageId; }

	// Accessors
	const Data_t & GetUsageData() const { return m_data; }

protected:

	bool m_OkButtonEnabled;
	bool m_CancelButtonEnabled;

	CTFAdvButton* m_pBtnOK;
	CTFAdvButton* m_pBtnCancel;
	CCvarToggleCheckButton *m_pCheckBox;

	CExLabel *m_pLblCheckBox;
private:

	Data_t		 m_data;
	int			 m_usageId;

	static int sm_currentUsageId;
};

#endif // TFGENERICCONFIRMATION_H