//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Frame.h>
#include <vgui/IScheme.h>
#include <game/client/iviewport.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>
#include <filesystem.h>
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>

#include "vguicenterprint.h"
#include "tf_controls.h"
#include "basemodelpanel.h"
#include "tf_arenateammenu.h"
#include <convar.h>
#include "IGameUIFuncs.h" // for key bindings
#include "hud.h" // for gEngfuncs
#include "c_tf_player.h"
#include "tf_gamerules.h"
#include "c_team.h"
#include "tf_hud_notification_panel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFArenaTeamMenu::CTFArenaTeamMenu( IViewPort *pViewPort ) : CTeamMenu( pViewPort )
{
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetVisible(false);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	m_iTeamMenuKey = BUTTON_CODE_INVALID;

	m_pAutoTeamButton = new CTFTeamButton(this, "teambutton2");
	m_pSpecTeamButton = new CTFTeamButton(this, "teambutton3");
	m_pSpecLabel = new CExLabel(this, "TeamMenuSpectate", "");
	m_pCancelButton = new CExButton(this, "CancelButton", "#TF_Cancel");

	vgui::ivgui()->AddTickSignal(GetVPanel());
	LoadControlSettings("Resource/UI/HudArenaTeamMenu.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFArenaTeamMenu::~CTFArenaTeamMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("Resource/UI/HudArenaTeamMenu.res");

	Update();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::ShowPanel(bool bShow)
{
	if ( BaseClass::IsVisible() == bShow )
		return;

	if ( !C_TFPlayer::GetLocalTFPlayer() )
		return;

	if ( !gameuifuncs || !gViewPortInterface || !engine )
		return;

	if (bShow)
	{

			if (TFGameRules()->State_Get() == GR_STATE_TEAM_WIN &&
				C_TFPlayer::GetLocalTFPlayer() &&
				C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber() != TFGameRules()->GetWinningTeam()
				&& C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber() != TEAM_SPECTATOR
				&& C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber() != TEAM_UNASSIGNED)
			{
				SetVisible(false);
				SetMouseInputEnabled(false);

				CHudNotificationPanel *pNotifyPanel = GET_HUDELEMENT(CHudNotificationPanel);
				if (pNotifyPanel)
				{
					pNotifyPanel->SetupNotifyCustom("#TF_CantChangeTeamNow", "ico_notify_flag_moving", C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber());
				}

				return;
			}

			gViewPortInterface->ShowPanel(PANEL_CLASS_RED, false);
			gViewPortInterface->ShowPanel(PANEL_CLASS_BLUE, false);

			engine->CheckPoint("TeamMenu");

			Activate();
			SetMouseInputEnabled(true);

			// get key bindings if shown
			m_iTeamMenuKey = gameuifuncs->GetButtonCodeForBind("changeteam");
			m_iScoreBoardKey = gameuifuncs->GetButtonCodeForBind("showscores");

	}
	else
	{
			SetVisible(false);
			SetMouseInputEnabled(false);

			if (IsConsole())
			{
				// Close the door behind us
				CTFTeamButton *pButton = dynamic_cast< CTFTeamButton *> (GetFocusNavGroup().GetCurrentFocus());
				if (pButton)
				{
					pButton->OnCursorExited();
				}
			}
	}
}

//-----------------------------------------------------------------------------
// Purpose: called to update the menu with new information
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::Update(void)
{
	BaseClass::Update();

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	if (pLocalPlayer && (pLocalPlayer->GetTeamNumber() != TEAM_UNASSIGNED))
	{
		if (m_pCancelButton)
		{
			m_pCancelButton->SetVisible(true);
		}
	}
	else
	{
		if (m_pCancelButton && m_pCancelButton->IsVisible())
		{
			m_pCancelButton->SetVisible(false);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: chooses and loads the text page to display that describes mapName map
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::LoadMapPage(const char *mapName)
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::OnKeyCodePressed(KeyCode code)
{
	if ((m_iTeamMenuKey != BUTTON_CODE_INVALID && m_iTeamMenuKey == code) ||
		code == KEY_XBUTTON_BACK ||
		code == KEY_XBUTTON_B)
	{
		C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

		if (pLocalPlayer && (pLocalPlayer->GetTeamNumber() != TEAM_UNASSIGNED))
		{
			ShowPanel(false);
		}
	}
	else if (code == KEY_SPACE)
	{
		engine->ClientCmd("jointeam auto");

		ShowPanel(false);
		OnClose();
	}
	else if (code == KEY_XBUTTON_A || code == KEY_XBUTTON_RTRIGGER)
	{
		// select the active focus
		if (GetFocusNavGroup().GetCurrentFocus())
		{
			ipanel()->SendMessage(GetFocusNavGroup().GetCurrentFocus()->GetVPanel(), new KeyValues("PressButton"), GetVPanel());
		}
	}
	else if (code == KEY_XBUTTON_RIGHT || code == KEY_XSTICK1_RIGHT)
	{
		CTFTeamButton *pButton;

		pButton = dynamic_cast< CTFTeamButton *> (GetFocusNavGroup().GetCurrentFocus());
		if (pButton)
		{
			pButton->OnCursorExited();
			GetFocusNavGroup().RequestFocusNext(pButton->GetVPanel());
		}
		else
		{
			GetFocusNavGroup().RequestFocusNext(NULL);
		}

		pButton = dynamic_cast< CTFTeamButton * > (GetFocusNavGroup().GetCurrentFocus());
		if (pButton)
		{
			pButton->OnCursorEntered();
		}
	}
	else if (code == KEY_XBUTTON_LEFT || code == KEY_XSTICK1_LEFT)
	{
		CTFTeamButton *pButton;

		pButton = dynamic_cast< CTFTeamButton *> (GetFocusNavGroup().GetCurrentFocus());
		if (pButton)
		{
			pButton->OnCursorExited();
			GetFocusNavGroup().RequestFocusPrev(pButton->GetVPanel());
		}
		else
		{
			GetFocusNavGroup().RequestFocusPrev(NULL);
		}

		pButton = dynamic_cast< CTFTeamButton * > (GetFocusNavGroup().GetCurrentFocus());
		if (pButton)
		{
			pButton->OnCursorEntered();
		}
	}
	else
	{
		BaseClass::OnKeyCodePressed(code);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when the user picks a team
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::OnCommand(const char *command)
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	if (Q_stricmp(command, "vguicancel"))
	{
		// we're selecting a team, so make sure it's not the team we're already on before sending to the server
		if (pLocalPlayer && (Q_strstr(command, "jointeam ")))
		{
			const char *pTeam = command + Q_strlen("jointeam ");
			int iTeam = TEAM_INVALID;

			if (Q_stricmp(pTeam, "spectate") == 0)
			{
				iTeam = TEAM_SPECTATOR;
			}
			else if (Q_stricmp(pTeam, "red") == 0)
			{
				iTeam = TF_TEAM_RED;
			}

			// are we selecting the team we're already on?
			if (pLocalPlayer->GetTeamNumber() != iTeam)
			{
				engine->ClientCmd(command);
			}
		}
		else if (pLocalPlayer && (Q_strstr(command, "jointeam_nomenus ")))
		{
			engine->ClientCmd(command);
		}
	}

	BaseClass::OnCommand(command);
	ShowPanel(false);
	OnClose();
}

//-----------------------------------------------------------------------------
// Frame-based update
//-----------------------------------------------------------------------------
void CTFArenaTeamMenu::OnTick()
{

}
