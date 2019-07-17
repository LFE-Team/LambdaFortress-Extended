//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "lfe_genericconfirmation.h"
#include "tf_mainmenu.h"
#include "vgui_controls/Label.h"
#include "vgui/ISurface.h"
#include "controls/tf_advbutton.h"
#include "cdll_util.h"
#include "controls/tf_cvartogglecheckbutton.h"

#include "modinfo.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


int CTFGenericConfirmation::sm_currentUsageId = 0;

CTFGenericConfirmation::Data_t::Data_t() : 
	pWindowTitle( NULL ),
	pMessageText( NULL ),
	bOkButtonEnabled( false ),
	pfnOkCallback( NULL ),
	pOkButtonText( NULL ),
	bCancelButtonEnabled( false ),
	pfnCancelCallback( NULL ),
	pCancelButtonText( NULL ),
	bCheckBoxEnabled( false ),
	pCheckBoxLabelText( NULL ),
	pCheckBoxCvarName( NULL )
{
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFGenericConfirmation::CTFGenericConfirmation( vgui::Panel* parent, const char *panelName ) :
	CTFDialogPanelBase( parent, panelName ),
	m_pLblCheckBox( 0 ),
	m_data(),
	m_usageId( 0 ),
	m_pCheckBox( 0 )
{
	m_OkButtonEnabled = true;
	m_CancelButtonEnabled = true;

	m_pBtnOK = new CTFAdvButton( this, "BtnOK", "" );
	m_pBtnCancel = new CTFAdvButton( this, "BtnCancel", "" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFGenericConfirmation::~CTFGenericConfirmation()
{
}

void CTFGenericConfirmation::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/GenericConfirmation.res");
}

void CTFGenericConfirmation::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		if ( m_CancelButtonEnabled )
		{
			if ( m_pCheckBox )
			{
				m_pCheckBox->ApplyChanges();
			}

			PostActionSignal(new KeyValues("CancelPressed"));
			OnResetData();
			Hide();

			if ( m_data.pfnCancelCallback != 0 )
			{
				m_data.pfnCancelCallback();
			}
		}
	}
	else if (!stricmp(command, "Ok"))
	{
		if ( m_OkButtonEnabled )
		{
			if ( m_pCheckBox )
			{
				m_pCheckBox->ApplyChanges();
			}

			PostActionSignal(new KeyValues("OkPressed"));
			OnApplyChanges();
			Hide();

			if ( m_data.pfnOkCallback != 0 )
			{
				m_data.pfnOkCallback();
			}
		}
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFGenericConfirmation::Show()
{
	BaseClass::Show();
	LoadLayout();
}

void CTFGenericConfirmation::Hide()
{
	BaseClass::Hide();
}

int CTFGenericConfirmation::SetUsageData( const Data_t & data )
{
	m_data = data;

	//SetTitle( data.pWindowTitle, false );
	CExLabel *pLblTitle = dynamic_cast< CExLabel* >( FindChildByName( "LblTitle" ) );
	pLblTitle->SetText( data.pWindowTitle );
	pLblTitle->MakeReadyForUse();

	CExLabel *pLblMessage = dynamic_cast< CExLabel* >( FindChildByName( "LblMessage" ) );
	pLblMessage->SetText( data.pMessageText );
	pLblMessage->MakeReadyForUse();
	//pLblMessage->m_bAutoWideToContents = true;

	CTFAdvButton *pOkButton = dynamic_cast< CTFAdvButton* >( FindChildByName( "BtnOk" ) );
	if ( data.pOkButtonText )
		pOkButton->SetText( data.pOkButtonText );

	CTFAdvButton *pCancelButton = dynamic_cast< CTFAdvButton* >( FindChildByName( "BtnCancel" ) );
	if ( data.pCancelButtonText )
		pCancelButton->SetText( data.pCancelButtonText );

	// tell our base version so input is disabled
	m_OkButtonEnabled = data.bOkButtonEnabled;
	m_CancelButtonEnabled = data.bCancelButtonEnabled;

	if ( data.bCheckBoxEnabled && data.pCheckBoxCvarName )
	{
		m_pCheckBox = new CCvarToggleCheckButton( 
			this, 
			"CheckButton", 
			"",
			data.pCheckBoxCvarName /*,
			true*/ );

		m_pCheckBox->MakeReadyForUse();

		//m_pLblCheckBox = new CExLabel( this, "LblCheckButton", data.pCheckBoxLabelText );
	}

	if ( m_data.pfnOkCallback == 0 )
		m_pBtnOK->SetCommandString("Ok");

	if ( m_data.pfnCancelCallback == 0 )
		m_pBtnCancel->SetCommandString("vguicancel");

	m_pBtnOK->MakeReadyForUse();
	m_pBtnCancel->MakeReadyForUse();

	if ( UI_IsDebug() )
	{
		Msg( "[GAMEUI] GenericConfirmation::SetWindowTitle : %s\n", data.pWindowTitle!=NULL ? data.pWindowTitle : "<NULL>" );
		Msg( "[GAMEUI] GenericConfirmation::SetMessageText : %s\n", data.pMessageText!=NULL ? data.pMessageText : "<NULL>" );
	}

	return m_usageId = ++sm_currentUsageId;
}

//=============================================================================
void CTFGenericConfirmation::LoadLayout()
{
	/*vgui::Button *pOkButton = NULL;
	vgui::Button *pCancelButton = NULL;

	pOkButton = dynamic_cast< vgui::Button* >( FindChildByName( "BtnOk" ) );
	pCancelButton = dynamic_cast< vgui::Button* >( FindChildByName( "BtnCancel" ) );
	pOkButton->GetSize( buttonWide, buttonTall );

	if ( m_data.bCancelButtonEnabled || m_data.bOkButtonEnabled )
	{
		// when only one button is enabled, center that button
		vgui::Button *pButton = NULL;
		bool bSingleButton = false;
		if ( ( m_data.bCancelButtonEnabled && !m_data.bOkButtonEnabled ) )
		{
			// cancel is centered
			bSingleButton = true;
			pButton = pCancelButton;
		}
		else if ( !m_data.bCancelButtonEnabled && m_data.bOkButtonEnabled )
		{
			// OK is centered
			bSingleButton = true;
			pButton = pOkButton;
		}

		if ( bSingleButton )
		{
			// center the button
			pButton->SetPos( ( dialogWidth - pButton->GetWide() )/2, dialogHeight - borderGap - buttonTall );
		}
	}*/
}
	