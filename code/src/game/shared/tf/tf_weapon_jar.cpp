//====== Copyright © 1996-2018, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Throwable Jar(s) from live TF2
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_jar.h"
#include "tf_fx_shared.h"
#include "in_buttons.h"
#include "datacache/imdlcache.h"
#include "effect_dispatch_data.h"
#include "engine/IEngineSound.h"
#include "tf_gamerules.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "particles_new.h"
// Server specific.
#else
#include "tf_player.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "tf_fx.h"
#include "tf_gamestats.h"
#endif

#define URINE_MODEL "models/weapons/c_models/urinejar.mdl"

#define TF_JAR_DAMAGE 0.0f
#define TF_JAR_VEL 935
#define TF_JAR_GRAV 1.0f

extern ConVar tf_grenade_show_radius;
//=============================================================================
//
// TF Projectile Jar tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Jar, DT_TFProjectile_Jar )

BEGIN_NETWORK_TABLE( CTFProjectile_Jar, DT_TFProjectile_Jar )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bCritical ) ),
#else
	SendPropBool( SENDINFO( m_bCritical ) ),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFProjectile_Jar )
END_DATADESC()
#endif

ConVar tf_jar_show_radius( "tf_jar_show_radius", "0", FCVAR_REPLICATED | FCVAR_CHEAT /*| FCVAR_DEVELOPMENTONLY*/, "Render jar radius." );

LINK_ENTITY_TO_CLASS( tf_projectile_jar, CTFProjectile_Jar );
PRECACHE_REGISTER( tf_projectile_jar );

CTFProjectile_Jar::CTFProjectile_Jar()
{
}

CTFProjectile_Jar::~CTFProjectile_Jar()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

#ifdef GAME_DLL
CTFProjectile_Jar *CTFProjectile_Jar::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, const Vector &vecVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pScorer, const AngularImpulse &angVelocity, const CTFWeaponInfo &weaponInfo )
{
	CTFProjectile_Jar *pJar = static_cast<CTFProjectile_Jar *>( CBaseEntity::CreateNoSpawn( "tf_projectile_jar", vecOrigin, vecAngles, pOwner ) );

	if ( pJar )
	{
		// Set firing weapon.
		pJar->SetLauncher( pWeapon );

		DispatchSpawn( pJar );

		pJar->InitGrenade( vecVelocity, angVelocity, pOwner, pWeapon );
		pJar->SetDetonateTimerLength( 5.0f );

		pJar->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pJar;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Precache( void )
{
	PrecacheModel( URINE_MODEL );

	PrecacheTeamParticles( "peejar_trail_%s", true );
	PrecacheParticleSystem( "peejar_impact" );

	PrecacheScriptSound( "Jar.Explode" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Spawn( void )
{
	SetModel( URINE_MODEL );
	BaseClass::Spawn();
	SetTouch( &CTFProjectile_Jar::JarTouch );

	AddSolidFlags( FSOLID_TRIGGER );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Explode( trace_t *pTrace, int bitsDamageType )
{
	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out of the wall a bit
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	CTFWeaponBase *pWeapon = dynamic_cast<CTFWeaponBase *>( m_hLauncher.Get() );

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	//EmitSound( "Jar.Explode" );
	CPVSFilter filter( vecOrigin );
	TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, GetWeaponID(), -1 );

	// no need for danger because it's just p i s s
	CSoundEnt::InsertSound( SOUND_COMBAT, vecOrigin, 512, 3.0 );
/*
#ifdef CLIENT_DLL
	CEffectData data;
	data.m_vOrigin = vecOrigin;
	DispatchEffect( "peejar_impact", data );
#endif
*/
	// Damage.
	CBaseEntity *pAttacker = NULL;
	pAttacker = pWeapon->GetOwnerEntity();

	float flRadius = GetDamageRadius();

	CTFRadiusDamageInfo radiusInfo;
	radiusInfo.info.Set( this, pAttacker, m_hLauncher, vec3_origin, vecOrigin, GetDamage(), GetDamageType() );
	radiusInfo.m_vecSrc = vecOrigin;
	radiusInfo.m_flRadius = flRadius;
	radiusInfo.m_flSelfDamageRadius = 121.0f; // Original rocket radius?

	// If we extinguish a friendly player reduce our recharge time by four seconds
	if ( TFGameRules()->RadiusJarEffect( radiusInfo, TF_COND_URINE ) && m_iDeflected == 0 && pWeapon ) 
	{
		pWeapon->ReduceEffectBarRegenTime( 4.0f );
	}

	// Debug!
	if ( tf_jar_show_radius.GetBool() )
	{
		DrawRadius( flRadius );
	}

	SetThink( &CBaseGrenade::SUB_Remove );
	SetTouch( NULL );
	
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::JarTouch( CBaseEntity *pOther )
{
	if ( pOther == GetThrower() )
		return;

	// Verify a correct "other."
	if ( !pOther->IsSolid() || pOther->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS ) )
		return;

	// Handle hitting skybox (disappear).
	trace_t pTrace;
	Vector velDir = GetAbsVelocity();
	VectorNormalize( velDir );
	Vector vecSpot = GetAbsOrigin() - velDir * 32;
	UTIL_TraceLine( vecSpot, vecSpot + velDir * 64, MASK_SOLID, this, COLLISION_GROUP_NONE, &pTrace );

	if ( pTrace.fraction < 1.0 && pTrace.surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	// Blow up if we hit player, npc or building
	if ( pOther->IsPlayer() || pOther->IsNPC() || pOther->IsBaseObject() )
	{
		Explode( &pTrace, GetDamageType() );
	} // We should bounce off of certain surfaces (resupply cabinets, spawn doors, etc.)
	else
	{
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Detonate()
{
	trace_t		tr;
	Vector		vecSpot;// trace starts here!

	SetThink( NULL );

	vecSpot = GetAbsOrigin() + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -32 ), MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, & tr);

	Explode( &tr, GetDamageType() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	BaseClass::VPhysicsCollision( index, pEvent );

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	if ( !pHitEntity )
	{
		return;
	}

	if ( pHitEntity->GetMoveType() == MOVETYPE_NONE )
	{
		// Blow up
		SetThink( &CTFProjectile_Jar::Detonate );
		SetNextThink( gpGlobals->curtime );
	}

	return;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	/*// Get jar's speed.
	float flVel = GetAbsVelocity().Length();
	QAngle angForward;
	VectorAngles( vecDir, angForward );
	AngularImpulse angVelocity( ( 600, random->RandomInt( -1200, 1200 ), 0 ) );
	// Now change jar's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );
	// Bounce it up a bit
	ApplyLocalAngularVelocityImpulse( angVelocity );
	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );*/

	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	if ( pPhysicsObject )
	{
		Vector vecOldVelocity, vecVelocity;

		pPhysicsObject->GetVelocity( &vecOldVelocity, NULL );

		float flVel = vecOldVelocity.Length();

		vecVelocity = vecDir;
		vecVelocity *= flVel;
		AngularImpulse angVelocity( ( 600, random->RandomInt( -1200, 1200 ), 0 ) );

		// Now change grenade's direction.
		pPhysicsObject->SetVelocityInstantaneous( &vecVelocity, &angVelocity );
	}

	CBaseCombatCharacter *pBCC = pDeflectedBy->MyCombatCharacterPointer();

	IncremenentDeflected();
	m_hDeflectOwner = pDeflectedBy;
	SetThrower( pBCC );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
}
#else
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Jar::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		// Now stick our initial velocity into the interpolation history 
		CInterpolatedVar< Vector > &interpolator = GetOriginInterpolator();

		interpolator.ClearHistory();
		float changeTime = GetLastChangeTime( LATCH_SIMULATION_VAR );

		// Add a sample 1 second back.
		Vector vCurOrigin = GetLocalOrigin() - m_vInitialVelocity;
		interpolator.AddToHead( changeTime - 1.0, &vCurOrigin, false );

		// Add the current sample.
		vCurOrigin = GetLocalOrigin();
		interpolator.AddToHead( changeTime, &vCurOrigin, false );

		CreateTrails();
	}

	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Jar::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	const char *pszEffectName = ConstructTeamParticle( "peejar_trail_%s", GetTeamNumber(), false, g_aTeamNamesShort );

	ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
}
#endif

//=============================================================================
//
// Weapon PISS tables.
//


IMPLEMENT_NETWORKCLASS_ALIASED( TFJar, DT_WeaponJar )

BEGIN_NETWORK_TABLE( CTFJar, DT_WeaponJar )
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFJar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_jar, CTFJar );
PRECACHE_WEAPON_REGISTER( tf_weapon_jar );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFJar )
END_DATADESC()
#endif

CTFJar::CTFJar()
{
	m_flEffectBarRegenTime = 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFJar::Precache( void )
{
	BaseClass::Precache();
	PrecacheParticleSystem( "peejar_drips" );
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFJar::PrimaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	if ( !HasAmmo() )
		return;

	CalcIsAttackCritical();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	FireProjectile( pPlayer );

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire( MP_CONCEPT_JARATE_LAUNCH );
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set next attack times.
	float flDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flDelay, mult_postfiredelay );
	m_flNextPrimaryAttack = gpGlobals->curtime + flDelay;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flEffectBarRegenTime = gpGlobals->curtime + InternalGetEffectBarRechargeTime();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFJar::GetProjectileDamage( void )
{
	return TF_JAR_DAMAGE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFJar::GetProjectileGravity( void )
{
	return TF_JAR_GRAV;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFJar::CalcIsAttackCriticalHelper( void )
{
	// No random critical hits.
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFJar::GetProjectileSpeed( void )
{
	float flVelocity = TF_JAR_VEL;
	CALL_ATTRIB_HOOK_FLOAT( flVelocity, mult_projectile_speed );
	return flVelocity;
}