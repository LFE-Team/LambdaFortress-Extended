//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef FUNC_VEHICLE_H
#define FUNC_VEHICLE_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "entityoutput.h"
#include "pathtrack.h"

//-----------------------------------------------------------------------------
// Purpose: Visualizes a respawn room to the enemy team
//-----------------------------------------------------------------------------
#define VEHICLE_SPEED0_ACCELERATION 0.005000000000000000
#define VEHICLE_SPEED1_ACCELERATION 0.002142857142857143
#define VEHICLE_SPEED2_ACCELERATION 0.003333333333333334
#define VEHICLE_SPEED3_ACCELERATION 0.004166666666666667
#define VEHICLE_SPEED4_ACCELERATION 0.004000000000000000
#define VEHICLE_SPEED5_ACCELERATION 0.003800000000000000
#define VEHICLE_SPEED6_ACCELERATION 0.004500000000000000
#define VEHICLE_SPEED7_ACCELERATION 0.004250000000000000
#define VEHICLE_SPEED8_ACCELERATION 0.002666666666666667
#define VEHICLE_SPEED9_ACCELERATION 0.002285714285714286
#define VEHICLE_SPEED10_ACCELERATION 0.001875000000000000
#define VEHICLE_SPEED11_ACCELERATION 0.001444444444444444
#define VEHICLE_SPEED12_ACCELERATION 0.001200000000000000
#define VEHICLE_SPEED13_ACCELERATION 0.000916666666666666
#define VEHICLE_SPEED14_ACCELERATION 0.001444444444444444

class CFuncVehicle : public CBaseEntity
{
	DECLARE_CLASS( CFuncVehicle, CBaseEntity );
	DECLARE_DATADESC();
public:
	void Spawn(void);
	void Restart(void);
	virtual Class_T Classify();
	void Precache(void);
	void Blocked(CBaseEntity *pOther);
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	bool KeyValue( const char *szKeyName, const char *szValue );
	int Save(CSave &save);
	int Restore(CRestore &restore);
	int ObjectCaps(void) { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DIRECTIONAL_USE; }
	void OverrideReset(void);
	void CheckTurning(void);
	void CollisionDetection(void);
	void TerrainFollowing(void);

	void Next(void);
	void Find(void);
	void NearestPath(void);
	void DeadEnd(void);

	void SetTrack( CPathTrack *track ) { m_ppath = track->Nearest(GetLocalOrigin()); }
	void SetControls( CBaseEntity *pControls );
	bool OnControls( CBaseEntity *pControls );
	void StopSound( void );
	void UpdateSound( void );

public:
	static CFuncVehicle *Instance( edict_t *pent );

public:
	CPathTrack *m_ppath;
	float m_length;
	float m_width;
	float m_height;
	float m_maxSpeed;
	float m_speed;
	float m_dir;
	float m_startSpeed;
	Vector m_controlMins;
	Vector m_controlMaxs;
	int m_soundPlaying;
	int m_sounds;
	int m_acceleration;
	float m_flVolume;
	float m_flBank;
	float m_oldSpeed;
	int m_iTurnAngle;
	float m_flSteeringWheelDecay;
	float m_flAcceleratorDecay;
	float m_flTurnStartTime;
	float m_flLaunchTime;
	float m_flLastNormalZ;
	float m_flCanTurnNow;
	float m_flUpdateSound;
	Vector m_vFrontLeft;
	Vector m_vFront;
	Vector m_vFrontRight;
	Vector m_vBackLeft;
	Vector m_vBack;
	Vector m_vBackRight;
	Vector m_vSurfaceNormal;
	Vector m_vVehicleDirection;
	CBaseEntity *m_pDriver;

private:
	unsigned short m_usAdjustPitch;
};

#endif // FUNC_VEHICLE_H