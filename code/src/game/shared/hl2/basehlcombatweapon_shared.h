//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef BASEHLCOMBATWEAPON_SHARED_H
#define BASEHLCOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "basecombatweapon_shared.h"
#if defined( TF_CLASSIC ) || defined( TF_CLASSIC_CLIENT )
#include "tf_fx_shared.h"
#endif

#if defined( CLIENT_DLL )
#define CBaseHLCombatWeapon C_BaseHLCombatWeapon
#endif

class CBaseHLCombatWeapon : public CBaseCombatWeapon
{
#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif
	DECLARE_CLASS( CBaseHLCombatWeapon, CBaseCombatWeapon );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#if defined( TF_CLASSIC ) || defined( TF_CLASSIC_CLIENT )
	// Setup.
	CBaseHLCombatWeapon();

	virtual void Spawn();
#endif

	virtual bool	WeaponShouldBeLowered( void );

			bool	CanLower();
	virtual bool	Ready( void );
	virtual bool	Lower( void );
	virtual bool	Deploy( void );
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	WeaponIdle( void );

	virtual void	AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles );
	virtual	float	CalcViewmodelBob( void );

	virtual Vector	GetBulletSpread( WeaponProficiency_t proficiency );
	virtual float	GetSpreadBias( WeaponProficiency_t proficiency );

	virtual const	WeaponProficiencyInfo_t *GetProficiencyValues();
	static const	WeaponProficiencyInfo_t *GetDefaultProficiencyValues();

	virtual void	ItemHolsterFrame( void );

	int				m_iPrimaryAttacks;		// # of primary attacks performed with this weapon
	int				m_iSecondaryAttacks;	// # of secondary attacks performed with this weapon
#if defined( TF_CLASSIC ) || defined( TF_CLASSIC_CLIENT )
	virtual int	GetDamageType() const { return DMG_BULLET; }
	virtual int GetCustomDamageType() const { return TF_DMG_CUSTOM_NONE; }

	void CalcIsAttackCritical( void );
	void CalcIsAttackMiniCritical( void );
	virtual bool CalcIsAttackCriticalHelper();
	bool IsCurrentAttackACrit() { return m_bCurrentAttackIsCrit; }
	bool IsCurrentAttackAMiniCrit() { return m_bCurrentAttackIsMiniCrit; }

	float				GetLastFireTime( void ) { return m_flLastFireTime; }

	virtual const char	*GetTracerType( void );

	virtual bool	CanCritRapidFire()		{ return false; }
#endif
protected:

	bool			m_bLowered;			// Whether the viewmodel is raised or lowered
	float			m_flRaiseTime;		// If lowered, the time we should raise the viewmodel
	float			m_flHolsterTime;	// When the weapon was holstered

#if defined( TF_CLASSIC ) || defined( TF_CLASSIC_CLIENT )
	bool			m_bCurrentAttackIsCrit;
	bool			m_bCurrentAttackIsMiniCrit;
	float			m_flCritTime;
	float			m_flLastCritCheckTime;
	int				m_iLastCritCheckFrame;
	int				m_iCurrentSeed;

	CNetworkVar( float, m_flLastFireTime );

	char			m_szTracerName[MAX_TRACER_NAME];
#endif
};

#endif // BASEHLCOMBATWEAPON_SHARED_H
