//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: the what am i even doing
//
//
//=============================================================================//

#ifndef TF_REVIVE_H
#define TF_REVIVE_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

#ifdef GAME_DLL
#include "baseanimating.h"
#include "tf_player.h"
#else
#include "c_baseanimating.h"
#endif

#ifdef CLIENT_DLL
#define CTFReviveMarker C_TFReviveMarker
#endif // CLIENT_DLL

//=============================================================================
//
//
//
//=============================================================================
//m_nRevives  ReviveThink MVM.PlayerRevived   speech_revivecall   speech_revivecall_medium    speech_revivecall_hard  mediccall   
//revive_player_stopped revive_player_complete revive_player_notify marker_entindex
class CTFReviveMarker : public CBaseAnimating
{
public:
	DECLARE_CLASS( CTFReviveMarker, CBaseAnimating );
	DECLARE_NETWORKCLASS();

	CTFReviveMarker();
	~CTFReviveMarker();

	virtual void	Spawn();
	virtual void	Precache();

	virtual bool	IsCombatItem( void ) const { return true; }

	virtual	bool		ShouldCollide( int collisionGroup, int contentsMask ) const;
	void			ReviveThink( void );
#ifdef GAME_DLL
	void			InitReviveMarker( CTFPlayer *pOwner, int iClass );
	virtual int		ShouldTransmit( const CCheckTransmitInfo *pInfo );
	void			RevivePlayer( void );
	static CTFReviveMarker *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, int iClass );
#endif
};

#endif // TF_REVIVE_H
