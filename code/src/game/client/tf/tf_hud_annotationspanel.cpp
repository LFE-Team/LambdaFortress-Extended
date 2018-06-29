//===================== 2018 ISPuddy reversed engineering =====================//
// Reverse Engie Training Annotations
// An in-world location-specific information bubble HUD.
//=============================================================================//

#include "cbase.h"
#include "tf_hud_annotationspanel.h"
#include "vgui_controls/AnimationController.h"
#include "iclientmode.h"
#include "c_tf_player.h"
#include "c_tf_playerresource.h"
#include <vgui_controls/Label.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include "c_baseobject.h"
#include "fmtstr.h"
#include "tf_gamerules.h"
#include "tf_hud_statpanel.h"
#include "view.h"
#include "ivieweffects.h"
#include "viewrender.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_HUDELEMENT_DEPTH( CTFAnnotationsPanel, 1 );

#define ANNOTATION_CALLOUT_WIDE		(XRES(100))
#define ANNOTATION_CALLOUT_TALL		(XRES(50))

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAnnotationsPanel::CTFAnnotationsPanel( const char *pElementName )
	: EditablePanel( NULL, "AnnotationsPanel" ), CHudElement( pElementName )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	SetVisible( false );
	SetScheme( "ClientScheme" );

	m_flShowCalloutsAt = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::Reset()
{
	Hide();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::Init()
{
	// listen for events
	ListenForGameEvent( "show_annotation" );
	ListenForGameEvent( "hide_annotation" );
	
	Hide();

	CHudElement::Init();
}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::FireGameEvent( IGameEvent * event )
{
	const char *pEventName = event->GetName();

/* this is from TF2's ModEvents.res
	"show_annotation"
	{
		"worldPosX" "float"
		"worldPosY" "float"
		"worldPosZ" "float"
		"worldNormalX" "float"
		"worldNormalY" "float"
		"worldNormalZ" "float"
		"id" "long"
		"text"		"string"	// name (unlocalized)
		"lifetime"	"float"
		"visibilityBitfield"	"long" // bitfield of the players that can see this
		"follow_entindex"	"long" // if this is set, follow this entity
		"show_distance"		"bool"
		"play_sound"		"string"
		"show_effect"		"bool"
	}

	"hide_annotation"
	{
		"id" "long"
	}
*/
	if ( Q_strcmp( "hide_annotation", pEventName ) == 0 )
	{
		//int iEntID = event->GetInt( "id" );
		Hide();
	}
	else if ( Q_strcmp( "show_annotation", pEventName ) == 0 )
	{
		Show();

		//const char *displaytext = event->GetString( "text" );
		//m_pDisplayTextLabel->SetText( displaytext );

		//int iEntID = event->GetInt( "id" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::ShowCalloutsIn( float flTime )
{
	m_flShowCalloutsAt = gpGlobals->curtime + flTime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFAnnotationsPanelCallout *CTFAnnotationsPanel::TestAndAddCallout( Vector &origin, Vector &vMins, Vector &vMaxs, CUtlVector<Vector> *vecCalloutsTL, 
			CUtlVector<Vector> *vecCalloutsBR, Vector &vecFreezeTL, Vector &vecFreezeBR, Vector &vecStatTL, Vector &vecStatBR, int *iX, int *iY )
{
	// This is the offset from the topleft of the callout to the arrow tip
	const int iXOffset = XRES(25);
	const int iYOffset = YRES(50);

	//if ( engine->IsBoxInViewCluster( vMins + origin, vMaxs + origin) && !engine->CullBox( vMins + origin, vMaxs + origin ) )
	{
		if ( GetVectorInScreenSpace( origin, *iX, *iY ) )
		{
			*iX -= iXOffset;
			*iY -= iYOffset;
			int iRight = *iX + ANNOTATION_CALLOUT_WIDE;
			int iBottom = *iY + ANNOTATION_CALLOUT_TALL;
			if ( *iX > 0 && *iY > 0 && (iRight < ScreenWidth()) && (iBottom < (ScreenHeight()-YRES(40))) )
			{
				// Make sure it wouldn't be over the top of the freezepanel or statpanel
				Vector vecCalloutTL( *iX, *iY, 0 );
				Vector vecCalloutBR( iRight, iBottom, 1 );
				if ( !QuickBoxIntersectTest( vecCalloutTL, vecCalloutBR, vecFreezeTL, vecFreezeBR ) &&
					 !QuickBoxIntersectTest( vecCalloutTL, vecCalloutBR, vecStatTL, vecStatBR ) )
				{
					// Make sure it doesn't intersect any other callouts
					bool bClear = true;
					for ( int iCall = 0; iCall < vecCalloutsTL->Count(); iCall++ )
					{
						if ( QuickBoxIntersectTest( vecCalloutTL, vecCalloutBR, vecCalloutsTL->Element(iCall), vecCalloutsBR->Element(iCall) ) )
						{
							bClear = false;
							break;
						}
					}

					if ( bClear )
					{
						// Verify that we have LOS to the gib
						trace_t	tr;
						UTIL_TraceLine( origin, MainViewOrigin(), MASK_OPAQUE, NULL, COLLISION_GROUP_NONE, &tr );
						bClear = ( tr.fraction >= 1.0f );
					}

					if ( bClear )
					{
						CTFAnnotationsPanelCallout *pCallout = new CTFAnnotationsPanelCallout( g_pClientMode->GetViewport(), "AnnotationsPanelCallout" );
						m_pCalloutPanels.AddToTail( vgui::SETUP_PANEL(pCallout) );
						vecCalloutsTL->AddToTail( vecCalloutTL );
						vecCalloutsBR->AddToTail( vecCalloutBR );
						pCallout->SetVisible( true );
						pCallout->SetBounds( *iX, *iY, ANNOTATION_CALLOUT_WIDE, ANNOTATION_CALLOUT_TALL );
						return pCallout;
					}
				}
			}
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::UpdateCallout( void )
{
/*
	CTFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer )
		return;

	// Precalc the vectors of the freezepanel & statpanel
	int iX, iY;
	m_pFreezePanelBG->GetPos( iX, iY );
	Vector vecFreezeTL( iX, iY, 0 );
	Vector vecFreezeBR( iX + m_pFreezePanelBG->GetWide(), iY + m_pFreezePanelBG->GetTall(), 1 );

	CUtlVector<Vector> vecCalloutsTL;
	CUtlVector<Vector> vecCalloutsBR;

	Vector vecStatTL(0,0,0);
	Vector vecStatBR(0,0,1);
	CTFStatPanel *pStatPanel = GET_HUDELEMENT( CTFStatPanel );
	if ( pStatPanel && pStatPanel->IsVisible() )
	{
		pStatPanel->GetPos( iX, iY );
		vecStatTL.x = iX;
		vecStatTL.y = iY;
		vecStatBR.x = vecStatTL.x + pStatPanel->GetWide();
		vecStatBR.y = vecStatTL.y + pStatPanel->GetTall();
	}

	Vector vMins, vMaxs;

	CBaseEntity *pTr = entindex();
	if ( pTr )
	{
		Vector origin = pTr->GetRenderOrigin();
		pTr->GetRenderBounds( vMins, vMaxs );

		// Try and add the callout
		CTFAnnotationsPanelCallout *pCallout = TestAndAddCallout( false, vMins, vMaxs, &vecCalloutsTL, &vecCalloutsBR, 
			vecFreezeTL, vecFreezeBR, vecStatTL, vecStatBR, &iX, &iY );
		if ( pCallout )
		{
			pCallout->UpdateText();
			iCount++;
		}
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::Show()
{
	m_flShowCalloutsAt = 0;
	SetVisible( true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::Hide()
{
	SetVisible( false );

	// Delete all our callout panels
	for ( int i = m_pCalloutPanels.Count()-1; i >= 0; i-- )
	{
		m_pCalloutPanels[i]->MarkForDeletion();
	}
	m_pCalloutPanels.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFAnnotationsPanel::ShouldDraw( void )
{
	return ( IsVisible() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanel::OnThink( void )
{
	BaseClass::OnThink();

	if ( m_flShowCalloutsAt && m_flShowCalloutsAt < gpGlobals->curtime )
	{
		if ( ShouldDraw() )
		{
			UpdateCallout();
		}
		m_flShowCalloutsAt = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFAnnotationsPanelCallout::CTFAnnotationsPanelCallout( Panel *parent, const char *name ) : EditablePanel(parent,name)
{
	m_pDisplayTextLabel = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void CTFAnnotationsPanelCallout::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/AnnotationsPanelCallout.res" );

	m_pDisplayTextLabel = dynamic_cast<Label *>( FindChildByName("CalloutLabel") );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAnnotationsPanelCallout::UpdateText( void )
{
	if ( !m_pDisplayTextLabel )
		return;

	int wide, tall;
	m_pDisplayTextLabel->GetContentSize( wide, tall );

	// is the text wider than the label?
	if ( wide > m_pDisplayTextLabel->GetWide() )
	{
		int nDiff = wide - m_pDisplayTextLabel->GetWide();
		int x, y, w, t;

		// make the label wider
		m_pDisplayTextLabel->GetBounds( x, y, w, t );
		m_pDisplayTextLabel->SetBounds( x, y, w + nDiff, t );

		CTFImagePanel *pBackground = dynamic_cast<CTFImagePanel *>( FindChildByName( "CalloutBG" ) );
		if ( pBackground )
		{
			// also adjust the background image
			pBackground->GetBounds( x, y, w, t );
			pBackground->SetBounds( x, y, w + nDiff, t );
		}

		// make ourselves bigger to accommodate the wider children
		GetBounds( x, y, w, t );
		SetBounds( x, y, w + nDiff, t );

		// check that we haven't run off the right side of the screen
		if ( x + GetWide() > ScreenWidth() )
		{
			// push ourselves to the left to fit on the screen
			nDiff = ( x + GetWide() ) - ScreenWidth();
			SetPos( x - nDiff, y );

			// push the arrow to the right to offset moving ourselves to the left
			vgui::ImagePanel *pArrow = dynamic_cast<ImagePanel *>( FindChildByName( "ArrowIcon" ) );
			if ( pArrow )
			{
				pArrow->GetBounds( x, y, w, t );
				pArrow->SetBounds( x + nDiff, y, w, t );
			}
		}
	}
}