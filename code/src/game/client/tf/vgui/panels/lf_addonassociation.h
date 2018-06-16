//========= Copyright © 1996-2008, Valve Corporation, All rights reserved? ============//
//
// Edited ASW Addons Menu.
//
//=====================================================================================//
#ifndef __VADDONASSOCIATION_H__
#define __VADDONASSOCIATION_H__
#ifdef _WIN32
#pragma once
#endif

#include "gameui_util.h"

#include "tf_dialogpanelbase.h"
#include "controls/tf_advcheckbutton.h"
#include "controls/tf_advbutton.h"

class AddonAssociation : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE( AddonAssociation, CTFDialogPanelBase );
public:
	enum EAssociation
	{
		kAssociation_None,
		kAssociation_Other,
		kAssociation_Ok
	};
	
	AddonAssociation(vgui::Panel *parent, const char *panelName);
	~AddonAssociation();

	static EAssociation VPKAssociation();
	static bool CheckAndSeeIfShouldShow();

protected:
	virtual void OnCommand(const char *command);
	virtual void OnThink();

private:
	CTFAdvCheckButton *m_pDoNotAskForAssociation;

};

#endif // __VADDONASSOCIATION_H__
