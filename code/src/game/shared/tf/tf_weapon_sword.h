//====== Copyright © 1996-2018, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Sword(s) from live TF2
//
//=============================================================================

#ifndef TF_WEAPON_SWORD_H
#define TF_WEAPON_SWORD_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFSword C_TFSword
#endif

//=============================================================================
//
// Sword class.
//
class CTFSword : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFSword, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFSword();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_SWORD; }

private:

	CTFSword( const CTFSword & ) {}
};

#endif // TF_WEAPON_SWORD_H
