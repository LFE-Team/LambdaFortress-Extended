//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BOTTLE_H
#define TF_WEAPON_BOTTLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFBottle C_TFBottle
#define CTFStickBomb C_TFStickBomb
#endif

//=============================================================================
//
// Bottle class.
//
class CTFBottle : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFBottle, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFBottle();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_BOTTLE; }

	virtual void		Smack( void );
	virtual void		WeaponReset( void );
	virtual bool		DefaultDeploy( char *szViewModel, char *szWeaponModel, int iActivity, char *szAnimExt );

	virtual void		SwitchBodyGroups( void );

protected:

	CNetworkVar( bool,	m_bBroken  );

private:

	CTFBottle( const CTFBottle & ) {}
};

//==================== sigsegv-mvm's reversed engineering =====================
//
// Caber class.
//
class CTFStickBomb : public CTFBottle
{
public:
	DECLARE_CLASS(CTFStickBomb, CTFBottle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	
	CTFStickBomb();
	CTFStickBomb( const CTFStickBomb& ) = delete;
	
	virtual int GetWeaponID() const override { return TF_WEAPON_STICKBOMB; }
	virtual const char *GetWorldModel() const override;
	virtual void Precache() override;
	virtual void Smack() override;
	virtual void SwitchBodyGroups() override;
	virtual void WeaponRegenerate() override;
	virtual void WeaponReset() override;
	
#ifdef CLIENT_DLL
	virtual int GetWorldModelIndex() override;
#endif
	
private:
	CNetworkVar( int, m_iDetonated );
};

#endif // TF_WEAPON_BOTTLE_H