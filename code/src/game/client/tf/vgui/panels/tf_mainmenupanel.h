#ifndef TFMAINMENUPANEL_H
#define TFMAINMENUPANEL_H

#include "tf_menupanelbase.h"
#include "steam/steam_api.h"
#include <vgui_controls/HTML.h>
#include "tf_dialogpanelbase.h"
#include "tf_controls.h"

class CAvatarImagePanel;
class CTFAdvButton;
class CTFBlogPanel;
class CTFAdvSlider;

enum MusicStatus
{
	MUSIC_STOP,
	MUSIC_FIND,
	MUSIC_PLAY,
	MUSIC_STOP_FIND,
	MUSIC_STOP_PLAY,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPanel, CTFMenuPanelBase);

public:
	CTFMainMenuPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMainMenuPanel();
	bool Init();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void Show();
	void Hide();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	void SetVersionLabel();
	void PlayMusic();
	void OnNotificationUpdate();
	void ShowBlogPanel(bool show);

private:
	void GetRandomMusic(char *pszBuf, int iBufLength);

	CExLabel			*m_pVersionLabel;
	CTFAdvButton		*m_pNotificationButton;
	CAvatarImagePanel	*m_pProfileAvatar; 
	vgui::ImagePanel	*m_pFakeBGImage;

	int					m_iShowFakeIntro;

	char				m_pzMusicLink[64];	
	int					m_nSongGuid;
	MusicStatus			m_psMusicStatus;

	CSteamID			m_SteamID;
	CTFBlogPanel		*m_pBlogPanel;

	static void ConfirmQuit();
};


class CTFBlogPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFBlogPanel, CTFMenuPanelBase);

public:
	CTFBlogPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFBlogPanel();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void LoadBlogPost(const char* URL);

private:
	vgui::HTML			*m_pHTMLPanel;
protected:
	CExRichText			*m_pInfo;
};

#endif // TFMAINMENUPANEL_H