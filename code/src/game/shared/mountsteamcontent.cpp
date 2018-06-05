//============================ Source-PlusPlus =============================//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "SteamCommon.h"
#ifdef CLIENT_DLL
#include "clientsteamcontext.h"
#endif
#include "filesystem.h"
#include "fmtstr.h"

#include "mountsteamcontent.h"

#include "ienginevgui.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include "icommandline.h"
#include "tier3/tier3.h"

void AddEP2(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/ep2/ep2_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/ep2/ep2_sound_vo_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/ep2", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/ep2_%language%.txt");
}

void AddCSS(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/cstrike/cstrike_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/cstrike/cstrike_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/cstrike", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/cstrike_%language%.txt");
}

void AddLostCoast(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/lostcoast/lostcoast_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/lostcoast/lostcoast_sound_vo_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/lostcoast", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/lostcoast_%language%.txt");
}

void AddPortal(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/portal/portal_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/portal/portal_sound_vo_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/portal", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/portal_%language%.txt");
}

void AddEP1(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/episodic/ep1_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/episodic/ep1_sound_vo_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/episodic", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/episodic_%language%.txt");
}

void AddHL2(const char* path)
{
	// Crashes for some reason...
	// We will keep this always in gameinfo for now...
	/*filesystem->AddSearchPath(CFmtStr("%s/hl2/hl2_misc.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2/hl2_sound_misc.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2/hl2_textures.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2/hl2_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2/hl2_sound_vo_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2", path), "GAME", PATH_ADD_TO_HEAD);*/
	g_pVGuiLocalize->AddFile("resource/hl2_%language%.txt");
}

void AddHL2MP(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/hl2mp/hl2mp_pak.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2mp/hl2mp_english.vpk", path), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/hl2mp", path), "GAME", PATH_ADD_TO_HEAD);
	g_pVGuiLocalize->AddFile("resource/hl2mp_%language%.txt");
}

void AddPlatform(const char* path)
{
	filesystem->AddSearchPath(CFmtStr("%s/platform", path), "PLATFORM", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/platform/platform_misc.vpk", path), "PLATFORM", PATH_ADD_TO_HEAD);
}

void MountExtraContent()
{
	KeyValuesAD gameinfo("GameInfo");
	gameinfo->LoadFromFile(filesystem, "gameinfo.txt");

	if (steamapicontext->SteamApps()->BIsAppInstalled(243730))
	{
		char sdk2013SPPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(243730, sdk2013SPPath, sizeof(sdk2013SPPath));

		if (gameinfo->GetBool("hl2content") || gameinfo->GetBool("ep1content") || gameinfo->GetBool("ep2content"))
			AddHL2(sdk2013SPPath);

		if (gameinfo->GetBool("ep2content") || gameinfo->GetBool("ep1content"))
			AddEP1(sdk2013SPPath);

		if (gameinfo->GetBool("ep2content"))
			AddEP2(sdk2013SPPath);

		if (gameinfo->GetBool("lostcoastcontent"))
			AddLostCoast(sdk2013SPPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(243750) && gameinfo->GetBool("hl2mpcontent"))
	{
		char sdk2013MPPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(243750, sdk2013MPPath, sizeof(sdk2013MPPath));
		AddHL2(sdk2013MPPath);
		AddHL2MP(sdk2013MPPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(220) && (gameinfo->GetBool("hl2content") || gameinfo->GetBool("ep1content") || gameinfo->GetBool("ep2content")))
	{
		char hl2Path[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(220, hl2Path, sizeof(hl2Path));
		AddHL2(hl2Path);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(320) && gameinfo->GetBool("hl2mpcontent"))
	{
		char hl2mpPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(320, hl2mpPath, sizeof(hl2mpPath));
		AddHL2(hl2mpPath);
		AddHL2MP(hl2mpPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(380) && (gameinfo->GetBool("ep1content") || gameinfo->GetBool("ep2content")))
	{
		char ep1Path[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(380, ep1Path, sizeof(ep1Path));
		AddEP1(ep1Path);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(420) && gameinfo->GetBool("ep2content"))
	{
		char ep2Path[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(420, ep2Path, sizeof(ep2Path));
		AddEP2(ep2Path);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(340) && gameinfo->GetBool("lostcoastcontent"))
	{
		char lostCoastPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(340, lostCoastPath, sizeof(lostCoastPath));
		AddLostCoast(lostCoastPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(400) && gameinfo->GetBool("portalcontent"))
	{
		char portalPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(400, portalPath, sizeof(portalPath));
		AddPortal(portalPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(240) && gameinfo->GetBool("csscontent"))
	{
		char cssPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(240, cssPath, sizeof(cssPath));
		AddCSS(cssPath);
	}

	if (steamapicontext->SteamApps()->BIsAppInstalled(243750))
	{
		char sdk2013MPPath[MAX_PATH];
		steamapicontext->SteamApps()->GetAppInstallDir(243750, sdk2013MPPath, sizeof(sdk2013MPPath));
		AddPlatform(sdk2013MPPath);
	}

	filesystem->AddSearchPath(CFmtStr("%s", CommandLine()->ParmValue("-game")), "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s", CommandLine()->ParmValue("-game")), "MOD", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s", CommandLine()->ParmValue("-game")), "MOD_WRITE", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s", CommandLine()->ParmValue("-game")), "DEFAULT_WRITE_PATH", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath(CFmtStr("%s/bin", CommandLine()->ParmValue("-game")), "GAMEBIN", PATH_ADD_TO_HEAD);
}