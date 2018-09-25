//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: Vote issues
//
//
//=============================================================================//
#include "cbase.h"
#include "tf_shareddefs.h"
#include "tf_voteissues.h"

extern ConVar sv_vote_timer_duration;

ConVar sv_vote_issue_kick_allowed( "sv_vote_issue_kick_allowed", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Can players call votes to kick players from the server?" );
ConVar sv_vote_issue_restart_game_allowed( "sv_vote_issue_restart_game_allowed", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Can players call votes to restart the game?" );
ConVar sv_vote_issue_restart_game_cooldown( "sv_vote_issue_restart_game_cooldown", "300", FCVAR_REPLICATED, "Minimum time before another restart vote can occur (in seconds)." );
ConVar sv_vote_issue_changelevel_allowed( "sv_vote_issue_changelevel_allowed", "1", FCVAR_NOTIFY | FCVAR_REPLICATED,  "Can players call votes to change levels?" );
ConVar sv_vote_issue_change_difficulty_allowed( "sv_vote_issue_change_difficulty_allowed", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Can players call votes to change difficulty?" );
ConVar sv_vote_issue_change_difficulty_cooldown( "sv_vote_issue_change_difficulty_cooldown", "150", FCVAR_REPLICATED, "Minimum time before another change difficulty vote can occur (in seconds)." );

//-----------------------------------------------------------------------------
// Purpose: Kick Issue
//-----------------------------------------------------------------------------
CKickIssue::CKickIssue(const char *typeString) : CBaseIssue(typeString)
{
	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), "");
	Q_snprintf(m_pzReason, sizeof(m_pzReason), "");
	m_iPlayerID = 0;
}

CKickIssue::~CKickIssue()
{
}

const char *CKickIssue::GetDisplayString()
{
	char result[64];
	Q_snprintf(result, sizeof(result), "#TF_vote_kick_player_%s", m_pzReason);
	char *szResult = (char*)malloc(sizeof(result));
	Q_strncpy(szResult, result, sizeof(result));
	return szResult;
}

const char *CKickIssue::GetVotePassedString()
{
	return "#TF_vote_passed_kick_player";
	//TODO: add something for "#TF_vote_passed_ban_player";
}

void CKickIssue::ListIssueDetails( CBasePlayer *pForWhom )
{
	char s[64];
	if (true)//There should be check or something
	{
		V_snprintf(s, sizeof(s), "callvote %s <userID>\n", GetTypeString());
		ClientPrint( pForWhom, 2, s );
	}
}

bool CKickIssue::IsEnabled()
{
	return sv_vote_issue_kick_allowed.GetBool();
}

const char *CKickIssue::GetDetailsString()
{
	if (m_iPlayerID > 0 && UTIL_PlayerByIndex(m_iPlayerID - 1))
	{
		return m_pzPlayerName;
	}
	else
	{
		return "Unnamed";
	}
}

void CKickIssue::OnVoteStarted()
{
	const char *pDetails = CBaseIssue::GetDetailsString();
	const char *pch;
	pch = strrchr(pDetails, ' ');
	if (!pch)
		return;

	int i = pch - pDetails + 1;
	Q_snprintf(m_pzReason, sizeof(m_pzReason), pDetails + i);

	char m_PlayerID[64];
	Q_snprintf(m_PlayerID, i, pDetails);

	m_iPlayerID = atoi(m_PlayerID);
	CBasePlayer *pVoteCaller = UTIL_PlayerByIndex(m_iPlayerID - 1);
	if (!pVoteCaller)
		return;

	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), pVoteCaller->GetPlayerName());

	g_voteController->TryCastVote(pVoteCaller->entindex(), "Option2");
	CSteamID pSteamID;
	pVoteCaller->GetSteamID(&pSteamID);
	g_voteController->AddPlayerToNameLockedList(pSteamID, sv_vote_timer_duration.GetFloat(), m_iPlayerID);
}

void CKickIssue::Init()
{

}

void CKickIssue::NotifyGC(bool a2)
{
	return;
}

int CKickIssue::PrintLogData()
{
	return 0;
}

void CKickIssue::OnVoteFailed( int iEntityHoldingVote )
{
	CBaseIssue::OnVoteFailed( iEntityHoldingVote );
	PrintLogData();
}

bool CKickIssue::CreateVoteDataFromDetails(const char *s)
{
	return 0;
}

/*bool CKickIssue::CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime )
{
	return false;
}*/

void CKickIssue::ExecuteCommand()
{
	engine->ServerCommand( UTIL_VarArgs( "kickid %d %s\n", m_iPlayerID, m_pzReason ) );
}

void CKickIssue::SetIssueCooldownDuration( float flDuration )
{
	flDuration = 300;
}

//-----------------------------------------------------------------------------
// Purpose: Restart Game Issue
//-----------------------------------------------------------------------------
CRestartGameIssue::CRestartGameIssue(const char *typeString) : CBaseIssue(typeString)
{
	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), "");
	m_iPlayerID = 0;
}

CRestartGameIssue::~CRestartGameIssue()
{
}

const char *CRestartGameIssue::GetDisplayString()
{
	return "#TF_vote_restart_game";
}

const char *CRestartGameIssue::GetVotePassedString()
{
	return "#TF_vote_passed_restart_game";
}

void CRestartGameIssue::ListIssueDetails( CBasePlayer *pForWhom )
{
	char s[64];
	if (true)//There should be check or something
	{
		V_snprintf(s, sizeof(s), "callvote %s <userID>\n", GetTypeString());
		ClientPrint( pForWhom, 2, s );
	}
}

bool CRestartGameIssue::IsEnabled()
{
	return sv_vote_issue_restart_game_allowed.GetBool();
}

const char *CRestartGameIssue::GetDetailsString()
{
	if (m_iPlayerID > 0 && UTIL_PlayerByIndex(m_iPlayerID - 1))
	{
		return m_pzPlayerName;
	}
	else
	{
		return "Unnamed";
	}
}

void CRestartGameIssue::OnVoteStarted()
{
	const char *pDetails = CBaseIssue::GetDetailsString();
	const char *pch;
	pch = strrchr(pDetails, ' ');
	if (!pch)
		return;

	int i = pch - pDetails + 1;

	char m_PlayerID[64];
	Q_snprintf(m_PlayerID, i, pDetails);

	m_iPlayerID = atoi(m_PlayerID);
	CBasePlayer *pVoteCaller = UTIL_PlayerByIndex(m_iPlayerID - 1);
	if (!pVoteCaller)
		return;

	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), pVoteCaller->GetPlayerName());

	g_voteController->TryCastVote(pVoteCaller->entindex(), "Option2");
	CSteamID pSteamID;
	pVoteCaller->GetSteamID(&pSteamID);
	g_voteController->AddPlayerToNameLockedList(pSteamID, sv_vote_timer_duration.GetFloat(), m_iPlayerID);
}

void CRestartGameIssue::Init()
{

}

void CRestartGameIssue::NotifyGC(bool a2)
{
	return;
}

int CRestartGameIssue::PrintLogData()
{
	return 0;
}

void CRestartGameIssue::OnVoteFailed( int iEntityHoldingVote )
{
	CBaseIssue::OnVoteFailed(iEntityHoldingVote);
	PrintLogData();
}

bool CRestartGameIssue::CreateVoteDataFromDetails(const char *s)
{
	return 0;
}

/*int CRestartGameIssue::CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime )
{
	return false;
}*/

void CRestartGameIssue::ExecuteCommand()
{
	engine->ServerCommand( "mp_restartgame 1 \n" );
}

void CRestartGameIssue::SetIssueCooldownDuration( float flDuration )
{
	flDuration = sv_vote_issue_restart_game_cooldown.GetFloat();
}

//-----------------------------------------------------------------------------
// Purpose: Change Level Issue
//-----------------------------------------------------------------------------
CChangeLevelIssue::CChangeLevelIssue(const char *typeString) : CBaseIssue(typeString)
{
	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), "");
	Q_snprintf(m_pzMap, sizeof(m_pzMap), "");
	m_iPlayerID = 0;
}

CChangeLevelIssue::~CChangeLevelIssue()
{
}

const char *CChangeLevelIssue::GetDisplayString()
{
	char result[64];
	Q_snprintf(result, sizeof(result), "#TF_vote_changelevel", m_pzMap);
	char *szResult = (char*)malloc(sizeof(result));
	Q_strncpy(szResult, result, sizeof(result));
	return szResult;
}

const char *CChangeLevelIssue::GetVotePassedString()
{
	return "#TF_vote_passed_changelevel";
}

void CChangeLevelIssue::ListIssueDetails( CBasePlayer *pForWhom )
{
	char s[64];
	if (true)//There should be check or something
	{
		V_snprintf(s, sizeof(s), "callvote %s <userID>\n", GetTypeString());
		ClientPrint( pForWhom, 2, s );
	}
}

bool CChangeLevelIssue::IsEnabled()
{
	return sv_vote_issue_changelevel_allowed.GetBool();
}

const char *CChangeLevelIssue::GetDetailsString()
{
	if (m_iPlayerID > 0 && UTIL_PlayerByIndex(m_iPlayerID - 1))
	{
		return m_pzPlayerName;
	}
	else
	{
		return "Unnamed";
	}
}

void CChangeLevelIssue::OnVoteStarted()
{
	const char *pDetails = CBaseIssue::GetDetailsString();
	const char *pch;
	pch = strrchr(pDetails, ' ');
	if (!pch)
		return;

	int i = pch - pDetails + 1;
	Q_snprintf(m_pzMap, sizeof(m_pzMap), pDetails + i);

	char m_PlayerID[64];
	Q_snprintf(m_PlayerID, i, pDetails);

	m_iPlayerID = atoi(m_PlayerID);
	CBasePlayer *pVoteCaller = UTIL_PlayerByIndex(m_iPlayerID - 1);
	if (!pVoteCaller)
		return;

	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), pVoteCaller->GetPlayerName());

	g_voteController->TryCastVote(pVoteCaller->entindex(), "Option2");
	CSteamID pSteamID;
	pVoteCaller->GetSteamID(&pSteamID);
	g_voteController->AddPlayerToNameLockedList(pSteamID, sv_vote_timer_duration.GetFloat(), m_iPlayerID);
}

void CChangeLevelIssue::Init()
{

}

void CChangeLevelIssue::NotifyGC(bool a2)
{
	return;
}

int CChangeLevelIssue::PrintLogData()
{
	return 0;
}

void CChangeLevelIssue::OnVoteFailed( int iEntityHoldingVote )
{
	CBaseIssue::OnVoteFailed( iEntityHoldingVote );
	PrintLogData();
}

bool CChangeLevelIssue::CreateVoteDataFromDetails(const char *s)
{
	return 0;
}

/*bool CChangeLevelIssue::CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime )
{
	return false;
}*/

void CChangeLevelIssue::ExecuteCommand()
{
	engine->ServerCommand( UTIL_VarArgs( "wait 5;changelevel %s\n", m_pzMap ) );
}

void CChangeLevelIssue::SetIssueCooldownDuration( float flDuration )
{
	flDuration = 300;
}

//-----------------------------------------------------------------------------
// Purpose: Change Difficulty Issue
//-----------------------------------------------------------------------------
CChangeDifficultyIssue::CChangeDifficultyIssue(const char *typeString) : CBaseIssue(typeString)
{
	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), "");
	Q_snprintf(m_pzReason, sizeof(m_pzReason), "");
	m_iPlayerID = 0;
	m_iLevel = 1;
}

CChangeDifficultyIssue::~CChangeDifficultyIssue()
{
}

const char *CChangeDifficultyIssue::GetDisplayString()
{
	char result[64];
	Q_snprintf(result, sizeof(result), "#LFE_vote_change_difficulty_%s", m_pzReason);
	char *szResult = (char*)malloc(sizeof(result));
	Q_strncpy(szResult, result, sizeof(result));
	return szResult;
}

const char *CChangeDifficultyIssue::GetVotePassedString()
{
	return "#LFE_vote_passed_change_difficulty";
}

void CChangeDifficultyIssue::ListIssueDetails( CBasePlayer *pForWhom )
{
	char s[64];
	if (true)//There should be check or something
	{
		V_snprintf(s, sizeof(s), "callvote %s <userID>\n", GetTypeString());
		ClientPrint( pForWhom, 2, s );
	}
}

bool CChangeDifficultyIssue::IsEnabled()
{
	return sv_vote_issue_change_difficulty_allowed.GetBool();
}

const char *CChangeDifficultyIssue::GetDetailsString()
{
	if (m_iPlayerID > 0 && UTIL_PlayerByIndex(m_iPlayerID - 1))
	{
		return m_pzPlayerName;
	}
	else
	{
		return "Unnamed";
	}
}

void CChangeDifficultyIssue::OnVoteStarted()
{
	const char *pDetails = CBaseIssue::GetDetailsString();
	const char *pch;
	pch = strrchr(pDetails, ' ');
	if (!pch)
		return;

	int i = pch - pDetails + 1;
	Q_snprintf(m_pzReason, sizeof(m_pzReason), pDetails + i);

	char m_PlayerID[64];
	Q_snprintf(m_PlayerID, i, pDetails);

	m_iPlayerID = atoi(m_PlayerID);
	CBasePlayer *pVoteCaller = UTIL_PlayerByIndex(m_iPlayerID - 1);
	if (!pVoteCaller)
		return;

	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), pVoteCaller->GetPlayerName());

	g_voteController->TryCastVote(pVoteCaller->entindex(), "Option2");
	CSteamID pSteamID;
	pVoteCaller->GetSteamID(&pSteamID);
	g_voteController->AddPlayerToNameLockedList(pSteamID, sv_vote_timer_duration.GetFloat(), m_iPlayerID);
}

void CChangeDifficultyIssue::Init()
{

}

void CChangeDifficultyIssue::NotifyGC(bool a2)
{
	return;
}

int CChangeDifficultyIssue::PrintLogData()
{
	return 0;
}

void CChangeDifficultyIssue::OnVoteFailed( int iEntityHoldingVote )
{
	CBaseIssue::OnVoteFailed( iEntityHoldingVote );
	PrintLogData();
}

bool CChangeDifficultyIssue::CreateVoteDataFromDetails(const char *s)
{
	return 0;
}

/*bool CChangeDifficultyIssue::CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime )
{
	return false;
}*/

void CChangeDifficultyIssue::ExecuteCommand()
{
	engine->ServerCommand( UTIL_VarArgs( "sv_difficulty %d\n", m_iLevel ) );
}

void CChangeDifficultyIssue::SetIssueCooldownDuration( float flDuration )
{
	flDuration = sv_vote_issue_change_difficulty_cooldown.GetFloat();
}

// may need these down there
//CBaseTFIssue, CChangeLevelIssue, CNextLevelIssue, CExtendLevelIssue, CScrambleTeams@@, CMannVsMachineChangeChallengeIssue@@, CEnableTemporaryHalloweenIssue,
//CTeamAutoBalanceIssue, CClassLimitsIssue, VCPauseGameIssue