#include "cbase.h"
#include "tf_mainmenupanel.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advslider.h"
#include "vgui_controls/SectionedListPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_notificationmanager.h"
#include "engine/IEngineSound.h"
#include "vgui_avatarimage.h"
#include "tf_gamerules.h"
#include <KeyValues.h>
#include "panels/lfe_genericconfirmation.h"
#include "panels/tf_optionsdialog.h"
#include "tier0/icommandline.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define APPID_TF2 440

#define BLOG_URL ""
//#define BLOG_URL "https://discordapp.com/channels/427017307835858944/449233158546784266/?nolinks=1&noheader=1&nofooter=1&fillwrapper=1"
// don't use discord link because it can fuck up the entire game
static void OnBlogToggle(IConVar *var, const char *pOldValue, float flOldValue)
{
	GET_MAINMENUPANEL(CTFMainMenuPanel)->ShowBlogPanel(((ConVar*)var)->GetBool());
}
ConVar lfe_mainmenu_music("lfe_mainmenu_music", "1", FCVAR_ARCHIVE, "Toggle music in the main menu");
ConVar lfe_mainmenu_showblog("lfe_mainmenu_showblog", "0", FCVAR_ARCHIVE, "Toggle blog in the main menu", OnBlogToggle);

ConVar lfe_ui_confirmation_quit("lfe_ui_confirmation_quit", "1", FCVAR_ARCHIVE, "Toggle quit confirmation");
ConVar lfe_ui_confirmation_disconnect("lfe_ui_confirmation_disconnect", "1", FCVAR_ARCHIVE, "Toggle disconnect confirmation");

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::CTFMainMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::~CTFMainMenuPanel()
{

}

bool CTFMainMenuPanel::Init()
{
	BaseClass::Init();

	m_psMusicStatus = MUSIC_FIND;
	m_pzMusicLink[0] = '\0';
	m_nSongGuid = 0;

	if (steamapicontext->SteamUser())
	{
		m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	}

	m_iShowFakeIntro = 4;
	m_pVersionLabel = NULL;
	m_pNotificationButton = NULL;
	m_pProfileAvatar = NULL;
	m_pFakeBGImage = NULL;
	m_pBlogPanel = new CTFBlogPanel(this, "BlogPanel");

	bInMenu = true;
	bInGame = false;
	return true;
}


void CTFMainMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	KeyValues *pConditions = NULL;
	if ( TFGameRules() && TFGameRules()->IsHalloween() )
	{
		pConditions = new KeyValues( "conditions" );
		AddSubKeyNamed( pConditions, "if_halloween" );
	}
	else if ( TFGameRules() && TFGameRules()->IsSmissmas() )
	{
		pConditions = new KeyValues( "conditions" );
		AddSubKeyNamed( pConditions, "if_smissmas" );
	}

	LoadControlSettings( "resource/UI/main_menu/MainMenuPanel.res", NULL, NULL, pConditions );

	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel"));
	m_pNotificationButton = dynamic_cast<CTFAdvButton *>(FindChildByName("NotificationButton"));
	m_pProfileAvatar = dynamic_cast<CAvatarImagePanel *>(FindChildByName("AvatarImage"));
	m_pFakeBGImage = dynamic_cast<vgui::ImagePanel *>(FindChildByName("FakeBGImage"));

	SetVersionLabel();

	if ( pConditions )
		pConditions->deleteThis();
}	

void CTFMainMenuPanel::PerformLayout()
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

	ShowBlogPanel(lfe_mainmenu_showblog.GetBool() || GetNotificationManager()->IsOutdated());
	OnNotificationUpdate();
	AutoLayout();

	if (m_iShowFakeIntro > 0)
	{
		char szBGName[128];
		engine->GetMainMenuBackgroundName(szBGName, sizeof(szBGName));
		char szImage[128];
		Q_snprintf(szImage, sizeof(szImage), "../console/%s", szBGName);
		int width, height;
		surface()->GetScreenSize(width, height);
		float fRatio = (float)width / (float)height;
		bool bWidescreen = (fRatio < 1.5 ? false : true);
		if (bWidescreen)
			Q_strcat(szImage, "_widescreen", sizeof(szImage));
		m_pFakeBGImage->SetImage(szImage);
		m_pFakeBGImage->SetVisible(true);
		m_pFakeBGImage->SetAlpha(255);
	}
};

void CTFMainMenuPanel::ShowBlogPanel(bool show)
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

void CTFMainMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		if ( lfe_ui_confirmation_quit.GetBool() )
		{
			CTFGenericConfirmation* confirmation = GET_MAINMENUPANEL(CTFGenericConfirmation);

			CTFGenericConfirmation::Data_t data;

			data.pWindowTitle = "#MMenu_PromptQuit_Title";
			data.pMessageText = "#MMenu_PromptQuit_Body";

			data.bOkButtonEnabled = true;
			data.pfnOkCallback = &ConfirmQuit;
			data.pOkButtonText = "#TF_Quit_Title";
			data.bCancelButtonEnabled = true;

			confirmation->SetUsageData(data);
			MAINMENU_ROOT->ShowPanel(CONFIRMATION_MENU);

			if ( UI_IsDebug() )
				Msg( "[GAMEUI] MAIN_MENU + CONFIRMATION_MENU\n");
		}
		else
		{
			engine->ClientCmd( "quit" );
		}
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		MAINMENU_ROOT->ShowPanel(OPTIONSDIALOG_MENU);
		GET_MAINMENUPANEL(CTFOptionsDialog)->SetCurrentPanel(OPTION_PANEL_ADV);

		if ( UI_IsDebug() )
			Msg( "[GAMEUI] MAIN_MENU -> OPTIONSDIALOG_MENU\n");
	}
	else if (!Q_strcmp(command, "newcreategame"))
	{
		MAINMENU_ROOT->ShowPanel(CREATESERVER_MENU);

		if ( UI_IsDebug() )
			Msg( "[GAMEUI] MAIN_MENU -> CREATESERVER_MENU\n");
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		MAINMENU_ROOT->ShowPanel(LOADOUT_MENU);

		if ( UI_IsDebug() )
			Msg( "[GAMEUI] MAIN_MENU -> LOADOUT_MENU\n");
	}
	else if (!Q_strcmp(command, "newstats"))
	{
		MAINMENU_ROOT->ShowPanel(STATSUMMARY_MENU);

		if ( UI_IsDebug() )
			Msg( "[GAMEUI] MAIN_MENU -> STATSUMMARY_MENU\n");
	}
	else if (!Q_strcmp(command, "newcredits"))
	{
		MAINMENU_ROOT->ShowPanel(CREDIT_MENU);

		if ( UI_IsDebug() )
			Msg( "[GAMEUI] MAIN_MENU -> CREDIT_MENU\n");
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

void CTFMainMenuPanel::OnTick()
{
	BaseClass::OnTick();

	if ( lfe_mainmenu_music.GetBool() && !bInGameLayout && !CommandLine()->FindParm( "-nostartupsound" ) )
	{
		if ((m_psMusicStatus == MUSIC_FIND || m_psMusicStatus == MUSIC_STOP_FIND) && !enginesound->IsSoundStillPlaying(m_nSongGuid))
		{
			GetRandomMusic(m_pzMusicLink, sizeof(m_pzMusicLink));
			m_psMusicStatus = MUSIC_PLAY;
		}
		else if ((m_psMusicStatus == MUSIC_PLAY || m_psMusicStatus == MUSIC_STOP_PLAY)&& m_pzMusicLink[0] != '\0')
		{
			enginesound->StopSoundByGuid(m_nSongGuid);
			ConVar *snd_musicvolume = cvar->FindVar("snd_musicvolume");
			float fVolume = (snd_musicvolume ? snd_musicvolume->GetFloat() : 1.0f);
			enginesound->EmitAmbientSound(m_pzMusicLink, fVolume, PITCH_NORM, 0);			
			m_nSongGuid = enginesound->GetGuidForLastSoundEmitted();
			m_psMusicStatus = MUSIC_FIND;
		}
	}
	else if (m_psMusicStatus == MUSIC_FIND)
	{
		enginesound->StopSoundByGuid(m_nSongGuid);
		m_psMusicStatus = (m_nSongGuid == 0 ? MUSIC_STOP_FIND : MUSIC_STOP_PLAY);
	}
};

void CTFMainMenuPanel::OnThink()
{
	BaseClass::OnThink();

	if (m_iShowFakeIntro > 0)
	{
		m_iShowFakeIntro--;
		if (m_iShowFakeIntro == 0)
		{
			vgui::GetAnimationController()->RunAnimationCommand(m_pFakeBGImage, "Alpha", 0, 1.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
		}
	}	
	if (m_pFakeBGImage->IsVisible() && m_pFakeBGImage->GetAlpha() == 0)
	{
		m_pFakeBGImage->SetVisible(false);
	}

	// check if user had required game
	if ( steamapicontext->SteamApps() )
	{
		if ( !steamapicontext->SteamApps()->BIsAppInstalled( APPID_TF2 ) )
		{
			CTFGenericConfirmation* confirmation = GET_MAINMENUPANEL(CTFGenericConfirmation);
			CTFGenericConfirmation::Data_t data;

			data.pWindowTitle = "#LFE_Warning_Title";
			data.pMessageText = "#LFE_Warning_InstallTF2";
			data.bOkButtonEnabled = true;
			data.pfnOkCallback = &ConfirmQuit;
			data.pOkButtonText = "#TF_Quit_Title";
			data.bCancelButtonEnabled = true;
			data.pCancelButtonText = "#LFE_Ignore";

			confirmation->SetUsageData(data);
			MAINMENU_ROOT->ShowPanel(CONFIRMATION_MENU);
		}
	}

	// hide all panel
	//MAINMENU_ROOT->HidePanel(SHADEBACKGROUND_MENU);
	MAINMENU_ROOT->HidePanel(LOADOUT_MENU);
	/*MAINMENU_ROOT->HidePanel(NOTIFICATION_MENU);
	MAINMENU_ROOT->HidePanel(CREDIT_MENU);
	MAINMENU_ROOT->HidePanel(OPTIONSDIALOG_MENU);
	MAINMENU_ROOT->HidePanel(CREATESERVER_MENU);
	MAINMENU_ROOT->HidePanel(STATSUMMARY_MENU);
	MAINMENU_ROOT->HidePanel(TOOLTIP_MENU);
	MAINMENU_ROOT->HidePanel(ITEMTOOLTIP_MENU);

	// always show blog
	//ShowBlogPanel();*/
};

void CTFMainMenuPanel::Show()
{
	BaseClass::Show();
	//vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
};

void CTFMainMenuPanel::Hide()
{
	BaseClass::Hide();
	//vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};


void CTFMainMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
	ShowBlogPanel(lfe_mainmenu_showblog.GetBool());
};

void CTFMainMenuPanel::GameLayout()
{
	BaseClass::GameLayout();
};

void CTFMainMenuPanel::PlayMusic()
{

}

void CTFMainMenuPanel::OnNotificationUpdate()
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
	if (GetNotificationManager()->IsOutdated())
	{
		if (m_pVersionLabel)
		{
			m_pVersionLabel->SetFgColor(Color(255, 20, 50, 100));
		}
	}
};

void CTFMainMenuPanel::SetVersionLabel()  //GetVersionString
{
	if (m_pVersionLabel)
	{
		char verString[64];
		Q_snprintf(verString, sizeof(verString), "Version: %s", GetNotificationManager()->GetVersionName());
		m_pVersionLabel->SetText(verString);
	}
};

void CTFMainMenuPanel::GetRandomMusic( char *pszBuf, int iBufLength )
{
	Assert( iBufLength );

	char szPath[MAX_PATH];

	// Check that there's music available
	if ( !g_pFullFileSystem->FileExists( "sound/ui/gamestartup1.mp3", "MOD" ) )
	{
		Assert(false);
		*pszBuf = '\0';
	}

	// Discover tracks, 1 through n
	int iLastTrack = 0;
	do
	{
		Q_snprintf( szPath, sizeof( szPath ), "sound/ui/gamestartup%d.mp3", ++iLastTrack );
	} while ( g_pFullFileSystem->FileExists( szPath, "MOD" ) );

	// Pick a random one
	Q_snprintf( szPath, sizeof( szPath ), "ui/gamestartup%d.mp3", RandomInt( 1, iLastTrack - 1 ) );
	Q_strncpy( pszBuf, szPath, iBufLength );
}

void CTFMainMenuPanel::ConfirmQuit()
{
	engine->ClientCmd( "quit" );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFBlogPanel::CTFBlogPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	m_pHTMLPanel = new vgui::HTML(this, "HTMLPanel");
	m_pInfo = new CExRichText( this, "InfoText" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFBlogPanel::~CTFBlogPanel()
{
}

void CTFBlogPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings( "resource/UI/main_menu/BlogPanel.res" );
}

void CTFBlogPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	LoadBlogPost(BLOG_URL);
}

void CTFBlogPanel::LoadBlogPost(const char* URL)
{
	if (m_pHTMLPanel)
	{
		m_pHTMLPanel->SetVisible(true);
		m_pHTMLPanel->OpenURL(URL, NULL);
	}
}