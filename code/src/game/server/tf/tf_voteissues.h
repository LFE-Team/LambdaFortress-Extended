//============== Copyright LFE-TEAM Not All rights reserved. =================//
//
// Purpose: Vote issues
//
//
//=============================================================================//
#ifndef TF_VOTEISSUES_H
#define TF_VOTEISSUES_H
#ifdef _WIN32
#pragma once
#endif

#include "vote_controller.h"

//-----------------------------------------------------------------------------
// Purpose: Kick Issue
//-----------------------------------------------------------------------------
class CKickIssue : public CBaseIssue
{
public:
	CKickIssue(const char *typeString);
	~CKickIssue();
	virtual const char *GetDisplayString();
	virtual const char *GetVotePassedString();
	virtual void	ListIssueDetails( CBasePlayer *pForWhom );
	virtual bool	IsEnabled();
	virtual const char *GetDetailsString();
	virtual void	OnVoteStarted();
	void			Init();
	void			NotifyGC( bool a2 );
	int				PrintLogData();
	virtual void	OnVoteFailed( int iEntityHoldingVote );
	bool			CreateVoteDataFromDetails(const char *s);
	//virtual bool	CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime );
	virtual void	ExecuteCommand();
	virtual bool	IsTeamRestrictedVote() { return true; }
	virtual void	SetIssueCooldownDuration( float flDuration );

private:
	char		 m_pzPlayerName[64];
	char		 m_pzReason[64];
	int			 m_iPlayerID;
};

//-----------------------------------------------------------------------------
// Purpose: Restart Game Issue
//-----------------------------------------------------------------------------
class CRestartGameIssue : public CBaseIssue
{
public:
	CRestartGameIssue( const char *typeString );
	~CRestartGameIssue();
	virtual const char *GetDisplayString();
	virtual const char *GetVotePassedString();
	virtual void	ListIssueDetails( CBasePlayer *pForWhom );
	virtual bool	IsEnabled();
	virtual const char *GetDetailsString();
	virtual void	OnVoteStarted();
	void			Init();
	void			NotifyGC( bool a2 );
	int				PrintLogData();
	virtual void	OnVoteFailed( int iEntityHoldingVote );
	bool			CreateVoteDataFromDetails( const char *s );
	//virtual bool	CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime );
	virtual void	ExecuteCommand();
	virtual bool	IsTeamRestrictedVote() { return false; }
	virtual void	SetIssueCooldownDuration( float flDuration );

private:
	char		 m_pzPlayerName[64];
	int			 m_iPlayerID;
};

//-----------------------------------------------------------------------------
// Purpose: Change Level Issue
//-----------------------------------------------------------------------------
class CChangeLevelIssue : public CBaseIssue
{
public:
	CChangeLevelIssue(const char *typeString);
	~CChangeLevelIssue();
	virtual const char *GetDisplayString();
	virtual const char *GetVotePassedString();
	virtual void	ListIssueDetails( CBasePlayer *pForWhom );
	virtual bool	IsEnabled();
	virtual const char *GetDetailsString();
	virtual void	OnVoteStarted();
	void			Init();
	void			NotifyGC( bool a2 );
	int				PrintLogData();
	virtual void	OnVoteFailed( int iEntityHoldingVote );
	bool			CreateVoteDataFromDetails(const char *s);
	//virtual bool	CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime );
	virtual void	ExecuteCommand();
	virtual bool	IsTeamRestrictedVote() { return true; }
	virtual void	SetIssueCooldownDuration( float flDuration );

private:
	char		 m_pzPlayerName[64];
	char		 m_pzMap[64];
	int			 m_iPlayerID;
};

//-----------------------------------------------------------------------------
// Purpose: Change Difficulty Issue
//-----------------------------------------------------------------------------
class CChangeDifficultyIssue : public CBaseIssue
{
public:
	CChangeDifficultyIssue(const char *typeString);
	~CChangeDifficultyIssue();
	virtual const char *GetDisplayString();
	virtual const char *GetVotePassedString();
	virtual void	ListIssueDetails( CBasePlayer *pForWhom );
	virtual bool	IsEnabled();
	virtual const char *GetDetailsString();
	virtual void	OnVoteStarted();
	void			Init();
	void			NotifyGC( bool a2 );
	int				PrintLogData();
	virtual void	OnVoteFailed( int iEntityHoldingVote );
	bool			CreateVoteDataFromDetails(const char *s);
	//virtual bool	CanCallVote( int nEntIndex, const char *pszDetails, vote_create_failed_t &nFailCode, int &nTime );
	virtual void	ExecuteCommand();
	virtual bool	IsTeamRestrictedVote() { return true; }
	virtual void	SetIssueCooldownDuration( float flDuration );

private:
	char		 m_pzPlayerName[64];
	char		 m_pzReason[64];
	int			 m_iPlayerID;
	int			 m_iLevel;
};
#endif // TF_VOTEISSUES_H
