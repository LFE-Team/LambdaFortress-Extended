//============== Copyright LFE-TEAM Not All rights reserved. ==================//
//
// Purpose: medic become super useful
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
#include "c_tf_player.h"
#endif

#ifdef CLIENT_DLL
#define CTFReviveMarker C_TFReviveMarker
#endif // CLIENT_DLL

//=============================================================================
//
//
//
//=============================================================================
//m_nRevives speech_revivecall   speech_revivecall_medium    speech_revivecall_hard
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
	bool			IsReviveInProgress( void );

	virtual	bool	ShouldCollide( int collisionGroup, int contentsMask ) const;
	void			ReviveThink( void );

	void			ReviveOwner( void );
	//void			PromptOwner( void );
#ifdef GAME_DLL
	DECLARE_DATADESC();
	virtual int		UpdateTransmitState( void );
	virtual int		ShouldTransmit( const CCheckTransmitInfo *pInfo );
	static CTFReviveMarker *Create( CTFPlayer *pOwner );
	void			AddMarkerHealth( float flAmount );
#else
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual bool	IsVisibleToTargetID( void ) const { return true; }
	void CreateReviveMeEffect( void );

	// Medic callout particle effect
	CNewParticleEffect	*m_pReviveMeEffect;
#endif
#ifdef GAME_DLL
	void	SetOwner( CTFPlayer *pOwner );
#endif
	//CNetworkVar( int, m_nRevives );

#ifdef GAME_DLL
	CNetworkHandle( CTFPlayer,	m_hOwner );
#else
	CHandle< C_TFPlayer > m_hOwner;
#endif
protected:

#ifdef GAME_DLL
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iHealth );
	//IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iMaxHealth );
#else
	int				m_iHealth;
	int				m_iMaxHealth;
#endif
};

#endif // TF_REVIVE_H
