//====== Copyright © 1996-2018, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Throwable Jar(s) from live TF2
//
//=============================================================================
#ifndef TF_WEAPON_JAR_H
#define TF_WEAPON_JAR_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_grenadeproj.h"

#ifdef CLIENT_DLL
#define CTFJar C_TFJar
#define CTFProjectile_Jar C_TFProjectile_Jar
#endif

class CTFProjectile_Jar : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS( CTFProjectile_Jar, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFProjectile_Jar();
	~CTFProjectile_Jar();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_JAR; }

#ifdef GAME_DLL
	static CTFProjectile_Jar *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, const Vector &vecVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pScorer, const AngularImpulse &angVelocity, const CTFWeaponInfo &weaponInfo );

	virtual void	Precache( void );
	virtual void	Spawn( void );

	virtual void	Detonate( void );
	virtual void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	void			JarTouch( CBaseEntity *pOther );

	void			SetCritical( bool bCritical ) { m_bCritical = bCritical; }

	virtual float	SetTimer( void ){ return gpGlobals->curtime + 5.0f; }

	virtual bool	IsDeflectable() { return true; }
	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

	virtual void	Explode( trace_t *pTrace, int bitsDamageType );
#else
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );
#endif

private:
#ifdef GAME_DLL
	EHANDLE m_Scorer;
	CNetworkVar( bool, m_bCritical );
#else
	bool		m_bCritical;
#endif
};

class CTFJar : public CTFWeaponBaseGun
{
public:
	DECLARE_CLASS( CTFJar, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFJar();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_JAR; }

	virtual void	Precache( void );

	virtual void	PrimaryAttack( void );

	virtual float	GetProjectileDamage( void );
	virtual float	GetProjectileSpeed( void );
	virtual float	GetProjectileGravity( void );
	virtual bool	CalcIsAttackCriticalHelper( void );

	virtual bool		HasChargeBar( void )							{ return true; }
	virtual const char* GetEffectLabelText( void )						{ return "#TF_Jar"; }
	virtual float		InternalGetEffectBarRechargeTime()				{ return 20.0; }
};

#endif // TF_WEAPON_JAR_H

