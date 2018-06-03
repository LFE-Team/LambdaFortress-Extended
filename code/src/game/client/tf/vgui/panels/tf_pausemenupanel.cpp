#include "cbase.h"
#include "tf_pausemenupanel.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advslider.h"
#include "vgui_controls/SectionedListPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_notificationmanager.h"
#include "engine/IEngineSound.h"
#include "vgui_avatarimage.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BLOG_URL "https://www.reddit.com/r/tf2/?nolinks=1&noheader=1&nofooter=1&fillwrapper=1"

static void OnPauseBlogToggle(IConVar *var, const char *pOldValue, float flOldValue)
{
	GET_MAINMENUPANEL(CTFPauseMenuPanel)->ShowBlogPanel(((ConVar*)var)->GetBool());
}
ConVar lf_pausemenu_music("lf_pausemenu_music", "0", FCVAR_ARCHIVE, "Toggle music in the pause menu");
ConVar lf_pausemenu_showblog("lf_pausemenu_showblog", "0", FCVAR_ARCHIVE, "Toggle blog in the main menu", OnPauseBlogToggle);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFPauseMenuPanel::CTFPauseMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFPauseMenuPanel::~CTFPauseMenuPanel()
{

}

bool CTFPauseMenuPanel::Init()
{
	BaseClass::Init();

	m_psMusicStatus = PAUSE_MUSIC_FIND;
	m_pzMusicLink[0] = '\0';
	m_nSongGuid = 0;

	if (steamapicontext->SteamUser())
	{
		m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	}

	m_pVersionLabel = NULL;
	m_pNotificationButton = NULL;
	m_pProfileAvatar = NULL;
	m_pBlogPanel = new CTFBlogPanel(this, "BlogPanel");
	m_pServerlistPanel = new CTFServerlistPanel(this, "ServerlistPanel");

	m_pNotificationButton = NULL;
	bInMenu = false;
	bInGame = true;
	return true;
};


void CTFPauseMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/PauseMenuPanel.res");
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel"));
	m_pNotificationButton = dynamic_cast<CTFAdvButton *>(FindChildByName("NotificationButton"));
	m_pProfileAvatar = dynamic_cast<CAvatarImagePanel *>(FindChildByName("AvatarImage"));

	SetVersionLabel();
}

void CTFPauseMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	if (m_pProfileAvatar)
	{
		m_pProfileAvatar->SetPlayer(m_SteamID, k_EAvatarSize64x64);
		m_pProfileAvatar->SetShouldDrawFriendIcon(false);
	}

	char szNickName[64];
	Q_snprintf(szNickName, sizeof(szNickName),
		(steamapicontext->SteamFriends() ? steamapicontext->SteamFriends()->GetPersonaName() : "Unknown"));
	SetDialogVariable("nickname", szNickName);

	ShowBlogPanel(lf_pausemenu_showblog.GetBool() || GetNotificationManager()->IsOutdated());
	OnNotificationUpdate();
	AutoLayout();
};

void CTFPauseMenuPanel::ShowBlogPanel(bool show)
{
	if (m_pBlogPanel)
	{
		m_pBlogPanel->SetVisible(show);
		if (show)
		{
			m_pBlogPanel->LoadBlogPost(BLOG_URL);
		}
	}
}

void CTFPauseMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		MAINMENU_ROOT->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		MAINMENU_ROOT->ShowPanel(OPTIONSDIALOG_MENU);
	}
	else if (!Q_strcmp(command, "newcreategame"))
	{
		MAINMENU_ROOT->ShowPanel(CREATESERVER_MENU);
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		MAINMENU_ROOT->ShowPanel(LOADOUT_MENU);
	}
	else if (!Q_strcmp(command, "newstats"))
	{
		MAINMENU_ROOT->ShowPanel(STATSUMMARY_MENU);
	}
	else if (!Q_strcmp(command, "checkversion"))
	{
		//MAINMENU_ROOT->CheckVersion();
	}
	else if (!Q_strcmp(command, "shownotification"))
	{
		if (m_pNotificationButton)
		{
			m_pNotificationButton->SetGlowing(false);
		}
		MAINMENU_ROOT->ShowPanel(NOTIFICATION_MENU);
	}
	else if (!Q_strcmp(command, "testnotification"))
	{
		wchar_t resultString[128];
		V_snwprintf(resultString, sizeof(resultString), L"test %d", GetNotificationManager()->GetNotificationsCount());
		MessageNotification Notification(L"Yoyo", resultString, time( NULL ) );
		GetNotificationManager()->SendNotification(Notification);
	}
	else if (!Q_strcmp(command, "randommusic"))
	{
		enginesound->StopSoundByGuid(m_nSongGuid);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFPauseMenuPanel::OnTick()
{
	BaseClass::OnTick();

	if (lf_pausemenu_music.GetBool() && bInGameLayout)
	{
		if ((m_psMusicStatus == PAUSE_MUSIC_FIND || m_psMusicStatus == PAUSE_MUSIC_STOP_FIND) && !enginesound->IsSoundStillPlaying(m_nSongGuid))
		{
			GetRandomMusic(m_pzMusicLink, sizeof(m_pzMusicLink));
			m_psMusicStatus = PAUSE_MUSIC_PLAY;
		}
		else if ((m_psMusicStatus == PAUSE_MUSIC_PLAY || m_psMusicStatus == PAUSE_MUSIC_STOP_PLAY)&& m_pzMusicLink[0] != '\0')
		{
			enginesound->StopSoundByGuid(m_nSongGuid);
			ConVar *snd_musicvolume = cvar->FindVar("snd_musicvolume");
			float fVolume = (snd_musicvolume ? snd_musicvolume->GetFloat() : 1.0f);
			enginesound->EmitAmbientSound(m_pzMusicLink, fVolume, PITCH_NORM, 0);			
			m_nSongGuid = enginesound->GetGuidForLastSoundEmitted();
			m_psMusicStatus = PAUSE_MUSIC_FIND;
		}
	}
	else if (m_psMusicStatus == PAUSE_MUSIC_FIND)
	{
		enginesound->StopSoundByGuid(m_nSongGuid);
		m_psMusicStatus = (m_nSongGuid == 0 ? PAUSE_MUSIC_STOP_FIND : PAUSE_MUSIC_STOP_PLAY);
	}
};

void CTFPauseMenuPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFPauseMenuPanel::OnNotificationUpdate()
{
	if (m_pNotificationButton)
	{
		if (GetNotificationManager()->GetNotificationsCount() > 0)
		{
			m_pNotificationButton->SetVisible(true);
		}
		else
		{
			m_pNotificationButton->SetVisible(false);
		}

		if (GetNotificationManager()->GetUnreadNotificationsCount() > 0)
		{
			m_pNotificationButton->SetGlowing(true);
		}
		else
		{
			m_pNotificationButton->SetGlowing(false);
		}
	}
};

void CTFPauseMenuPanel::Show()
{
	BaseClass::Show();
	//vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
};

void CTFPauseMenuPanel::Hide()
{
	BaseClass::Hide();
	//vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFPauseMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFPauseMenuPanel::GameLayout()
{
	BaseClass::GameLayout();
	ShowBlogPanel(lf_pausemenu_showblog.GetBool());
};

void CTFPauseMenuPanel::PlayMusic()
{

}

void CTFPauseMenuPanel::SetVersionLabel()  //GetVersionString
{
	if (m_pVersionLabel)
	{
		char verString[64];
		Q_snprintf(verString, sizeof(verString), "Version: %s", GetNotificationManager()->GetVersionName());
		m_pVersionLabel->SetText(verString);
	}
};

void CTFPauseMenuPanel::GetRandomMusic(char *pszBuf, int iBufLength)
{
	Assert(iBufLength);

	char szPath[MAX_PATH];

	// Check that there's music available
	if (!g_pFullFileSystem->FileExists("sound/ui/gamestartup1.mp3"))
	{
		Assert(false);
		*pszBuf = '\0';
	}

	// Discover tracks, 1 through n
	int iLastTrack = 0;
	do
	{
		Q_snprintf(szPath, sizeof(szPath), "sound/ui/gamestartup%d.mp3", ++iLastTrack);
	} while (g_pFullFileSystem->FileExists(szPath));

	// Pick a random one
	Q_snprintf(szPath, sizeof(szPath), "ui/gamestartup%d.mp3", RandomInt(1, iLastTrack - 1));
	Q_strncpy(pszBuf, szPath, iBufLength);
}

void CTFPauseMenuPanel::SetServerlistSize(int size)
{
	m_pServerlistPanel->SetServerlistSize(size);
}

void CTFPauseMenuPanel::UpdateServerInfo()
{
	m_pServerlistPanel->UpdateServerInfo();
}