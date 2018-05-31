//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef tf_cvarslider_H
#define tf_cvarslider_H
#ifdef _WIN32
#pragma once
#endif

//#include <vgui_controls/Slider.h>
#include "tf_advslider.h"

class CCvarSlider : public CTFAdvSlider
{
	DECLARE_CLASS_SIMPLE(CCvarSlider, CTFAdvSlider);

public:

	CCvarSlider(vgui::Panel *parent, const char *panelName, const char *name);
	CCvarSlider( vgui::Panel *parent, const char *panelName, char const *caption,
		float minValue, float maxValue, char const *cvarname, bool bShowFrac = false );
	~CCvarSlider();

	void			SetupSlider( float minValue, float maxValue, const char *cvarname );

	virtual void	ApplySettings( KeyValues *inResourceData );
	virtual void	GetSettings( KeyValues *outResourceData );

	virtual void	Paint();

	void			SetMinMaxValues( float minValue, float maxValue );

    void            Reset();
	void			ApplyChanges( void );

private:
	MESSAGE_FUNC( OnApplyChanges, "ApplyChanges" );
};

#endif // tf_cvarslider_H
