//========= Copyright © 1996-2008, Valve Corporation, All rights reserved? ============//
//
// Edited ASW Addons Menu.
//
//=====================================================================================//
#ifndef __VADDONS_H__
#define __VADDONS_H__
#ifdef _WIN32
#pragma once
#endif

#include "VGenericPanelList.h"
#include "gameui_util.h"

#include "tf_dialogpanelbase.h"
#include <vgui/KeyCode.h>

#include "controls/tf_advcheckbutton.h"
#include "controls/tf_advbutton.h"

class CNB_Button;
class CNB_Header_Footer;

//class AddonGenericPanelList;

class AddonListItem : public vgui::EditablePanel, IGenericPanelListItem
{
	DECLARE_CLASS_SIMPLE( AddonListItem, vgui::EditablePanel );

public:
	AddonListItem(vgui::Panel *parent, const char *panelName);

	void SetAddonName( const wchar_t *pcName );
	void SetAddonType( const wchar_t* pcType );
	void SetAddonEnabled( bool bEnabled );
	bool GetAddonEnabled( );

	// Inherited from IGenericPanelListItem
	virtual bool IsLabel() { return false; }
	void OnMousePressed( vgui::MouseCode code );
	virtual void OnMessage(const KeyValues *params, vgui::VPANEL ifromPanel);
	virtual void Paint();

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	vgui::Label* m_LblName;
	vgui::Label* m_LblType;
	vgui::IBorder* m_DefaultBorder;
	vgui::IBorder* m_FocusBorder;
	vgui::CheckButton* m_BtnEnabled;
	bool m_bCurrentlySelected;
	vgui::HFont	m_hTextFont;

	CPanelAnimationVarAliasType( float, m_flDetailsExtraHeight, "DetailsExtraHeight", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDetailsRowHeight, "DetailsRowHeight", "0", "proportional_float" );
};

class Addons : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE( Addons, CTFDialogPanelBase );

public:
	Addons(vgui::Panel *parent, const char *panelName);
	~Addons();
	void Activate();
	void PaintBackground( void );
	void OnCommand(const char *command);
	virtual void OnMessage(const KeyValues *params, vgui::VPANEL ifromPanel);

	virtual void OnThink();

protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	bool LoadAddonListFile( KeyValues *&pAddons );
	bool LoadAddonInfoFile( KeyValues *&pAddonInfo, const char *pcAddonDir, bool bIsVPK );
	void SetDetailsUIForAddon( int nIndex );
	void GetAddonImage( const char *pcAddonDir, char *pcImagePath, int nImagePathSize, bool bIsVPK );
	void ExtractAddonMetadata( const char *pcAddonDir );

private:

	GenericPanelList* m_GplAddons;
	vgui::Label *m_LblName;
	vgui::Label *m_LblNoAddons;
	vgui::Label *m_LblType;
	vgui::Label *m_LblAuthor;
	vgui::Label *m_LblDescription;
	vgui::ImagePanel *m_ImgAddonIcon;
	CNB_Header_Footer *m_pHeaderFooter;
	CTFAdvCheckButton *m_pDoNotAskForAssociation;

	vgui::EditablePanel *m_pSupportRequiredPanel;
	vgui::EditablePanel *m_pInstallingSupportPanel;
	
	struct AddonInfo
	{
		char szDirectory[60];
		wchar_t szName[120];
		wchar_t szAuthor[120];
		wchar_t szDescription[1024];
		bool bEnabled;
		wchar_t szTypes[60];
		char szImageName[MAX_PATH];
	};

	enum ConversionErrorType
	{
		CE_SUCCESS,
		CE_MEMORY_ERROR,
		CE_CANT_OPEN_SOURCE_FILE,
		CE_ERROR_PARSING_SOURCE,
		CE_SOURCE_FILE_FORMAT_NOT_SUPPORTED,
		CE_ERROR_WRITING_OUTPUT_FILE,
		CE_ERROR_LOADING_DLL
	};

	KeyValues *m_pAddonList;
	CUtlVector<Addons::AddonInfo> m_addonInfoList;
	ConversionErrorType SConvertJPEGToTGA(const char *jpegpath, const char *tgaPath );
	ConversionErrorType ConvertTGAToVTF(const char *tgaPath );
	ConversionErrorType WriteVMT(const char *vtfPath, const char *pcAddonDir );
};
#endif