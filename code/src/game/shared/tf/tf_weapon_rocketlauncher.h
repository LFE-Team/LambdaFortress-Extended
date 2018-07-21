//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#ifndef TF_WEAPON_ROCKETLAUNCHER_H
#define TF_WEAPON_ROCKETLAUNCHER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_rocket.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFRocketLauncher C_TFRocketLauncher
#define CTFRocketLauncher_DirectHit C_TFRocketLauncher_DirectHit
#define CTFRocketLauncher_AirStrike C_TFRocketLauncher_AirStrike
#define CTFWeaponFlameBall C_TFWeaponFlameBall
#endif

//=============================================================================
//
// TF Weapon Rocket Launcher.
//
class CTFRocketLauncher : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFRocketLauncher, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFRocketLauncher();
	~CTFRocketLauncher();

#ifndef CLIENT_DLL
	virtual void	Precache();
#endif
	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER; }
	virtual CBaseEntity *FireProjectile( CTFPlayer *pPlayer );
	virtual void	ItemPostFrame( void );
	virtual bool	Deploy( void );
	virtual bool	DefaultReload( int iClipSize1, int iClipSize2, int iActivity );

#ifdef CLIENT_DLL
	virtual void CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex );
#endif

private:
	float	m_flShowReloadHintAt;

	CTFRocketLauncher( const CTFRocketLauncher & ) {}
};

// Server specific
#ifdef GAME_DLL

//=============================================================================
//
// Generic rocket.
//
class CTFRocket : public CTFBaseRocket
{
public:

	DECLARE_CLASS( CTFRocket, CTFBaseRocket );

	// Creation.
	static CTFRocket *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL );	
	virtual void Spawn();
	virtual void Precache();
};

#endif

// Live tf2 weapons
class CTFRocketLauncher_DirectHit : public CTFRocketLauncher
{
public:
	DECLARE_CLASS( CTFRocketLauncher_DirectHit, CTFRocketLauncher );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT; }
};

class CTFRocketLauncher_AirStrike : public CTFRocketLauncher
{
public:
	DECLARE_CLASS( CTFRocketLauncher_AirStrike, CTFRocketLauncher );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER; }
};

class CTFRocketLauncher_Mortar : public CTFRocketLauncher
{
public:
	DECLARE_CLASS( CTFRocketLauncher_Mortar, CTFRocketLauncher );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER; }
};

class CTFWeaponFlameBall : public CTFRocketLauncher
{
public:
	DECLARE_CLASS( CTFWeaponFlameBall, CTFRocketLauncher );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFWeaponFlameBall();
	~CTFWeaponFlameBall();

	virtual void	Spawn( void );
	virtual void	Precache( void );

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER_FIREBALL; }

	virtual void	PrimaryAttack();
	virtual void	SecondaryAttack();

	virtual CBaseEntity *FireProjectile( CTFPlayer *pPlayer );
	virtual void	ItemPostFrame( void );
	virtual bool	Deploy( void );
	virtual bool	DefaultReload( int iClipSize1, int iClipSize2, int iActivity );

	virtual void	GetProjectileFireSetup( CTFPlayer *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates = true, bool bUseHitboxes = false );
#ifdef GAME_DLL
	virtual void	DeflectEntity( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir );
	virtual void	DeflectPlayer( CTFPlayer *pVictim, CTFPlayer *pAttacker, Vector &vecDir );
	virtual void	DeflectNPC( CAI_BaseNPC *pVictim, CTFPlayer *pAttacker, Vector &vecDir );
	virtual void	DeflectPhysics( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir );

	void			SetHitTarget( void );
	void			HitTargetThink( void );
#endif
private:

	CNetworkVar( bool, m_bHitTarget );

};

#endif // TF_WEAPON_ROCKETLAUNCHER_H