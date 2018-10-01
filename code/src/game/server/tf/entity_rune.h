//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Mannpower powerups 
//
//=============================================================================//

#ifndef ENTITY_RUNE_H
#define ENTITY_RUNE_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"
#include "tf_shareddefs.h"

//=============================================================================

class CTFRune : public CTFPowerup
{
public:
	DECLARE_CLASS( CTFRune, CTFPowerup );
	DECLARE_DATADESC();

	CTFRune();

	void	Spawn( void );
	void	Precache( void );
	void	Materialize( void );
	bool	MyTouch( CBasePlayer *pPlayer );
	float	GetEffectDuration( void ) { return m_flEffectDuration; }
	void	SetEffectDuration( float flTime ) { m_flEffectDuration = flTime; }

	virtual const char *GetDefaultPickupSound( void ) { return "HealthKit.Touch"; }
	virtual const char *GetDefaultPowerupModel( void ) { return "models/class_menu/random_class_icon.mdl"; }

	virtual int GetCondition( void ) { return TF_COND_LAST; } // Should trigger an assert.

	static CTFRune *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszClassname, float flDuration );

	string_t	m_strPickupSound;
protected:
	float		m_flEffectDuration;
};

//=============================================================================

class CTFRuneTemp : public CTFRune
{
public:
	DECLARE_CLASS( CTFRuneTemp, CTFRune );
	DECLARE_DATADESC();

	CTFRuneTemp();
	virtual void	Spawn( void );
	CBaseEntity* Respawn( void );
	float	GetRespawnDelay( void );
	virtual int	GetCondition( void ) { return m_iPowerupCondition; }

private:
	int		m_iPowerupCondition;

	bool	 m_bRespawning;

	CNetworkVar( float, m_flRespawnTime );
	CNetworkVar( float, m_flRespawnAtTime );
};

//=============================================================================

class CTFRuneTempCrit : public CTFRuneTemp
{
public:
	DECLARE_CLASS( CTFRuneTempCrit, CTFRuneTemp );
	DECLARE_DATADESC();

	CTFRuneTempCrit();

	virtual int	GetCondition( void ) { return TF_COND_CRITBOOSTED_RUNE_TEMP; }

	virtual bool	MyTouch( CBasePlayer *pPlayer );
};

//=============================================================================

class CTFRuneTempUber : public CTFRuneTemp
{
public:
	DECLARE_CLASS( CTFRuneTempUber, CTFRuneTemp );
	DECLARE_DATADESC();

	CTFRuneTempUber();

	// is this the condition that live tf2 use?
	virtual int	GetCondition( void ) { return TF_COND_INVULNERABLE_USER_BUFF; }

	virtual bool	MyTouch( CBasePlayer *pPlayer );
};

//=============================================================================

class CTFRuneCustom : public CTFRune
{
public:
	DECLARE_CLASS( CTFRuneCustom, CTFRune );
	DECLARE_DATADESC();

	CTFRuneCustom();

	virtual int	GetCondition( void ) { return m_iPowerupCondition; }

private:
	int		m_iPowerupCondition;
};

DECLARE_AUTO_LIST( IInfoPowerupSpawnAutoList );

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class CTFInfoPowerupSpawn : public CBaseEntity, public IInfoPowerupSpawnAutoList
{
public:
	DECLARE_CLASS( CTFInfoPowerupSpawn, CBaseEntity );
	DECLARE_DATADESC();

	CTFInfoPowerupSpawn();
	~CTFInfoPowerupSpawn();

	void	Spawn( void );
};
#endif // ENTITY_RUNE_H
