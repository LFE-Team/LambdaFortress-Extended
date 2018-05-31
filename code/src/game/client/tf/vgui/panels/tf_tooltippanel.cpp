#include "cbase.h"
#include "tf_tooltippanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbuttonbase.h"
#include <vgui_controls/TextImage.h>

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFToolTipPanel::CTFToolTipPanel( vgui::Panel* parent, const char *panelName ) : CTFMenuPanelBase( parent, panelName )
{
	m_pText = new CExLabel( this, "TextLabel", "" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFToolTipPanel::~CTFToolTipPanel()
{

}

void CTFToolTipPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( GetResFilename() );
}

void CTFToolTipPanel::PerformLayout( void )
{
	BaseClass::PerformLayout();

	AdjustToolTipSize();
}

void CTFToolTipPanel::ShowToolTip( char *sText )
{
	Show();

	if ( m_pText )
	{
		m_pText->SetText( sText );
		InvalidateLayout( true );
	}
}

void CTFToolTipPanel::HideToolTip( void )
{
	Hide();
}

void CTFToolTipPanel::Show( void )
{
	BaseClass::Show();
	MakePopup();
}

void CTFToolTipPanel::Hide( void )
{
	BaseClass::Hide();
}

void CTFToolTipPanel::OnThink( void )
{
	BaseClass::OnThink();
	int cursorX, cursorY;
	surface()->SurfaceGetCursorPos( cursorX, cursorY );
	//SetPos(cursorX + toProportionalWide(8), cursorY + toProportionalTall(10));

	int iTipW, iTipH;
	GetSize( iTipW, iTipH );

	int wide, tall;
	surface()->GetScreenSize( wide, tall );

	if ( wide - iTipW > cursorX )
	{
		cursorY += YRES( 10 );
		cursorX += XRES( 8 );

		// menu hanging right
		if ( tall - iTipH > cursorY )
		{
			// menu hanging down
			SetPos( cursorX, cursorY );
		}
		else
		{
			// menu hanging up
			SetPos( cursorX, cursorY - iTipH - YRES( 20 ) );
		}
	}
	else
	{
		cursorY += YRES( 10 );
		cursorX += XRES( 8 );
		// menu hanging left
		if ( tall - iTipH > cursorY )
		{
			// menu hanging down
			SetPos( cursorX - iTipW, cursorY );
		}
		else
		{
			// menu hanging up
			SetPos( cursorX - iTipW, cursorY - iTipH - YRES( 20 ) );
		}
	}
}

void CTFToolTipPanel::AdjustToolTipSize( void )
{
	// Figure out text size and resize tooltip accordingly.
	if ( m_pText )
	{
		int x, y, wide, tall;
		m_pText->GetPos( x, y );
		m_pText->GetTextImage()->GetContentSize( wide, tall );

		m_pText->SetTall( tall );
		SetSize( x * 2 + wide, y * 2 + tall );
	}
}
