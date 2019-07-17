#ifndef TFMAINMENUPAUSEPANEL_H
#define TFMAINMENUPAUSEPANEL_H

#include "tf_menupanelbase.h"
#include "tf_mainmenupanel.h"
#include "steam/steam_api.h"
#include <vgui_controls/HTML.h>

class CAvatarImagePanel;
class CTFAdvButton;
class CTFBlogPanel;
class CTFAdvSlider;

enum PauseMusicStatus
{
	PAUSE_MUSIC_STOP,
	PAUSE_MUSIC_FIND,
	PAUSE_MUSIC_PLAY,
	PAUSE_MUSIC_STOP_FIND,
	PAUSE_MUSIC_STOP_PLAY,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFPauseMenuPanel : public CTFMenuPanelBase
{
	friend class CTFBlogPanel;
	friend class CTFServerlistPanel;

	DECLARE_CLASS_SIMPLE(CTFPauseMenuPanel, CTFMenuPanelBase);

public:
	CTFPauseMenuPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFPauseMenuPanel();
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
	void OnNotificationUpdate();
	void SetVersionLabel();
	void PlayMusic();
	void ShowBlogPanel(bool show);

private:
	void GetRandomMusic(char *pszBuf, int iBufLength);

	CExLabel			*m_pVersionLabel;
	CTFAdvButton		*m_pNotificationButton;
	CAvatarImagePanel	*m_pProfileAvatar; 

	char				m_pzMusicLink[64];	
	int					m_nSongGuid;
	PauseMusicStatus	m_psMusicStatus;

	CSteamID			m_SteamID;
	CTFBlogPanel		*m_pBlogPanel;

	static void ConfirmQuit();
	static void ConfirmDisconnect();
};
#endif // TFMAINMENUPAUSEPANEL_H