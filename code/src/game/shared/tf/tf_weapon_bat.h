//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BAT_H
#define TF_WEAPON_BAT_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFBat C_TFBat
#define CTFBat_Wood C_TFBat_Wood
#define CTFBat_Giftwarp C_TFBat_Giftwarp
#define CTFBat_Fish C_TFBat_Fish
#endif

//=============================================================================
//
// Bat class.
//
class CTFBat : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFBat, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFBat();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BAT; }

private:

	CTFBat( const CTFBat & ) {}
};

//=============================================================================
//
// Wood Bat class.
//
class CTFBat_Wood : public CTFBat
{
public:

	DECLARE_CLASS( CTFBat_Wood, CTFBat );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BAT_WOOD; }
};
/*
//=============================================================================
//
// Wood Giftwarp class.
//
class CTFBat_Giftwrap : public CTFBat
{
public:

	DECLARE_CLASS( CTFBat_Giftwrap, CTFBat );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BAT_GIFTWARP; }
};
*/
//=============================================================================
//
// Wood Bat class.
//
class CTFBat_Fish : public CTFBat
{
public:

	DECLARE_CLASS( CTFBat_Fish, CTFBat );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BAT_FISH; }
};

#endif // TF_WEAPON_BAT_H
