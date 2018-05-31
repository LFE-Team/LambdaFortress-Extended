#include "cbase.h"
#include "tf_itemtooltippanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbuttonbase.h"
#include "controls/tf_advmodelpanel.h"
#include <vgui/ILocalize.h>
#include <vgui_controls/TextImage.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFItemToolTipPanel::CTFItemToolTipPanel( Panel *parent, const char *panelName ) : CTFToolTipPanel( parent, panelName )
{
	m_pClassModelPanel = new CTFAdvModelPanel( this, "classmodelpanel" );
	m_pTitle = new CExLabel( this, "TitleLabel", "Title" );
	m_pClassName = new CExLabel( this, "ClassNameLabel", "ClassName" );
	m_pAttributeText = new CExLabel( this, "AttributeLabel", "Attribute" );

	for ( int i = 0; i < 20; i++ )
	{
		m_pAttributes.AddToTail( new CExLabel( this, VarArgs( "attribute_%d", i ), "Attribute" ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFItemToolTipPanel::~CTFItemToolTipPanel()
{

}

void CTFItemToolTipPanel::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	if ( m_pTitle )
	{
		m_colorTitle = m_pTitle->GetFgColor();
	}
}

void CTFItemToolTipPanel::OnChildSettingsApplied( KeyValues *pInResourceData, Panel *pChild )
{
	// Apply settings from template to all attribute strings.
	if ( pChild == m_pAttributeText )
	{
		for ( int i = 0; i < m_pAttributes.Count(); i++ )
		{
			m_pAttributes[i]->ApplySettings( pInResourceData );
		}
	}

	BaseClass::OnChildSettingsApplied( pInResourceData, pChild );
}

void CTFItemToolTipPanel::ShowToolTip( CEconItemDefinition *pItemData )
{
	Show();

	IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
	if ( !pScheme )
		return;

	/*
	char pModel[64];
	Q_snprintf(pModel, sizeof(pModel), pItemData->model_world);
	if (!Q_strcmp(pModel, ""))
	Q_snprintf(pModel, sizeof(pModel), pItemData->model_player);
	m_pClassModelPanel->SetModelName(strdup(pModel), 0);
	if (Q_strcmp(pModel, ""))
	{
	m_pClassModelPanel->SetVisible(true);
	m_pClassModelPanel->Update();
	}
	*/

	// Set item name.
	if ( m_pTitle )
	{
		m_pTitle->SetText( pItemData->GenerateLocalizedFullItemName() );

		// Set the color according to quality.	
		const char *pszColor = EconQuality_GetColorString( pItemData->item_quality );

		if ( pszColor )
		{
			m_pTitle->SetFgColor( pScheme->GetColor( pszColor, m_colorTitle ) );
		}
	}

	// Set item type name.
	if ( m_pClassName )
	{
		m_pClassName->SetText( pItemData->item_type_name );
	}

	// List atrributes.
	for ( int i = 0; i < m_pAttributes.Count(); i++ )
		m_pAttributes[i]->SetVisible( false );

	if ( m_pAttributeText )
	{
		for ( int i = 0; i <= pItemData->attributes.Count() && i < m_pAttributes.Count(); i++ )
		{
			CExLabel *pLabel = m_pAttributes[i];

			if ( i == pItemData->attributes.Count() )
			{
				// Show item description at the end.
				if ( pItemData->item_description[0] == '\0' )
					continue;

				pLabel->SetText( pItemData->item_description );

				pLabel->SetFgColor( pScheme->GetColor( "ItemAttribNeutral", COLOR_WHITE ) );
				pLabel->SetVisible( true );
			}
			else
			{
				CEconItemAttribute *pAttribute = &pItemData->attributes[i];
				EconAttributeDefinition *pStatic = pAttribute->GetStaticData();
				if ( !pStatic || pStatic->hidden )
					continue;

				const wchar_t *pszLocalized = g_pVGuiLocalize->Find( pStatic->description_string );

				if ( !pszLocalized || pszLocalized[0] == '\0' )
					continue;

				float flValue = pAttribute->value;

				switch ( pStatic->description_format )
				{
				case ATTRIB_FORMAT_PERCENTAGE:
					flValue = flValue - 1.0f;
					flValue *= 100.0f;
					break;
				case ATTRIB_FORMAT_INVERTED_PERCENTAGE:
					flValue = 1.0f - flValue;
					flValue *= 100.0f;
					break;
				case ATTRIB_FORMAT_ADDITIVE_PERCENTAGE:
					flValue *= 100.0f;
					break;
				}

				wchar_t wszValue[32];
				V_snwprintf( wszValue, sizeof( wszValue ) / sizeof( wchar_t ), L"%.0f", flValue );

				wchar_t wszAttrib[128];
				g_pVGuiLocalize->ConstructString( wszAttrib, sizeof( wszAttrib ), pszLocalized, 1, wszValue );

				pLabel->SetText( wszAttrib );

				Color attrcolor;
				switch ( pStatic->effect_type )
				{
				case ATTRIB_EFFECT_POSITIVE:
					attrcolor = pScheme->GetColor( "ItemAttribPositive", COLOR_WHITE );
					break;
				case ATTRIB_EFFECT_NEGATIVE:
					attrcolor = pScheme->GetColor( "ItemAttribNegative", COLOR_WHITE );
					break;			
				case ATTRIB_EFFECT_NEUTRAL:
				default:
					attrcolor = pScheme->GetColor( "ItemAttribNeutral", COLOR_WHITE );
					break;
				}

				pLabel->SetFgColor( attrcolor );
				pLabel->SetVisible( true );
			}
		}
	}

	InvalidateLayout( true );
}

void CTFItemToolTipPanel::HideToolTip( void )
{
	Hide();
}

void CTFItemToolTipPanel::AdjustToolTipSize( void )
{
	// Place attributes one under the other.
	int x, y;
	m_pAttributeText->GetPos( x, y );

	int iTotalHeight = y;
	for ( int i = 0; i < m_pAttributes.Count(); i++ )
	{
		CExLabel *pLabel = m_pAttributes[i];

		if ( pLabel->IsVisible() )
		{
			pLabel->SetPos( x, iTotalHeight );

			int twide, ttall;
			pLabel->GetTextImage()->GetContentSize( twide, ttall );
			pLabel->SetTall( ttall );

			iTotalHeight += ttall;
		}
	}

	// Set the tooltip size based on attribute list size.
	SetTall( iTotalHeight + YRES( 10 ) );
}
