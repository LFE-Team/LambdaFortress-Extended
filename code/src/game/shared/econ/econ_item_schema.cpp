#include "cbase.h"
#include "econ_item_schema.h"
#include "econ_item_system.h"
#include "tier3/tier3.h"
#include "vgui/ILocalize.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

const char *EconQuality_GetColorString( EEconItemQuality quality )
{
	if ( quality >= 0 && quality < QUALITY_COUNT )
	{
		return g_szQualityColorStrings[quality];
	}

	return NULL;
}

const char *EconQuality_GetLocalizationString( EEconItemQuality quality )
{
	if ( quality >= 0 && quality < QUALITY_COUNT )
	{
		return g_szQualityLocalizationStrings[quality];
	}

	return NULL;
}

#ifdef CLIENT_DLL
//=============================================================================
// CEconItemAttribute
//=============================================================================
void RecvProxy_AttributeClass( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	RecvProxy_StringToString( pData, pStruct, pOut );

	// Cache received attribute string.
	CEconItemAttribute *pAttrib = (CEconItemAttribute *)pStruct;
	pAttrib->m_strAttributeClass = AllocPooledString( pAttrib->attribute_class );
}
#endif

BEGIN_NETWORK_TABLE_NOBASE( CEconItemAttribute, DT_EconItemAttribute )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iAttributeDefinitionIndex ) ),
	RecvPropFloat( RECVINFO( value ) ),
	RecvPropString( RECVINFO( value_string ) ),
	RecvPropString( RECVINFO( attribute_class ), 0, RecvProxy_AttributeClass ),
#else
	SendPropInt( SENDINFO( m_iAttributeDefinitionIndex ) ),
	SendPropFloat( SENDINFO( value ) ),
	SendPropString( SENDINFO( value_string ) ),
	SendPropString( SENDINFO( attribute_class ) ),
#endif
END_NETWORK_TABLE()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconItemAttribute::Init( int iIndex, float flValue, const char *pszAttributeClass /*= NULL*/ )
{
	m_iAttributeDefinitionIndex = iIndex;
	value = flValue;
	value_string.GetForModify()[0] = '\0';

	if ( pszAttributeClass )
	{
		V_strncpy( attribute_class.GetForModify(), pszAttributeClass, sizeof( attribute_class ) );
	}
	else
	{
		EconAttributeDefinition *pAttribDef = GetStaticData();
		if ( pAttribDef )
		{
			V_strncpy( attribute_class.GetForModify(), pAttribDef->attribute_class, sizeof( attribute_class ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconItemAttribute::Init( int iIndex, const char *pszValue, const char *pszAttributeClass /*= NULL*/ )
{
	m_iAttributeDefinitionIndex = iIndex;
	value = 0.0f;
	V_strncpy( value_string.GetForModify(), pszValue, sizeof( value_string ) );

	if ( pszAttributeClass )
	{
		V_strncpy( attribute_class.GetForModify(), pszAttributeClass, sizeof( attribute_class ) );
	}
	else
	{
		EconAttributeDefinition *pAttribDef = GetStaticData();
		if ( pAttribDef )
		{
			V_strncpy( attribute_class.GetForModify(), pAttribDef->attribute_class, sizeof( attribute_class ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
EconAttributeDefinition *CEconItemAttribute::GetStaticData( void )
{
	return GetItemSchema()->GetAttributeDefinition( m_iAttributeDefinitionIndex );
}


//=============================================================================
// CEconItemDefinition
//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
EconItemVisuals *CEconItemDefinition::GetVisuals( int iTeamNum /*= TEAM_UNASSIGNED*/ )
{
	if ( iTeamNum > LAST_SHARED_TEAM && iTeamNum < TF_TEAM_COUNT )
	{
		return &visual[iTeamNum];
	}

	return &visual[TEAM_UNASSIGNED];
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CEconItemDefinition::GetLoadoutSlot( int iClass /*= TF_CLASS_UNDEFINED*/ )
{
	if ( iClass && item_slot_per_class[iClass] != -1 )
	{
		return item_slot_per_class[iClass];
	}

	return item_slot;
}

//-----------------------------------------------------------------------------
// Purpose: Generate item name to show in UI with prefixes, qualities, etc...
//-----------------------------------------------------------------------------
const wchar_t *CEconItemDefinition::GenerateLocalizedFullItemName( void )
{
	static wchar_t wszFullName[256];

	wchar_t wszQuality[128] = { '\0' };

	if ( item_quality == QUALITY_UNIQUE )
	{
		// Attach "the" if necessary to unique items.
		if ( propername )
		{
			const wchar_t *pszPrepend = g_pVGuiLocalize->Find( "#TF_Unique_Prepend_Proper" );

			if ( pszPrepend )
			{
				V_wcsncpy( wszQuality, pszPrepend, sizeof( wszQuality ) );
			}
		}
	}
	else if ( item_quality != QUALITY_NORMAL )
	{
		// Live TF2 apparently allows multiple qualities per item but eh, we don't need that for now.
		const char *pszLocale = EconQuality_GetLocalizationString( item_quality );

		if ( pszLocale )
		{	
			const wchar_t *pszQuality = g_pVGuiLocalize->Find( pszLocale );

			if ( pszQuality )
			{
				V_wcsncpy( wszQuality, pszQuality, sizeof( wszQuality ) );
				// Add a space at the end.
				V_wcsncat( wszQuality, L" ", sizeof( wszQuality ) >> 2 );
			}
		}	
	}

	// Get base item name.
	wchar_t wszItemName[128];

	const wchar_t *pszLocalizedName = g_pVGuiLocalize->Find( item_name );
	if ( pszLocalizedName )
	{
		V_wcsncpy( wszItemName, pszLocalizedName, sizeof( wszItemName ) );
	}
	else
	{
		g_pVGuiLocalize->ConvertANSIToUnicode( item_name, wszItemName, sizeof( wszItemName ) );
	}

	// Oh boy.
	wchar_t wszCraftNumber[128] = { '\0' };
	wchar_t wszCraftSeries[128] = { '\0' };
	wchar_t wszToolTarget[128] = { '\0' };
	wchar_t wszRecipeComponent[128] = { '\0' };

	const wchar_t *pszFormat = g_pVGuiLocalize->Find( "#ItemNameFormat" );

	if ( pszFormat )
	{
		g_pVGuiLocalize->ConstructString( wszFullName, sizeof( wszFullName ), pszFormat, 6,
			wszQuality, wszItemName, wszCraftNumber, wszCraftSeries, wszToolTarget, wszRecipeComponent );
	}
	else
	{
		V_wcsncpy( wszFullName, L"Unlocalized", sizeof( wszFullName ) );
	}

	return wszFullName;
}

//-----------------------------------------------------------------------------
// Purpose: Find an attribute with the specified class.
//-----------------------------------------------------------------------------
CEconItemAttribute *CEconItemDefinition::IterateAttributes( string_t strClass )
{
	// Returning the first attribute found.
	for ( int i = 0; i < attributes.Count(); i++ )
	{
		CEconItemAttribute *pAttribute = &attributes[i];

		if ( pAttribute->m_strAttributeClass == strClass )
		{
			return pAttribute;
		}
	}

	return NULL;
}
