//=========== Copyright © 2018, LFE-Team, Not All rights reserved. ============
//
// Purpose:
//
//=============================================================================
#ifndef TF_FLAME_H
#define TF_FLAME_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "tf_weapon_flamethrower.h"
// Client specific.
#ifdef CLIENT_DLL
	#include "particlemgr.h"
	#include "particle_util.h"
	#include "particles_simple.h"
	#include "c_tf_projectile_rocket.h"

#else
	#include "tf_projectile_rocket.h"
	#include "baseentity.h"
#endif

// Client specific.
#ifdef CLIENT_DLL
#define CTFFlameEntity C_TFFlameEntity
#endif

class CTFFlameThrower;

DECLARE_AUTO_LIST( ITFFlameEntityAutoList );

class CTFFlameEntity : public CBaseEntity, public ITFFlameEntityAutoList
{
public:
	DECLARE_CLASS( CTFFlameEntity, CBaseEntity );
	DECLARE_NETWORKCLASS();

	CTFFlameEntity();
	~CTFFlameEntity();

	virtual void Spawn( void );
	void	Precache( void );
//=============================================================================
//
// Client specific.
//
#ifdef CLIENT_DLL
	virtual void	OnDataChanged( DataUpdateType_t updateType );

	virtual void	CreateTrails( void );
//=============================================================================
//
// Server specific.
//
#else
public:
	DECLARE_DATADESC();
	static CTFFlameEntity *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, int iDmgType, float m_flDmgAmount );

	void FlameThink( void );
	void CheckCollision( CBaseEntity *pOther, bool *pbHitWorld );
	CBaseEntity *GetAttacker( void ) { return m_hAttacker.Get(); }
private:
	void OnCollide( CBaseEntity *pOther );
	void SetHitTarget( void );

	Vector						m_vecInitialPos;		// position the flame was fired from
	Vector						m_vecPrevPos;			// position from previous frame
	Vector						m_vecBaseVelocity;		// base velocity vector of the flame (ignoring rise effect)
	Vector						m_vecAttackerVelocity;	// velocity of attacking player at time flame was fired
	float						m_flTimeRemove;			// time at which the flame should be removed
	int							m_iDmgType;				// damage type
	float						m_flDmgAmount;			// amount of base damage
	CUtlVector<EHANDLE>			m_hEntitiesBurnt;		// list of entities this flame has burnt
	EHANDLE						m_hAttacker;			// attacking player
	int							m_iAttackerTeam;		// team of attacking player
#endif
	CHandle<CTFFlameThrower>	m_hLauncher;			// weapon that fired this flame
};

#endif // TF_FLAME_H