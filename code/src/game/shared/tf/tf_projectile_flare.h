//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#ifndef TF_PROJECTILE_FLARE_H
#define TF_PROJECTILE_FLARE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"
#include "tf_weapon_rocketlauncher.h"
#ifdef GAME_DLL
#include "iscorer.h"
#endif

// Client specific.
#ifdef CLIENT_DLL
#define CTFProjectile_Flare C_TFProjectile_Flare
#define CTFProjectile_BallOfFire C_TFProjectile_BallOfFire
#endif

class CTFProjectile_Flare : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_Flare, CTFBaseRocket );
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();

	CTFProjectile_Flare();
	~CTFProjectile_Flare();

#ifdef GAME_DLL

	static CTFProjectile_Flare *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );
	virtual void	Spawn();
	virtual void	Precache();

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_FLAREGUN; }

	void	SetScorer( CBaseEntity *pScorer );

	void	SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual int		GetDamageType();

	virtual bool IsDeflectable() { return true; }
	virtual void Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

	// Overrides.
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );
#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif

private:
#ifdef GAME_DLL
	CBaseHandle m_Scorer;
	CNetworkVar( bool,	m_bCritical );
#else
	bool		m_bCritical;
#endif

};

class CTFProjectile_BallOfFire : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_BallOfFire, CTFBaseRocket );
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();

	CTFProjectile_BallOfFire();
	~CTFProjectile_BallOfFire();

#ifdef GAME_DLL

	static CTFProjectile_BallOfFire *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );
	virtual void	Spawn();
	virtual void	Precache();

	void FlameThink( void );

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_ROCKETLAUNCHER_FIREBALL; }

	void	SetScorer( CBaseEntity *pScorer );

	void	SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual int		GetDamageType();

	virtual bool IsDeflectable() { return true; }
	virtual void Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

	// Overrides.
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );

	virtual void	RocketTouch( CBaseEntity *pOther );

	void CheckCollision( CBaseEntity *pOther, bool *pbHitWorld );
#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	ClientThink();
	virtual void	CreateTrails( void );

#endif

private:
	Vector						m_vecInitialPos;		// position the flame was fired from
	Vector						m_vecPrevPos;			// position from previous frame
	float						m_flTimeRemove;			// time at which the flame should be removed the placeholder edition
	CHandle<CTFWeaponFlameBall>	m_hLauncher;			// weapon that fired this flame
#ifdef GAME_DLL
	CUtlVector<EHANDLE>			m_hEntitiesBurnt;		// list of entities this flame has burnt

	CBaseHandle m_Scorer;
	CNetworkVar( bool,	m_bCritical );

	void OnCollide( CBaseEntity *pOther );
	void SetHitTarget( void );
#else
	bool		m_bCritical;
#endif

};

#endif //TF_PROJECTILE_FLARE_H