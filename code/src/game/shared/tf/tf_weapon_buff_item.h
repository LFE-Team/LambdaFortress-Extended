//=========== Copyright � 2018, LFE-Team, Not All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BUFF_H
#define TF_WEAPON_BUFF_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFBuffItem C_TFBuffItem
#endif

//=============================================================================
//
// B U F F item class.
//
class CTFBuffItem : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFBuffItem, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFBuffItem();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BUFF_ITEM; }

	virtual void		Precache( void );

	virtual void		PrimaryAttack( void );
	virtual void		WeaponReset( void );

	int					GetBuffType( void );

	void				CheckEffectBarRegen( void );
	virtual bool		HasChargeBar( void )							{ return true; }
	virtual const char* GetEffectLabelText( void )						{ return "#TF_Rage"; }
	virtual float		InternalGetEffectBarRechargeTime( void )		{ return 600.0f; }
private:

	CTFBuffItem( const CTFBuffItem & ) {}
};

#endif // TF_WEAPON_FIREAXE_H