//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "func_vehicle.h"
#include "trains.h"
#include "ndebugoverlay.h"
#include "util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//===========================================================================================================

BEGIN_DATADESC( CFuncVehicle )
	DEFINE_FIELD( m_ppath, FIELD_CLASSPTR ),
	DEFINE_KEYFIELD( m_length, FIELD_FLOAT, "length" ),
	DEFINE_KEYFIELD( m_width, FIELD_FLOAT, "width" ),
	DEFINE_KEYFIELD( m_height, FIELD_FLOAT, "height" ),
	DEFINE_FIELD( m_speed, FIELD_FLOAT ),
	DEFINE_FIELD( m_dir, FIELD_FLOAT ),
	DEFINE_KEYFIELD( m_maxSpeed, FIELD_FLOAT, "startspeed" ),
	DEFINE_FIELD( m_controlMins, FIELD_VECTOR ),
	DEFINE_FIELD( m_controlMaxs, FIELD_VECTOR ),
	DEFINE_KEYFIELD( m_sounds, FIELD_INTEGER, "sounds" ),
	DEFINE_FIELD( m_flVolume, FIELD_FLOAT ),
	DEFINE_KEYFIELD( m_flBank, FIELD_FLOAT, "bank" ),
	DEFINE_FIELD( m_oldSpeed, FIELD_FLOAT ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_vehicle, CFuncVehicle );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFuncVehicle::KeyValue( const char *szKeyName, const char *szValue )
{
	if (FStrEq(szKeyName, "volume"))
	{
		m_flVolume = (float) (atoi(szValue));
		m_flVolume *= 0.1f;
	}
	else if (FStrEq(szKeyName, "acceleration"))
	{
		m_acceleration = (int) (atoi(szValue));
		if (m_acceleration < 1)
			m_acceleration = 1;
		else if (m_acceleration > 10)
			m_acceleration = 10;
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}

void CFuncVehicle::Blocked( CBaseEntity *pOther )
{
	if ( ( pOther->GetFlags() & FL_ONGROUND ) && pOther->GetGroundEntity() == this )
	{
		pOther->SetAbsVelocity( GetAbsVelocity() );
		return;
	}
	else
	{
		Vector vecNewVelocity;
		vecNewVelocity = pOther->GetAbsOrigin() - GetAbsOrigin();
		VectorNormalize(vecNewVelocity);
		vecNewVelocity *= 150;
		pOther->SetAbsVelocity( vecNewVelocity );
	}

	DevMsg( 1, "TRAIN(%s): Blocked by %s (dmg:%.2f)\n", GetDebugName(), pOther->GetClassname(), 150 );

	Vector vecForward, vecRight;
	GetVectors( &vecForward, &vecRight, NULL );

	Vector vFrontLeft = (vecForward * -1) * (m_length * 0.5);
	Vector vFrontRight = (vecRight * -1) * (m_width * 0.5);
	Vector vBackLeft = GetAbsOrigin() + vFrontLeft - vFrontRight;
	Vector vBackRight = GetAbsOrigin() - vFrontLeft + vFrontRight;
	float minx = min(vBackLeft.x, vBackRight.x);
	float maxx = max(vBackLeft.x, vBackRight.x);
	float miny = min(vBackLeft.y, vBackRight.y);
	float maxy = max(vBackLeft.y, vBackRight.y);
	//float minz = GetAbsOrigin().z;
	float maxz = GetAbsOrigin().z + (2 * abs((int)( CollisionProp()->OBBMins().z - CollisionProp()->OBBMaxs().z)));

	if (pOther->GetAbsOrigin().x < minx || pOther->GetAbsOrigin().x > maxx || pOther->GetAbsOrigin().y < miny || pOther->GetAbsOrigin().y > maxy ||
		pOther->GetAbsOrigin().z < GetAbsOrigin().z || pOther->GetAbsOrigin().z > maxz)
		pOther->TakeDamage( CTakeDamageInfo( this, this, 150, DMG_CRUSH ) );
}

void CFuncVehicle::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	float delta = value;

	if (useType != USE_SET)
	{
		if (!ShouldToggle(useType, (m_flSpeed != 0)))
			return;

		if (m_flSpeed == 0)
		{
			m_flSpeed = m_speed * m_dir;
			Next();
		}
		else
		{
			m_flSpeed = 0;
			SetAbsVelocity( vec3_origin );
			SetLocalAngularVelocity( vec3_angle );
			StopSound();
			SetThink(NULL);
		}
	}

	if (delta < 10)
	{
		if (delta < 0 && m_flSpeed > 145)
			StopSound();

		float flSpeedRatio = delta;

		if (delta > 0)
		{
			flSpeedRatio = m_flSpeed / m_speed;

			if (m_flSpeed < 0)
				flSpeedRatio = m_acceleration * 0.0005 + flSpeedRatio + VEHICLE_SPEED0_ACCELERATION;
			else if (m_flSpeed < 10)
				flSpeedRatio = m_acceleration * 0.0006 + flSpeedRatio + VEHICLE_SPEED1_ACCELERATION;
			else if (m_flSpeed < 20)
				flSpeedRatio = m_acceleration * 0.0007 + flSpeedRatio + VEHICLE_SPEED2_ACCELERATION;
			else if (m_flSpeed < 30)
				flSpeedRatio = m_acceleration * 0.0007 + flSpeedRatio + VEHICLE_SPEED3_ACCELERATION;
			else if (m_flSpeed < 45)
				flSpeedRatio = m_acceleration * 0.0007 + flSpeedRatio + VEHICLE_SPEED4_ACCELERATION;
			else if (m_flSpeed < 60)
				flSpeedRatio = m_acceleration * 0.0008 + flSpeedRatio + VEHICLE_SPEED5_ACCELERATION;
			else if (m_flSpeed < 80)
				flSpeedRatio = m_acceleration * 0.0008 + flSpeedRatio + VEHICLE_SPEED6_ACCELERATION;
			else if (m_flSpeed < 100)
				flSpeedRatio = m_acceleration * 0.0009 + flSpeedRatio + VEHICLE_SPEED7_ACCELERATION;
			else if (m_flSpeed < 150)
				flSpeedRatio = m_acceleration * 0.0008 + flSpeedRatio + VEHICLE_SPEED8_ACCELERATION;
			else if (m_flSpeed < 225)
				flSpeedRatio = m_acceleration * 0.0007 + flSpeedRatio + VEHICLE_SPEED9_ACCELERATION;
			else if (m_flSpeed < 300)
				flSpeedRatio = m_acceleration * 0.0006 + flSpeedRatio + VEHICLE_SPEED10_ACCELERATION;
			else if (m_flSpeed < 400)
				flSpeedRatio = m_acceleration * 0.0005 + flSpeedRatio + VEHICLE_SPEED11_ACCELERATION;
			else if (m_flSpeed < 550)
				flSpeedRatio = m_acceleration * 0.0005 + flSpeedRatio + VEHICLE_SPEED12_ACCELERATION;
			else if (m_flSpeed < 800)
				flSpeedRatio = m_acceleration * 0.0005 + flSpeedRatio + VEHICLE_SPEED13_ACCELERATION;
			else
				flSpeedRatio = m_acceleration * 0.0005 + flSpeedRatio + VEHICLE_SPEED14_ACCELERATION;
		}
		else if (delta < 0)
		{
			flSpeedRatio = m_flSpeed / m_speed;

			if (flSpeedRatio > 0)
				flSpeedRatio -= 0.0125;
			else if (flSpeedRatio <= 0 && flSpeedRatio > -0.05)
				flSpeedRatio -= 0.0075;
			else if (flSpeedRatio <= 0.05 && flSpeedRatio > -0.1)
				flSpeedRatio -= 0.01;
			else if (flSpeedRatio <= 0.15 && flSpeedRatio > -0.15)
				flSpeedRatio -= 0.0125;
			else if (flSpeedRatio <= 0.15 && flSpeedRatio > -0.22)
				flSpeedRatio -= 0.01375;
			else if (flSpeedRatio <= 0.22 && flSpeedRatio > -0.3)
				flSpeedRatio -= - 0.0175;
			else if (flSpeedRatio <= 0.3)
				flSpeedRatio -= 0.0125;
		}

		if (flSpeedRatio > 1)
			flSpeedRatio = 1;
		else if (flSpeedRatio < -0.35)
			flSpeedRatio = -0.35;

		m_flSpeed = m_speed * flSpeedRatio;
		Next();
		m_flAcceleratorDecay = gpGlobals->curtime + 0.25;
	}
	else
	{
		if (gpGlobals->curtime > m_flCanTurnNow)
		{
			if (delta == 20)
			{
				m_iTurnAngle++;
				m_flSteeringWheelDecay = gpGlobals->curtime + 0.075;

				if (m_iTurnAngle > 8)
					m_iTurnAngle = 8;
			}
			else if (delta == 30)
			{
				m_iTurnAngle--;
				m_flSteeringWheelDecay = gpGlobals->curtime + 0.075;

				if (m_iTurnAngle < -8)
					m_iTurnAngle = -8;
			}

			m_flCanTurnNow = gpGlobals->curtime + 0.05;
		}
	}
}

extern void FixupAngles( QAngle &v );

#define VEHICLE_STARTPITCH 60
#define VEHICLE_MAXPITCH 200
#define VEHICLE_MAXSPEED 1500

void CFuncVehicle::StopSound(void)
{
	/*if (m_soundPlaying && pev->noise)
	{
		unsigned short us_sound = ((unsigned short)m_sounds & 0x0007) << 12;
		unsigned short us_encode = us_sound;

		PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0, (float *)&vec3_origin, (float *)&vec3_origin, 0, 0, us_encode, 0, 1, 0);
	}

	m_soundPlaying = 0;*/
}

void CFuncVehicle::UpdateSound(void)
{
	/*if (!pev->noise)
		return;

	float flpitch = VEHICLE_STARTPITCH + (abs((int)m_flSpeed) * (VEHICLE_MAXPITCH - VEHICLE_STARTPITCH) / VEHICLE_MAXSPEED);

	if (flpitch > 200)
		flpitch = 200;

	CPASAttenuationFilter filter( this );
	CPASAttenuationFilter filterReliable( this );
	filterReliable.MakeReliable();

	if (!m_soundPlaying)
	{
		if (m_sounds < 5)
		{

			EmitSound_t ep;
			ep.m_nChannel = CHAN_ITEM;
			ep.m_pSoundName = "plats/vehicle_brake1.wav";
			ep.m_flVolume = m_flVolume;
			ep.m_SoundLevel = SNDLVL_NORM;
			ep.m_pOrigin = &WorldSpaceCenter();

			EmitSound( filter, entindex(), ep );
		}

		EMIT_SOUND_DYN(ENT(pev), CHAN_STATIC, (char *)STRING(pev->noise), m_flVolume, ATTN_NORM, 0, (int)flpitch);
		m_soundPlaying = 1;
	}
	else
	{
		unsigned short us_sound = ((unsigned short)(m_sounds) & 0x0007) << 12;
		unsigned short us_pitch = ((unsigned short)(flpitch / 10.0) & 0x003F) << 6;
		unsigned short us_volume = ((unsigned short)(m_flVolume * 40) & 0x003F);
		unsigned short us_encode = us_sound | us_pitch | us_volume;

		PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_UPDATE, edict(), m_usAdjustPitch, 0.0, (float *)&vec3_origin, (float *)&vec3_origin, 0.0, 0.0, us_encode, 0, 0, 0);
	}*/
}

void CFuncVehicle::CheckTurning(void)
{
	trace_t tr;
	Vector vecStart, vecEnd;

	Vector vecRight;
	GetVectors( NULL, &vecRight, NULL );

	if (m_iTurnAngle < 0)
	{
		if (m_flSpeed > 0)
		{
			vecStart = m_vFrontLeft;
			vecEnd = vecStart - vecRight * 16;
		}
		else if (m_flSpeed < 0)
		{
			vecStart = m_vBackLeft;
			vecEnd = vecStart + vecRight * 16;
		}

		UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction != 1)
			m_iTurnAngle = 1;
	}
	else if (m_iTurnAngle > 0)
	{
		if (m_flSpeed > 0)
		{
			vecStart = m_vFrontRight;
			vecEnd = vecStart + vecRight * 16;
		}
		else if (m_flSpeed < 0)
		{
			vecStart = m_vBackRight;
			vecEnd = vecStart - vecRight * 16;
		}

		UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction != 1)
			m_iTurnAngle = -1;
	}

	if (m_flSpeed <= 0)
		return;

	float speed;
	int turning = abs(m_iTurnAngle);

	if (turning > 4)
	{
		if (m_flTurnStartTime != -1)
		{
			float time = gpGlobals->curtime - m_flTurnStartTime;

			if (time >= 0)
				speed = m_speed * 0.98;
			else if (time > 0.3)
				speed = m_speed * 0.95;
			else if (time > 0.6)
				speed = m_speed * 0.9;
			else if (time > 0.8)
				speed = m_speed * 0.8;
			else if (time > 1)
				speed = m_speed * 0.7;
			else if (time > 1.2)
				speed = m_speed * 0.5;
			else
				speed = time;
		}
		else
		{
			m_flTurnStartTime = gpGlobals->curtime;
			speed = m_speed;
		}
	}
	else
	{
		m_flTurnStartTime = -1;

		if (turning > 2)
			speed = m_speed * 0.9;
		else
			speed = m_speed;
	}

	if (speed < m_flSpeed)
		m_flSpeed -= m_speed * 0.1;
}

void CFuncVehicle::CollisionDetection(void)
{
	trace_t tr;
	Vector vecStart, vecEnd;
	float flDot;

	Vector vecForward;
	GetVectors( &vecForward, NULL, NULL );

	if (m_flSpeed < 0)
	{
		vecStart = m_vBackLeft;
		vecEnd = vecStart + (vecForward * 16);
		UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction != 1)
		{
			flDot = DotProduct(vecForward, tr.plane.normal * -1);

			if (flDot < 0.7 && tr.plane.normal.z < 0.1)
			{
				m_vSurfaceNormal = tr.plane.normal;
				m_vSurfaceNormal.z = 0;
				m_flSpeed *= 0.99;
			}
			else if (tr.plane.normal.z < 0.65 || tr.startsolid)
				m_flSpeed *= -1;
			else
				m_vSurfaceNormal = tr.plane.normal;

			CBaseEntity *pHit = CBaseEntity::Instance(tr.DidHit());

			if (pHit && FClassnameIs(pHit, "func_vehicle") )
				DevMsg("I hit another vehicle\n");
		}

		vecStart = m_vBackRight;
		vecEnd = vecStart + (vecForward * 16);
		UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction == 1)
		{
			vecStart = m_vBack;
			vecEnd = vecStart + (vecForward * 16);
			UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

			if (tr.fraction == 1)
				return;
		}

		flDot = DotProduct(vecForward, tr.plane.normal * -1);

		if (flDot >= 0.7)
		{
			if (tr.plane.normal.z < 0.65 || tr.startsolid != false)
				m_flSpeed *= -1;
			else
				m_vSurfaceNormal = tr.plane.normal;
		}
		else if (tr.plane.normal.z < 0.1)
		{
			m_vSurfaceNormal = tr.plane.normal;
			m_vSurfaceNormal.z = 0;
			m_flSpeed *= 0.99;
		}
		else if (tr.plane.normal.z < 0.65 || tr.startsolid != false)
			m_flSpeed *= -1;
		else
			m_vSurfaceNormal = tr.plane.normal;
	}
	else if (m_flSpeed > 0)
	{
		vecStart = m_vFrontRight;
		vecEnd = vecStart - (vecForward * 16);
		UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction == 1)
		{
			vecStart = m_vFrontLeft;
			vecEnd = vecStart - (vecForward * 16);
			UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

			if (tr.fraction == 1)
			{
				vecStart = m_vFront;
				vecEnd = vecStart - (vecForward * 16);
				UTIL_TraceLine(vecStart, vecEnd, MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

				if (tr.fraction == 1)
					return;
			}
		}

		flDot = DotProduct(vecForward, tr.plane.normal * -1);

		if (flDot <= -0.7)
		{
			if (tr.plane.normal.z < 0.65 || tr.startsolid != false)
				m_flSpeed *= -1;
			else
				m_vSurfaceNormal = tr.plane.normal;
		}
		else if (tr.plane.normal.z < 0.1)
		{
			m_vSurfaceNormal = tr.plane.normal;
			m_vSurfaceNormal.z = 0;
			m_flSpeed *= 0.99;
		}
		else if (tr.plane.normal.z < 0.65 || tr.startsolid != false )
			m_flSpeed *= -1;
		else
			m_vSurfaceNormal = tr.plane.normal;
	}
}

void CFuncVehicle::TerrainFollowing(void)
{
	trace_t tr;
	UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + Vector(0, 0, (m_height + 48) * -1), MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction != 1)
		m_vSurfaceNormal = tr.plane.normal;
	else if (tr.contents)
		m_vSurfaceNormal = Vector(0, 0, 1);
}

void CFuncVehicle::Next(void)
{
	Vector vGravityVector = vec3_origin;

	Vector vecForward, vecRight, vecUp;
	GetVectors( &vecForward, &vecRight, &vecUp );

	Vector forward = (vecForward * -1) * (m_length * 0.5);
	Vector right = (vecRight * -1) * (m_width * 0.5);
	Vector up = vecUp * 16;

	m_vFrontRight = GetAbsOrigin() + forward - right + up;
	m_vFrontLeft = GetAbsOrigin() + forward + right + up;
	m_vFront = GetAbsOrigin() + forward + up;
	m_vBackLeft = GetAbsOrigin() - forward - right + up;
	m_vBackRight = GetAbsOrigin() - forward + right + up;
	m_vBack = GetAbsOrigin() - forward + up;
	m_vSurfaceNormal = vec3_origin;

	CheckTurning();

	if (gpGlobals->curtime > m_flSteeringWheelDecay)
	{
		m_flSteeringWheelDecay = gpGlobals->curtime + 0.1;

		if (m_iTurnAngle < 0)
			m_iTurnAngle++;
		else if (m_iTurnAngle > 0)
			m_iTurnAngle--;
	}

	if (gpGlobals->curtime > m_flAcceleratorDecay)
	{
		if (m_flSpeed < 0)
		{
			m_flSpeed += 20;

			if (m_flSpeed > 0)
				m_flSpeed = 0;
		}
		else if (m_flSpeed > 0)
		{
			m_flSpeed -= 20;

			if (m_flSpeed < 0)
				m_flSpeed = 0;
		}
	}

	if (m_flSpeed == 0)
	{
		m_iTurnAngle = 0;
		SetLocalAngularVelocity( vec3_angle );
		SetAbsVelocity( vec3_origin );
		SetThink(&CFuncVehicle::Next);
		SetNextThink( gpGlobals->curtime + 0.1f );
		return;
	}

	TerrainFollowing();
	CollisionDetection();

	if (m_vSurfaceNormal == vec3_origin)
	{
		if (m_flLaunchTime != -1)
		{
			vGravityVector = Vector(0, 0, 0);
			vGravityVector.z = (gpGlobals->curtime - m_flLaunchTime) * -35;

			if (vGravityVector.z < -400)
				vGravityVector.z = -400;
		}
		else
		{
			m_flLaunchTime = gpGlobals->curtime;
			vGravityVector = Vector(0, 0, 0);
			SetAbsVelocity( GetAbsVelocity() * 1.5 );
		}

		m_vVehicleDirection = vecForward * -1;
	}
	else
	{
		m_vVehicleDirection = CrossProduct(m_vSurfaceNormal, vecForward);
		m_vVehicleDirection = CrossProduct(m_vSurfaceNormal, m_vVehicleDirection);

		QAngle angles;
		VectorAngles( m_vVehicleDirection, angles );
		angles.y += 180;

		if (m_iTurnAngle != 0)
			angles.y += m_iTurnAngle;

		FixupAngles(angles);

		float vx = UTIL_AngleDistance(angles.x, GetAbsAngles().x);
		float vy = UTIL_AngleDistance(angles.y, GetAbsAngles().y);

		if (vx > 10)
			vx = 10;
		else if (vx < -10)
			vx = -10;

		if (vy > 10)
			vy = 10;
		else if (vy < -10)
			vy = -10;

		//GetLocalAngularVelocity().y = (int)(vy * 10);
		//GetLocalAngularVelocity().x = (int)(vx * 10);
		m_flLaunchTime = -1;
		m_flLastNormalZ = m_vSurfaceNormal.z;
	}

	AngleVectors( GetAbsAngles(), &m_vVehicleDirection );

	if (gpGlobals->curtime > m_flUpdateSound)
	{
		UpdateSound();
		m_flUpdateSound = gpGlobals->curtime + 1;
	}

	if (m_vSurfaceNormal == vec3_origin)
	{
		SetAbsVelocity( GetAbsVelocity() + vGravityVector );
	}
	else
	{
		VectorNormalize( m_vVehicleDirection );
		SetAbsVelocity( m_vVehicleDirection * m_flSpeed );
	}

	SetThink(&CFuncVehicle::Next);
	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CFuncVehicle::DeadEnd(void)
{
	CPathTrack *pTrack, *pNext;
	pTrack = m_ppath;

	DevMsg( 2, "TRAIN(%s): Dead end ", GetDebugName() );

	if ( pTrack )
	{
		if ( m_oldSpeed < 0 )
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetPrevious(), true );
				if ( pNext )
					pTrack = pNext;
			} while ( pNext );
		}
		else
		{
			do
			{
				pNext = pTrack->ValidPath( pTrack->GetNext(), true );
				if ( pNext )
					pTrack = pNext;
			} while ( pNext );
		}
	}

	SetAbsVelocity( vec3_origin );
	SetLocalAngularVelocity( vec3_angle );

	if ( pTrack )
	{
		DevMsg( 2, "at %s\n", pTrack->GetDebugName() );
		variant_t emptyVariant;
		pTrack->AcceptInput( "InPass", this, this, emptyVariant, 0 );
	}
	else
	{
		DevMsg( 2, "\n" );
	}
}

void CFuncVehicle::SetControls( CBaseEntity *pControls )
{
	Vector offset = pControls->GetLocalOrigin();

	m_controlMins = pControls->WorldAlignMins() + offset;
	m_controlMaxs = pControls->WorldAlignMaxs() + offset;
}

bool CFuncVehicle::OnControls( CBaseEntity *pTest )
{
	Vector offset = pTest->GetLocalOrigin() - GetLocalOrigin();

	if ( m_spawnflags & SF_TRACKTRAIN_NOCONTROL )
		return false;

	//VectorAngles(GetAbsAngles());

	Vector vecForward, vecRight, vecUp;
	GetVectors( &vecForward, &vecRight, &vecUp );

	Vector local;
	local.x = DotProduct(offset, vecForward);
	local.y = -DotProduct(offset, vecRight);
	local.z = DotProduct(offset, vecUp);

	if (local.x >= m_controlMins.x && local.y >= m_controlMins.y && local.z >= m_controlMins.z && local.x <= m_controlMaxs.x && local.y <= m_controlMaxs.y && local.z <= m_controlMaxs.z)
		return true;

	return false;
}

void CFuncVehicle::Find( void )
{
	m_ppath = (CPathTrack *)gEntList.FindEntityByName( NULL, m_target );
	if ( !m_ppath )
		return;

	if ( !FClassnameIs( m_ppath, "path_track" ) 
#ifndef PORTAL	//env_portal_path_track is a child of path_track and would like to get found
		 && !FClassnameIs( m_ppath, "env_portal_path_track" )
#endif //#ifndef PORTAL
		)
	{
		Warning( "func_track_train must be on a path of path_track\n" );
		Assert(0);
		m_ppath = NULL;
		return;
	}



	Vector nextPos = m_ppath->GetLocalOrigin();
	Vector look = nextPos;
	m_ppath->LookAhead( look, m_length, 0 );
	nextPos.z += m_height;
	look.z += m_height;

	Vector LooknextPos = look - nextPos;
	QAngle nextAngles;
	AngleVectors( nextAngles, &LooknextPos );
	nextAngles.y += 180;

	if ( HasSpawnFlags( SF_TRACKTRAIN_NOPITCH ) )
		nextAngles.x = 0;

	Teleport( &nextPos, &nextAngles, NULL );
	SetNextThink( gpGlobals->curtime + 0.1f );
	SetThink(&CFuncVehicle::Next);
	m_flSpeed = m_startSpeed;
	UpdateSound();
}

void CFuncVehicle::NearestPath( void )
{
	CBaseEntity *pTrack = NULL;
	CBaseEntity *pNearest = NULL;
	float dist, closest;

	closest = 1024;

	for ( CEntitySphereQuery sphere( GetAbsOrigin(), 1024 ); ( pTrack = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
	{
		// filter out non-tracks
		if ( !(pTrack->GetFlags() & (FL_CLIENT|FL_NPC)) && FClassnameIs( pTrack, "path_track" ) )
		{
			dist = (GetAbsOrigin() - pTrack->GetAbsOrigin()).Length();
			if ( dist < closest )
			{
				closest = dist;
				pNearest = pTrack;
			}
		}
	}

	if ( !pNearest )
	{
		Msg( "Can't find a nearby track !!!\n" );
		SetThink(NULL);
		return;
	}

	DevMsg( 2, "TRAIN: %s, Nearest track is %s\n", GetDebugName(), pNearest->GetDebugName() );
	// If I'm closer to the next path_track on this path, then it's my real path
	pTrack = ((CPathTrack *)pNearest)->GetNext();
	if ( pTrack )
	{
		if ( (GetLocalOrigin() - pTrack->GetLocalOrigin()).Length() < (GetLocalOrigin() - pNearest->GetLocalOrigin()).Length() )
			pNearest = pTrack;
	}

	m_ppath = (CPathTrack *)pNearest;

	if (m_flSpeed != 0)
	{
		SetNextThink( gpGlobals->curtime + 0.1f );
		SetThink(&CFuncVehicle::Next);
	}
}

void CFuncVehicle::OverrideReset(void)
{
	SetNextThink( gpGlobals->curtime + 0.1f );
	SetThink(&CFuncVehicle::NearestPath);
}

CFuncVehicle *CFuncVehicle::Instance( edict_t *pent )
{ 
	CBaseEntity *pEntity = CBaseEntity::Instance( pent );
	if ( FClassnameIs( pEntity, "func_vehicle" ) )
		return (CFuncVehicle *)pEntity;
	return NULL;
}

Class_T CFuncVehicle::Classify(void)
{
	//return CLASS_VEHICLE;
	return CLASS_NONE;
}

void CFuncVehicle::Spawn(void)
{
	if (m_flSpeed == 0)
		m_speed = 165;
	else
		m_speed = m_flSpeed;

	if (!m_sounds)
		m_sounds = 3;

	Msg( "M_speed = %f\n", m_speed );

	m_flSpeed = 0;
	SetAbsVelocity( vec3_origin );
	SetLocalAngularVelocity( vec3_angle );
	//pev->impulse = (int)m_speed;
	m_acceleration = 5;
	m_dir = 1;
	m_flTurnStartTime = -1;

	if ( !m_target )
		Msg( "Vehicle with no target\n" );

	SetSolid( SOLID_BSP );
	if ( HasSpawnFlags( SF_TRACKTRAIN_PASSABLE ) )
		AddSolidFlags( FSOLID_NOT_SOLID );

	SetMoveType( MOVETYPE_PUSH );

	SetModel( STRING( GetModelName() ) );
	//UTIL_SetSize(pev, pev->mins, pev->maxs);

	//pev->oldorigin = GetAbsOrigin();
	m_controlMins = CollisionProp()->OBBMins();
	m_controlMaxs = CollisionProp()->OBBMaxs();
	m_controlMaxs.z += 72;

	SetNextThink( gpGlobals->curtime + 0.1f );
	SetThink(&CFuncVehicle::Find);
	Precache();
}

void CFuncVehicle::Restart(void)
{
	Msg( "M_speed = %f\n", m_speed );

	m_flSpeed = 0;
	SetAbsVelocity( vec3_origin );
	SetLocalAngularVelocity( vec3_angle );
	//pev->impulse = (int)m_speed;
	m_flTurnStartTime = -1;
	m_flUpdateSound = -1;
	m_dir = 1;
	m_pDriver = NULL;

	if ( !m_target )
		Msg( "Vehicle with no target\n" );

	//UTIL_SetOrigin(this, pev->oldorigin);
	SetNextThink( gpGlobals->curtime + 0.1f );
	SetThink(&CFuncVehicle::Find);
}

void CFuncVehicle::Precache(void)
{
	if (m_flVolume == 0)
		m_flVolume = 1;

	switch (m_sounds)
	{
		case 1: PrecacheSound("plats/vehicle1.wav");
		case 2: PrecacheSound("plats/vehicle2.wav");
		case 3: PrecacheSound("plats/vehicle3.wav");
		case 4: PrecacheSound("plats/vehicle4.wav");
		case 5: PrecacheSound("plats/vehicle6.wav");
		case 6: PrecacheSound("plats/vehicle7.wav");
	}

	PrecacheSound("plats/vehicle_brake1.wav");
	PrecacheSound("plats/vehicle_start1.wav");
	//m_usAdjustPitch = PRECACHE_EVENT(1, "events/vehicle.sc");
}

class CFuncVehicleControls : public CBaseEntity
{
	DECLARE_CLASS( CFuncVehicleControls, CBaseEntity );
public:
	int ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void Spawn(void);
	void Find(void);

	DECLARE_DATADESC();
};

BEGIN_DATADESC( CFuncVehicleControls )

	// Function Pointers
	DEFINE_FUNCTION( Find ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( func_vehiclecontrols, CFuncVehicleControls );

void CFuncVehicleControls::Find(void)
{
	CBaseEntity *pTarget = NULL;

	do 
	{
		pTarget = gEntList.FindEntityByName( pTarget, m_target );
	} while ( pTarget && !FClassnameIs(pTarget, "func_vehicle") );

	if ( !pTarget )
	{
		Msg( "No vehicle %s\n", STRING(m_target) );
		return;
	}

	CFuncVehicle *ptrain = (CFuncVehicle*) pTarget;
	ptrain->SetControls(this);
	//UTIL_Remove(this);
	SetThink( NULL );
}

void CFuncVehicleControls::Spawn(void)
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
	SetModel( STRING( GetModelName() ) );
	AddEffects( EF_NODRAW );

	Assert( GetParent() && "func_vehiclecontrols needs parent to properly align to vehicle" );
	
	SetThink(&CFuncVehicleControls::Find);
	SetNextThink( gpGlobals->curtime );
}