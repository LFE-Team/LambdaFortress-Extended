//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "tf_cvarslider.h"
#include <stdio.h>
//#include "EngineInterface.h"
#include "tier1/KeyValues.h"
#include "tier1/convar.h"
#include <vgui/IVGui.h>
#include <vgui_controls/PropertyPage.h>

#define tf_cvarslider_SCALE_FACTOR 1.0f

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//DECLARE_BUILD_FACTORY( CCvarSlider );
vgui::Panel *CCvarSlider_Factory()
{
	return new CCvarSlider(NULL, NULL, "CCvarSlider");
}
DECLARE_BUILD_FACTORY_CUSTOM(CCvarSlider, CCvarSlider_Factory);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::CCvarSlider(Panel *parent, const char *panelName, const char *name) : CTFAdvSlider(parent, panelName, name)
{
	SetupSlider( 0, 1, "" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::CCvarSlider( Panel *parent, const char *panelName, char const *caption,
	float minValue, float maxValue, char const *cvarname, bool bShowFrac )
		: CTFAdvSlider(parent, panelName, caption)
{
	m_bShowFrac = bShowFrac;
	SetupSlider( minValue, maxValue, cvarname );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::~CCvarSlider()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::SetupSlider( float minValue, float maxValue, const char *cvarname )
{
	AddActionSignalTarget( this );

	m_flMinValue = minValue;
	m_flMaxValue = maxValue;

	SetMinMax(minValue, maxValue);

	SetCommandString( cvarname );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
}

//-----------------------------------------------------------------------------
// Purpose: Get control settings for editing
//-----------------------------------------------------------------------------
void CCvarSlider::GetSettings( KeyValues *outResourceData )
{
	BaseClass::GetSettings(outResourceData);
}

void CCvarSlider::SetMinMaxValues( float minValue, float maxValue )
{
	SetMinMax(minValue, maxValue);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::Paint()
{
	BaseClass::Paint();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::Reset()
{
	// Set slider to current value
//	m_fStartValue = engine->pfnGetCvarFloat( m_szCvarName );
	ConVarRef var( GetCommandString() );
	if ( !var.IsValid() )
	{
		SetValue( 0 );
		return;
	}

	SetValue( var.GetFloat() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::OnApplyChanges( void )
{
	ConVarRef var( GetCommandString() );
	if ( var.IsValid() )
	{
		var.SetValue( GetFinalValue() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::ApplyChanges( void )
{
	OnApplyChanges();
}
