//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: RPG
//
//=============================================================================//


#ifndef WEAPON_RPG_H
#define WEAPON_RPG_H
#ifdef _WIN32
#pragma once
#endif


#include "hl1mp_basecombatweapon_shared.h"
//#include "sprite.h"
//#include "NPCEvent.h"
//#include "smoke_trail.h"
//#include "hl1_basegrenade.h"

#ifdef CLIENT_DLL
#include "iviewrender_beams.h"
#include "c_smoke_trail.h"
#endif

#ifndef CLIENT_DLL
#include "smoke_trail.h"
#include "sprite.h"
#include "NPCEvent.h"
#include "beam_shared.h"
#include "hl1_basegrenade.h"

class CWeaponHL1RPG;

//###########################################################################
//	CRpgRocket
//###########################################################################
class CRpgRocket : public CHL1BaseGrenade
{
	DECLARE_CLASS(CRpgRocket, CHL1BaseGrenade);
	DECLARE_SERVERCLASS();

public:
	CRpgRocket();

	Class_T Classify(void) { return CLASS_NONE; }

	void Spawn(void);
	void Precache(void);
	void RocketTouch(CBaseEntity *pOther);
	void IgniteThink(void);
	void SeekThink(void);

	virtual void Detonate(void);

	static CRpgRocket *Create(const Vector &vecOrigin, const QAngle &angAngles, CBasePlayer *pentOwner = NULL);

	CHandle<CWeaponHL1RPG>		m_hOwner;
	float					m_flIgniteTime;
	int						m_iTrail;

	DECLARE_DATADESC();
};


#endif

#ifdef CLIENT_DLL
#define CLaserHL1Dot C_LaserHL1Dot
#endif

class CLaserHL1Dot;

#ifdef CLIENT_DLL
#define CWeaponHL1RPG C_WeaponHL1RPG
#endif

//-----------------------------------------------------------------------------
// CWeaponHL1RPG
//-----------------------------------------------------------------------------
class CWeaponHL1RPG : public CBaseHL1MPCombatWeapon
{
	DECLARE_CLASS(CWeaponHL1RPG, CBaseHL1MPCombatWeapon);
public:

	CWeaponHL1RPG(void);
	~CWeaponHL1RPG();

	void	ItemPostFrame(void);
	void	Precache(void);
	bool	Deploy(void);
	void	PrimaryAttack(void);
	void	WeaponIdle(void);
	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);
	void	NotifyRocketDied(void);
	bool	Reload(void);

	void	Drop(const Vector &vecVelocity);

	virtual int	GetDefaultClip1(void) const;

	//	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

private:
	void	CreateLaserPointer(void);
	void	UpdateSpot(void);
	bool	IsGuiding(void);
	void	StartGuiding(void);
	void	StopGuiding(void);

#ifndef CLIENT_DLL
	//	DECLARE_ACTTABLE();
#endif

private:
	//	bool				m_bGuiding;
	//	CHandle<CLaserHL1Dot>	m_hLaserHL1Dot;
	//	CHandle<CRpgRocket>	m_hMissile;
	//	bool				m_bIntialStateUpdate;
	//	bool				m_bLaserHL1DotSuspended;
	//	float				m_flLaserHL1DotReviveTime;

	CNetworkVar(bool, m_bGuiding);
	CNetworkVar(bool, m_bIntialStateUpdate);
	CNetworkVar(bool, m_bLaserHL1DotSuspended);
	CNetworkVar(float, m_flLaserHL1DotReviveTime);

	CNetworkHandle(CBaseEntity, m_hMissile);

#ifndef CLIENT_DLL
	CHandle<CLaserHL1Dot>	m_hLaserHL1Dot;
#endif
};


#endif	// WEAPON_RPG_H
