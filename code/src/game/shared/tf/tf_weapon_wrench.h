//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_WRENCH_H
#define TF_WEAPON_WRENCH_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFWrench C_TFWrench
#define CTFRobotArm C_TFRobotArm
#endif

//=============================================================================
//
// Wrench class.
//

class CTFWrench : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFWrench, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFWrench();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_WRENCH; }
	virtual void		Smack( void );
	virtual float		GetMeleeDamage( CBaseEntity *pTarget, int &iCustomDamage );

#ifdef GAME_DLL
	virtual void OnFriendlyBuildingHit( CBaseObject *pObject, CTFPlayer *pPlayer, Vector vecHitPos );
#endif

private:

	CTFWrench( const CTFWrench & ) {}
};

class CTFRobotArm : public CTFWrench
{
public:
	DECLARE_CLASS( CTFRobotArm, CTFWrench );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROBOT_ARM; }
};

#endif // TF_WEAPON_WRENCH_H