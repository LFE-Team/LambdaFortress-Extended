//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Flame Thrower
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_flamethrower.h"
#include "tf_fx_shared.h"
#include "in_buttons.h"
#include "ammodef.h"
#include "tf_gamerules.h"
#include "tf_player_shared.h"
#include "props_shared.h"
#include "tf_weapon_compound_bow.h"
#include "tf_projectile_arrow.h"
#include "eventlist.h"
#include "props_shared.h"
#if defined( CLIENT_DLL )

	#include "c_tf_player.h"
	#include "vstdlib/random.h"
	#include "engine/IEngineSound.h"
	#include "soundenvelope.h"
	#include "dlight.h"
	#include "iefx.h"
	#include "prediction.h"

#else

	#include "explode.h"
	#include "tf_player.h"
	#include "tf_gamerules.h"
	#include "tf_gamestats.h"
	#include "ilagcompensationmanager.h"
	#include "collisionutils.h"
	#include "tf_team.h"
	#include "tf_obj.h"
	#include "ai_basenpc.h"
	#include "npc_antlion.h"
	#include "ai_condition.h"
	#include "props.h"
	#include "soundent.h"

	ConVar	tf_debug_flamethrower("tf_debug_flamethrower", "0", FCVAR_CHEAT | FCVAR_REPLICATED, "Visualize the flamethrower damage." );
	ConVar  tf_flamethrower_velocity( "tf_flamethrower_velocity", "2300.0", FCVAR_CHEAT | FCVAR_REPLICATED, "Initial velocity of flame damage entities." ); // old = 2300.0
	ConVar	tf_flamethrower_drag("tf_flamethrower_drag", "0.87", FCVAR_CHEAT | FCVAR_REPLICATED, "Air drag of flame damage entities." );
	ConVar	tf_flamethrower_float("tf_flamethrower_float", "50.0", FCVAR_CHEAT | FCVAR_REPLICATED, "Upward float velocity of flame damage entities." );
	ConVar  tf_flamethrower_flametime("tf_flamethrower_flametime", "0.5", FCVAR_CHEAT | FCVAR_REPLICATED, "Time to live of flame damage entities." );
	ConVar  tf_flamethrower_vecrand("tf_flamethrower_vecrand", "0.05", FCVAR_CHEAT | FCVAR_REPLICATED, "Random vector added to initial velocity of flame damage entities." );
	ConVar  tf_flamethrower_boxsize("tf_flamethrower_boxsize", "12.0", FCVAR_CHEAT | FCVAR_REPLICATED, "Size of flame damage entities." );
	ConVar  tf_flamethrower_maxdamagedist("tf_flamethrower_maxdamagedist", "350.0", FCVAR_CHEAT | FCVAR_REPLICATED, "Maximum damage distance for flamethrower." );
	ConVar  tf_flamethrower_shortrangedamagemultiplier("tf_flamethrower_shortrangedamagemultiplier", "1.2", FCVAR_CHEAT | FCVAR_REPLICATED, "Damage multiplier for close-in flamethrower damage." );
	ConVar  tf_flamethrower_velocityfadestart("tf_flamethrower_velocityfadestart", ".3", FCVAR_CHEAT | FCVAR_REPLICATED, "Time at which attacker's velocity contribution starts to fade." );
	ConVar  tf_flamethrower_velocityfadeend("tf_flamethrower_velocityfadeend", ".5", FCVAR_CHEAT | FCVAR_REPLICATED, "Time at which attacker's velocity contribution finishes fading." );
	//ConVar  tf_flame_force( "tf_flame_force", "30" );
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// position of end of muzzle relative to shoot position
#define TF_FLAMETHROWER_MUZZLEPOS_FORWARD		70.0f
#define TF_FLAMETHROWER_MUZZLEPOS_RIGHT			12.0f
#define TF_FLAMETHROWER_MUZZLEPOS_UP			-12.0f

#define TF_FLAMETHROWER_AMMO_PER_SECOND_PRIMARY_ATTACK		14.0f
#define TF_FLAMETHROWER_AMMO_PER_SECONDARY_ATTACK	20

#ifdef CLIENT_DLL
	extern ConVar lfe_muzzlelight;
#endif

ConVar  lfe_allow_airblast( "lfe_allow_airblast", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Enable/Disable the Airblast function of the Flamethrower." );
ConVar  lfe_allow_airblast_physics( "lfe_allow_airblast_physics", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Enable/Disable the Airblast pushing Physics." );
#ifdef GAME_DLL
ConVar	lfe_debug_airblast( "lfe_debug_airblast", "0", FCVAR_CHEAT | FCVAR_REPLICATED, "Visualize airblast box." );
ConVar  lfe_debug_airblast_physics_force( "lfe_debug_airblast_physics_force", "30.0f", FCVAR_CHEAT | FCVAR_REPLICATED, "How much is the power of airblast?" );
ConVar  lfe_debug_airblast_physics_mass( "lfe_debug_airblast_physics_mass", "5000.0f", FCVAR_CHEAT | FCVAR_REPLICATED, "Don't push something too heavy." );
ConVar  lfe_debug_airblast_physics_distance( "lfe_debug_airblast_physics_distance", "150", FCVAR_CHEAT | FCVAR_REPLICATED, "What distance that pyro can airblast physics?" );
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFFlameThrower, DT_WeaponFlameThrower )

BEGIN_NETWORK_TABLE( CTFFlameThrower, DT_WeaponFlameThrower )
	#if defined( CLIENT_DLL )
		RecvPropInt( RECVINFO( m_iWeaponState ) ),
		RecvPropBool( RECVINFO( m_bCritFire ) ),
		RecvPropBool( RECVINFO( m_bHitTarget ) )
	#else
		SendPropInt( SENDINFO( m_iWeaponState ), 4, SPROP_UNSIGNED | SPROP_CHANGES_OFTEN ),
		SendPropBool( SENDINFO( m_bCritFire ) ),
		SendPropBool( SENDINFO( m_bHitTarget ) )
	#endif
END_NETWORK_TABLE()

#if defined( CLIENT_DLL )
BEGIN_PREDICTION_DATA( CTFFlameThrower )
	DEFINE_PRED_FIELD( m_iWeaponState, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bCritFire, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( tf_weapon_flamethrower, CTFFlameThrower );
PRECACHE_WEAPON_REGISTER( tf_weapon_flamethrower );

BEGIN_DATADESC( CTFFlameThrower )
END_DATADESC()

// ------------------------------------------------------------------------------------------------ //
// CTFFlameThrower implementation.
// ------------------------------------------------------------------------------------------------ //
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFFlameThrower::CTFFlameThrower()
{
	WeaponReset();

#if defined( CLIENT_DLL )
	m_pFlameEffect = NULL;
	m_pFiringStartSound = NULL;
	m_pFiringLoop = NULL;
	m_bFiringLoopCritical = false;
	m_pPilotLightSound = NULL;
	m_pHitTargetSound = NULL;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFFlameThrower::~CTFFlameThrower()
{
	DestroySounds();
}


void CTFFlameThrower::DestroySounds( void )
{
#if defined( CLIENT_DLL )
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
	if ( m_pFiringStartSound )
	{
		controller.SoundDestroy( m_pFiringStartSound );
		m_pFiringStartSound = NULL;
	}
	if ( m_pFiringLoop )
	{
		controller.SoundDestroy( m_pFiringLoop );
		m_pFiringLoop = NULL;
	}
	if ( m_pPilotLightSound )
	{
		controller.SoundDestroy( m_pPilotLightSound );
		m_pPilotLightSound = NULL;
	}
	if ( m_pHitTargetSound )
	{
		controller.SoundDestroy( m_pHitTargetSound );
		m_pHitTargetSound = NULL;
	}

	m_bHitTarget = false;
	m_bOldHitTarget = false;
#endif

}
void CTFFlameThrower::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_iWeaponState = FT_STATE_IDLE;
	m_bCritFire = false;
	m_bHitTarget = false;
	m_flStartFiringTime = 0;
	m_flAmmoUseRemainder = 0;

#ifdef GAME_DLL
	m_flStopHitSoundTime = 0.0f;
#endif

	DestroySounds();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::Precache( void )
{
	BaseClass::Precache();
	PrecacheParticleSystem( "pyro_blast" );
	PrecacheScriptSound( "Weapon_FlameThrower.AirBurstAttack" );
	PrecacheScriptSound( "TFPlayer.AirBlastImpact" );
	PrecacheScriptSound( "TFPlayer.FlameOut" );
	PrecacheScriptSound( "Weapon_FlameThrower.AirBurstAttackDeflect" );
	PrecacheScriptSound( "Weapon_FlameThrower.FireHit" );
	PrecacheParticleSystem( "deflect_fx" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::Spawn( void )
{
	m_iAltFireHint = HINT_ALTFIRE_FLAMETHROWER;
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFFlameThrower::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_iWeaponState = FT_STATE_IDLE;
	m_bCritFire = false;
	m_bHitTarget = false;

#if defined ( CLIENT_DLL )
	StopFlame();
	StopPilotLight();
#endif

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::ItemPostFrame()
{
	if ( m_bLowered )
		return;

	// Get the player owning the weapon.
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( !pOwner )
		return;

	int iAmmo = pOwner->GetAmmoCount( m_iPrimaryAmmoType );
	bool bFired = false;

	if ( ( pOwner->m_nButtons & IN_ATTACK2 ) && m_flNextSecondaryAttack <= gpGlobals->curtime )
	{
		float flAmmoPerSecondaryAttack = TF_FLAMETHROWER_AMMO_PER_SECONDARY_ATTACK;
		CALL_ATTRIB_HOOK_FLOAT( flAmmoPerSecondaryAttack, mult_airblast_cost );

		if ( iAmmo >= flAmmoPerSecondaryAttack )
		{
			SecondaryAttack();
			bFired = true;
		}
	}
	else if ( ( pOwner->m_nButtons & IN_ATTACK ) && iAmmo > 0 && m_iWeaponState != FT_STATE_AIRBLASTING )
	{
		PrimaryAttack();
		bFired = true;
	}

	if ( !bFired )
	{
		if ( m_iWeaponState > FT_STATE_IDLE )
		{
			SendWeaponAnim( ACT_MP_ATTACK_STAND_POSTFIRE );
			pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_POST );
			m_iWeaponState = FT_STATE_IDLE;
			m_bCritFire = false;
			m_bHitTarget = false;
		}

		if ( !ReloadOrSwitchWeapons() )
		{
			WeaponIdle();
		}
	}

	//BaseClass::ItemPostFrame();
}

class CTraceFilterIgnoreObjects : public CTraceFilterSimple
{
public:
	// It does have a base, but we'll never network anything below here..
	DECLARE_CLASS( CTraceFilterIgnoreObjects, CTraceFilterSimple );

	CTraceFilterIgnoreObjects( const IHandleEntity *passentity, int collisionGroup )
		: CTraceFilterSimple( passentity, collisionGroup )
	{
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		CBaseEntity *pEntity = EntityFromEntityHandle( pServerEntity );

		if ( pEntity && ( pEntity->IsBaseObject() || pEntity->IsNPC() || ( pEntity->GetMoveType() == MOVETYPE_VPHYSICS || ( pEntity->VPhysicsGetObject() ) ) ) )
			return false;

		return BaseClass::ShouldHitEntity( pServerEntity, contentsMask );
	}
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::PrimaryAttack()
{
	// Are we capable of firing again?
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return;

	// Get the player owning the weapon.
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( !pOwner )
		return;

	if ( !CanAttack() )
	{
#if defined ( CLIENT_DLL )
		StopFlame();
#endif
		m_iWeaponState = FT_STATE_IDLE;
		return;
	}

	CalcIsAttackCritical();

	// Because the muzzle is so long, it can stick through a wall if the player is right up against it.
	// Make sure the weapon can't fire in this condition by tracing a line between the eye point and the end of the muzzle.
	trace_t trace;	
	Vector vecEye = pOwner->EyePosition();
	Vector vecMuzzlePos = GetVisualMuzzlePos();
	CTraceFilterIgnoreObjects traceFilter( this, COLLISION_GROUP_NONE );
	UTIL_TraceLine( vecEye, vecMuzzlePos, MASK_SOLID, &traceFilter, &trace );
	if ( trace.fraction < 1.0 && ( !trace.m_pEnt || trace.m_pEnt->m_takedamage == DAMAGE_NO ) )
	{
		// there is something between the eye and the end of the muzzle, most likely a wall, don't fire, and stop firing if we already are
		if ( m_iWeaponState > FT_STATE_IDLE )
		{
#if defined ( CLIENT_DLL )
			StopFlame();
#endif
			m_iWeaponState = FT_STATE_IDLE;
		}
		return;
	}

	switch ( m_iWeaponState )
	{
	case FT_STATE_IDLE:
	case FT_STATE_AIRBLASTING:
		{
			// Just started, play PRE and start looping view model anim

			pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRE );

			SendWeaponAnim( ACT_VM_PRIMARYATTACK );

			m_flStartFiringTime = gpGlobals->curtime + 0.16;	// 5 frames at 30 fps

			m_iWeaponState = FT_STATE_STARTFIRING;
		}
		break;
	case FT_STATE_STARTFIRING:
		{
			// if some time has elapsed, start playing the looping third person anim
			if ( gpGlobals->curtime > m_flStartFiringTime )
			{
				m_iWeaponState = FT_STATE_FIRING;
				m_flNextPrimaryAttackAnim = gpGlobals->curtime;
			}
		}
		break;
	case FT_STATE_FIRING:
		{
			if ( gpGlobals->curtime >= m_flNextPrimaryAttackAnim )
			{
				pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
				m_flNextPrimaryAttackAnim = gpGlobals->curtime + 1.4;		// fewer than 45 frames!
			}
		}
		break;

	default:
		break;
	}

#ifdef CLIENT_DLL
	// Restart our particle effect if we've transitioned across water boundaries
	if ( m_iParticleWaterLevel != -1 && pOwner->GetWaterLevel() != m_iParticleWaterLevel )
	{
		if ( m_iParticleWaterLevel == WL_Eyes || pOwner->GetWaterLevel() == WL_Eyes )
		{
			RestartParticleEffect();
		}
	}
#endif

#ifdef CLIENT_DLL
	// Handle the flamethrower light
	if (lfe_muzzlelight.GetBool())
	{
		dlight_t *dl = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );
		dl->origin = vecMuzzlePos;
		dl->die = gpGlobals->curtime + 0.01f;
		if ( m_bCritFire )
		{
			dl->color.r = 255;
			dl->color.g = 110;
			dl->color.b = 10;
			dl->radius = 400.f;
			dl->decay = 512.0f;
			dl->style = 1;
		}
		else
		{
			dl->color.r = 255;
			dl->color.g = 100;
			dl->color.b = 10;
			dl->radius = 340.f;
			dl->decay = 512.0f;
			dl->style = 1;
		}
	}
#endif

#ifdef GAME_DLL
	if ( m_bCritFire )
	{
		CSoundEnt::InsertSound( SOUND_DANGER, GetAbsOrigin(), 500, 3.0 ); // CRIT IS DANGEROUS NOW PANIC!
	}
	else
	{
		CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 400, 3.0 );
	}
#endif

#if !defined (CLIENT_DLL)
	// Let the player remember the usercmd he fired a weapon on. Assists in making decisions about lag compensation.
	pOwner->NoteWeaponFired();

	pOwner->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pOwner, m_bCritFire );

	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( pOwner, pOwner->GetCurrentCommand() );
#endif

	float flFiringInterval = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flFiringInterval, mult_postfiredelay );

	// Don't attack if we're underwater
	if ( pOwner->GetWaterLevel() != WL_Eyes )
	{
		// Find eligible entities in a cone in front of us.
		Vector vOrigin = pOwner->Weapon_ShootPosition();
		Vector vForward, vRight, vUp;
		QAngle vAngles = pOwner->EyeAngles() + pOwner->GetPunchAngle();
		AngleVectors( vAngles, &vForward, &vRight, &vUp );

		#define NUM_TEST_VECTORS	30

#ifdef CLIENT_DLL
		bool bWasCritical = m_bCritFire;
#endif

		// Burn & Ignite 'em
		int iDmgType = g_aWeaponDamageTypes[ GetWeaponID() ];
		m_bCritFire = IsCurrentAttackACrit();
		if ( m_bCritFire )
		{
			iDmgType |= DMG_CRITICAL;
		}

		if ( IsCurrentAttackAMiniCrit() )
		{
			iDmgType |= DMG_MINICRITICAL;
		}

#ifdef CLIENT_DLL
		if ( bWasCritical != m_bCritFire )
		{
			RestartParticleEffect();
		}
#endif


#ifdef GAME_DLL
		// create the flame entity
		int iDamagePerSec = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_nDamage;
		float flDamage = (float)iDamagePerSec * flFiringInterval;
		CALL_ATTRIB_HOOK_FLOAT( flDamage, mult_dmg );
		CTFFlameEntity::Create( GetFlameOriginPos(), pOwner->EyeAngles(), this, iDmgType, flDamage );
#endif
	}

#ifdef GAME_DLL
	// Figure how much ammo we're using per shot and add it to our remainder to subtract.  (We may be using less than 1.0 ammo units
	// per frame, depending on how constants are tuned, so keep an accumulator so we can expend fractional amounts of ammo per shot.)
	// Note we do this only on server and network it to client.  If we predict it on client, it can get slightly out of sync w/server
	// and cause ammo pickup indicators to appear
	
	float flAmmoPerSecond = TF_FLAMETHROWER_AMMO_PER_SECOND_PRIMARY_ATTACK;
	CALL_ATTRIB_HOOK_FLOAT( flAmmoPerSecond, mult_flame_ammopersec );

	m_flAmmoUseRemainder += flAmmoPerSecond * flFiringInterval;
	// take the integer portion of the ammo use accumulator and subtract it from player's ammo count; any fractional amount of ammo use
	// remains and will get used in the next shot
	int iAmmoToSubtract = (int) m_flAmmoUseRemainder;
	if ( iAmmoToSubtract > 0 )
	{
		pOwner->RemoveAmmo( iAmmoToSubtract, m_iPrimaryAmmoType );
		m_flAmmoUseRemainder -= iAmmoToSubtract;
		// round to 2 digits of precision
		m_flAmmoUseRemainder = (float) ( (int) (m_flAmmoUseRemainder * 100) ) / 100.0f;
	}
#endif

	m_flNextPrimaryAttack = gpGlobals->curtime + flFiringInterval;
	m_flTimeWeaponIdle = gpGlobals->curtime + flFiringInterval;

#if !defined (CLIENT_DLL)
	lagcompensation->FinishLagCompensation( pOwner );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::SecondaryAttack()
{
	if ( !lfe_allow_airblast.GetBool() )
		return;

	int iNoAirblast = 0;
	CALL_ATTRIB_HOOK_FLOAT( iNoAirblast, set_flamethrower_push_disabled );
	if ( iNoAirblast )
		return;

	// Get the player owning the weapon.
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( !pOwner )
		return;

	if ( !CanAttack() )
	{
		m_iWeaponState = FT_STATE_IDLE;
		return;
	}

#ifdef CLIENT_DLL
	StopFlame();
#endif

	// i dunno how will this thing work since it won't even work in live tf2.
	int iChargedAirblast = 0;
	CALL_ATTRIB_HOOK_FLOAT( iChargedAirblast, set_charged_airblast );
	if ( iChargedAirblast != 1 )
	{
	m_iWeaponState = FT_STATE_AIRBLASTING;
	SendWeaponAnim( ACT_VM_SECONDARYATTACK );
	WeaponSound( WPN_DOUBLE );

#ifdef CLIENT_DLL
	if ( prediction->IsFirstTimePredicted() )
	{
		StartFlame();
	}
#else
	// Let the player remember the usercmd he fired a weapon on. Assists in making decisions about lag compensation.
	pOwner->NoteWeaponFired();

	pOwner->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pOwner, false );

	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( pOwner, pOwner->GetCurrentCommand() );

	Vector vecDir = pOwner->EyeDirection3D();
	QAngle angDir = pOwner->EyeAngles();
	AngleVectors( angDir, &vecDir );

	const Vector vecBlastSize = Vector( 128, 128, 64 );

	// Picking max out of length, width, height for airblast distance.
	float flBlastDist = max( max( vecBlastSize.x, vecBlastSize.y ), vecBlastSize.z );

	Vector vecOrigin = pOwner->Weapon_ShootPosition() + vecDir * flBlastDist;

	CBaseEntity *pList[64];

	int count = UTIL_EntitiesInBox( pList, 64, vecOrigin - vecBlastSize, vecOrigin + vecBlastSize, 0 );

	if ( lfe_debug_airblast.GetBool() )
	{
		NDebugOverlay::Box( vecOrigin, -vecBlastSize, vecBlastSize, 0, 0, 255, 100, 2.0 );
	}

	for ( int i = 0; i < count; i++ )
	{
		CBaseEntity *pEntity = pList[i];

		if ( !pEntity )
			continue;

		if ( pEntity == pOwner )
			continue;

		if ( !pEntity->IsDeflectable() )
			continue;

		// Make sure we can actually see this entity so we don't hit anything through walls.
		trace_t tr;
		UTIL_TraceLine( pOwner->Weapon_ShootPosition(), pEntity->WorldSpaceCenter(), MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr );
		if ( tr.fraction != 1.0f )
			continue;


		if ( pEntity->IsPlayer() )
		{
			if ( !pEntity->IsAlive() )
				continue;

			CTFPlayer *pTFPlayer = ToTFPlayer( pEntity );

			Vector vecPushDir = pOwner->EyeDirection3D();
			QAngle angPushDir = angDir;

			// Push them at least 45 degrees up.
			angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

			AngleVectors( angPushDir, &vecPushDir );

			DeflectPlayer( pTFPlayer, pOwner, vecPushDir );
		}
		else if ( pEntity->IsNPC() ) // push npcs
		{
			if ( !pEntity->IsAlive() )
				continue;

			CAI_BaseNPC *pNPC = pEntity->MyNPCPointer();

			Vector vecPushDir = pOwner->EyeDirection3D();
			QAngle angPushDir = angDir;

			// Push them at least 45 degrees up.
			angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

			AngleVectors( angPushDir, &vecPushDir );

			DeflectNPC( pNPC, pOwner, vecPushDir );
		}
		else if( pEntity->IsNPC() && pEntity->Classify() != CLASS_HEADCRAB && !FClassnameIs(pEntity, "npc_antlion") ) // gravity gun like  push antlions
		{
			if ( !pEntity->IsAlive() )
				continue;

			CNPC_Antlion *pAnt = dynamic_cast<CNPC_Antlion *>(pEntity);

			Vector	vecShoveDir = vecDir;
			vecShoveDir.z = 0.0f;
			//pAnt->SetCondition( COND_ANTLION_FLIPPED );
			pAnt->ApplyAbsVelocityImpulse( ( vecShoveDir * random->RandomInt( 50.0f, 100.0f ) ) + Vector(0,0,64.0f) );
			pAnt->SetGroundEntity( NULL );
			pAnt->Flip();
		}
		else if ( pEntity->VPhysicsGetObject() ) // push physics?
		{
			Vector vecPos = pEntity->GetAbsOrigin();
			Vector vecDeflect;

			DeflectPhysics( pEntity, pOwner, vecDeflect );
		}
		else
		{
			// Deflect projectile to the point that we're aiming at, similar to rockets.
			Vector vecPos = pEntity->GetAbsOrigin();
			Vector vecDeflect;
			GetProjectileReflectSetup( GetTFPlayerOwner(), vecPos, &vecDeflect, false );

			int nDestoryProj = 0;
			int nDeflectDisabled = 0;
			CALL_ATTRIB_HOOK_INT( nDestoryProj, airblast_destroy_projectile );
			CALL_ATTRIB_HOOK_INT( nDeflectDisabled, airblast_deflect_projectiles_disabled );
			if ( nDestoryProj != 0 && nDeflectDisabled !=0 )
			{
				// place holder
				DeflectEntity( pEntity, pOwner, vecDeflect );
			}
			else
			{
				DeflectEntity( pEntity, pOwner, vecDeflect );
			}
		}

		CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 300, 3.0 );
	}

	lagcompensation->FinishLagCompensation( pOwner );
#endif

	float flAmmoPerSecondaryAttack = TF_FLAMETHROWER_AMMO_PER_SECONDARY_ATTACK;
	CALL_ATTRIB_HOOK_FLOAT( flAmmoPerSecondaryAttack, mult_airblast_cost );

	pOwner->RemoveAmmo( flAmmoPerSecondaryAttack, m_iPrimaryAmmoType );

	// Don't allow firing immediately after airblasting.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 0.75f;
	}
}

#ifdef GAME_DLL
void CTFFlameThrower::DeflectEntity( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir )
{
	if ( !TFGameRules() )
		return;

	if ( ( pEntity->GetTeamNumber() == pAttacker->GetTeamNumber() ) && !TFGameRules()->IsFriendlyFire() )
		return;

	pEntity->Deflected( pAttacker, vecDir );
	pEntity->EmitSound( "Weapon_FlameThrower.AirBurstAttackDeflect" );
}

void CTFFlameThrower::DeflectPlayer( CTFPlayer *pVictim, CTFPlayer *pAttacker, Vector &vecDir )
{
	if ( !pVictim )
		return;

	int nPushbackDisabled = 0;
	CALL_ATTRIB_HOOK_INT( nPushbackDisabled, airblast_pushback_disabled );

	if ( pVictim->InSameTeam( pAttacker ) && !TFGameRules()->IsFriendlyFire() )
	{
		if ( pVictim->m_Shared.InCond( TF_COND_BURNING ) )
		{
			// Extinguish teammates.
			pVictim->m_Shared.RemoveCond( TF_COND_BURNING );
			pVictim->EmitSound( "TFPlayer.FlameOut" );

			float flExReHp = 0.0f;
			CALL_ATTRIB_HOOK_FLOAT( flExReHp, extinguish_restores_health );

			if ( flExReHp )
			{
				int iHealthRestored = pAttacker->TakeHealth( flExReHp, DMG_GENERIC );

				if ( iHealthRestored )
				{
					IGameEvent *event = gameeventmanager->CreateEvent( "player_healonhit" );

					if ( event )
					{
						event->SetInt( "amount", iHealthRestored );
						event->SetInt( "entindex", pAttacker->entindex() );

						gameeventmanager->FireEvent( event );
					}
				}
			}
	
			CTF_GameStats.Event_PlayerAwardBonusPoints( pAttacker, pVictim, 1 );
		}
	}
	else if ( nPushbackDisabled != 1 )
	{
		// Don't push players if they're too far off to the side. Ignore Z.
		Vector vecVictimDir = pVictim->WorldSpaceCenter() - pAttacker->WorldSpaceCenter();

		Vector vecVictimDir2D( vecVictimDir.x, vecVictimDir.y, 0.0f );
		VectorNormalize( vecVictimDir2D );
	
		Vector vecDir2D( vecDir.x, vecDir.y, 0.0f );
		VectorNormalize( vecDir2D );

		float flDot = DotProduct( vecDir2D, vecVictimDir2D );
		if ( flDot >= 0.8 )
		{
			// Push enemy players.
			pVictim->SetGroundEntity( NULL );
			pVictim->SetAbsVelocity( vecDir * 500 );
			pVictim->EmitSound( "TFPlayer.AirBlastImpact" );
			pVictim->SetAirblastState( true );

			// Add pusher as recent damager so he can get a kill credit for pushing a player to his death.
			pVictim->AddDamagerToHistory( pAttacker );
		}
	}
}

void CTFFlameThrower::DeflectNPC( CAI_BaseNPC *pVictim, CTFPlayer *pAttacker, Vector &vecDir )
{
	if ( !pVictim )
		return;

	int nPushbackDisabled = 0;
	CALL_ATTRIB_HOOK_INT( nPushbackDisabled, airblast_pushback_disabled );

	if ( pVictim->InSameTeam( pAttacker ) && !TFGameRules()->IsHL1FriendlyFire() )
	{
		if ( pVictim->IsOnFire() )
		{
			// we should calling Extinguish instead of RemoveCond for npcs.
			pVictim->Extinguish();
			pVictim->EmitSound( "TFPlayer.FlameOut" );

			float flExReHp = 0.0f;
			CALL_ATTRIB_HOOK_FLOAT( flExReHp, extinguish_restores_health );

			if ( flExReHp )
			{
				int iHealthRestored = pAttacker->TakeHealth( flExReHp, DMG_GENERIC );

				if ( iHealthRestored )
				{
					IGameEvent *event = gameeventmanager->CreateEvent( "player_healonhit" );

					if ( event )
					{
						event->SetInt( "amount", iHealthRestored );
						event->SetInt( "entindex", pAttacker->entindex() );

						gameeventmanager->FireEvent( event );
					}
				}
			}

			CTF_GameStats.Event_PlayerAwardBonusPoints( pAttacker, pVictim, 1 );
		}
	}
	else if ( nPushbackDisabled != 1 )
	{
		// Don't push players if they're too far off to the side. Ignore Z.
		Vector vecVictimDir = pVictim->WorldSpaceCenter() - pAttacker->WorldSpaceCenter();

		Vector vecVictimDir2D( vecVictimDir.x, vecVictimDir.y, 0.0f );
		VectorNormalize( vecVictimDir2D );

		Vector vecDir2D( vecDir.x, vecDir.y, 0.0f );
		VectorNormalize( vecDir2D );

		float flDot = DotProduct( vecDir2D, vecVictimDir2D );
		if ( flDot >= 0.8 )
		{
			// Push enemy NPC.
			pVictim->SetGroundEntity( NULL );
			pVictim->SetAbsVelocity( vecDir * 500 );
			pVictim->EmitSound( "TFPlayer.AirBlastImpact" );
			pVictim->AddCond( TF_COND_KNOCKED_INTO_AIR, PERMANENT_CONDITION );

			// Add pusher as recent damager so he can get a kill credit for pushing a player to his death.
			pVictim->AddDamagerToHistory( pAttacker );
		}
		pVictim->Deflected( pAttacker, vecDir );
	}
}

void CTFFlameThrower::DeflectPhysics( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir ) // new one copy from physexplosion. // old one copy from deflected pipe grenade.
{
	int nPushbackDisabled = 0;
	CALL_ATTRIB_HOOK_INT( nPushbackDisabled, airblast_pushback_disabled );

	if ( lfe_allow_airblast_physics.GetBool() && nPushbackDisabled != 1 )
	{
		float		flDist;
		// iterate on all entities in the vicinity.
		// I've removed the traceline heuristic from phys explosions. SO right now they will
		// affect entities through walls. (sjb)
		// UNDONE: Try tracing world-only?
		//while ((pEntity = FindEntity( pEntity, pAttacker, NULL )) != NULL)
		//{
			while ((pEntity = gEntList.FindEntityInSphere( pEntity, GetAbsOrigin(), lfe_debug_airblast_physics_distance.GetFloat() )) != NULL)
			{
			// UNDONE: Ask the object if it should get force if it's not MOVETYPE_VPHYSICS?
			//if ( pEntity->m_takedamage != DAMAGE_NO && (pEntity->GetMoveType() == MOVETYPE_VPHYSICS || ( pEntity->VPhysicsGetObject() )) )
			//{
				flDist = (pEntity->WorldSpaceCenter() - GetAbsOrigin()).Length();

					CTakeDamageInfo info( this, this, 2, DMG_BLAST );
					CalculateExplosiveDamageForce( &info, (pEntity->GetAbsOrigin() - GetAbsOrigin()), pEntity->GetAbsOrigin() );
					
					if ( (pEntity->GetAbsOrigin() - GetAbsOrigin()).Length2D() <= lfe_debug_airblast_physics_distance.GetFloat() )
					{
						if ( /*pEntity->m_takedamage != DAMAGE_NO &&*/ pEntity->GetMoveType() == MOVETYPE_VPHYSICS || (pEntity->VPhysicsGetObject() && !pEntity->IsPlayer()) ) 
						{
							IPhysicsObject *pPhysObject = pEntity->VPhysicsGetObject();

							if ( pPhysObject )
							{
								float flMass = pPhysObject->GetMass();

								if ( flMass <= lfe_debug_airblast_physics_mass.GetFloat() )
								{
									// Increase the vertical lift of the force
									Vector vecForce = info.GetDamageForce();
									vecForce.z *= lfe_debug_airblast_physics_force.GetFloat();
									info.SetDamageForce( vecForce );

									pEntity->VPhysicsTakeDamage( info );
									//pEntity->Deflected( pAttacker, vecDir );
									pEntity->EmitSound( "TFPlayer.AirBlastImpact" ); // Weapon_FlameThrower.AirBurstAttackDeflect = earrape.
								}
							}
						}
					}
			}
			//}
		//}
		

		/*
		if ( (pEntity->GetAbsOrigin() - GetAbsOrigin()).Length2D() <= lfe_debug_airblast_physics_distance.GetFloat() )
		{
			IPhysicsObject *pPhysicsObject = pEntity->VPhysicsGetObject();
			if ( pPhysicsObject )
			{
				Vector vecOldVelocity, vecVelocity;

				pPhysicsObject->GetVelocity( &vecOldVelocity, NULL );

				float flSpeed = vecOldVelocity.Length();

				vecVelocity = vecDir;
				vecVelocity *= flSpeed;
				AngularImpulse angVelocity( ( 600, random->RandomInt( -1200, 1200 ), 0 ) );

				// Now change physics's direction.
				pPhysicsObject->SetVelocityInstantaneous( &vecVelocity, &angVelocity );
			}
		}
		*/
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFFlameThrower::Lower( void )
{
	if ( BaseClass::Lower() )
	{
		// If we were firing, stop
		if ( m_iWeaponState > FT_STATE_IDLE )
		{
			SendWeaponAnim( ACT_MP_ATTACK_STAND_POSTFIRE );
			m_iWeaponState = FT_STATE_IDLE;
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the position of the tip of the muzzle at it appears visually
//-----------------------------------------------------------------------------
Vector CTFFlameThrower::GetVisualMuzzlePos()
{
	return GetMuzzlePosHelper( true );
}

//-----------------------------------------------------------------------------
// Purpose: Returns the position at which to spawn flame damage entities
//-----------------------------------------------------------------------------
Vector CTFFlameThrower::GetFlameOriginPos()
{
	return GetMuzzlePosHelper( false );
}

//-----------------------------------------------------------------------------
// Purpose: Returns the position of the tip of the muzzle
//-----------------------------------------------------------------------------
Vector CTFFlameThrower::GetMuzzlePosHelper( bool bVisualPos )
{
	Vector vecMuzzlePos;
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( pOwner ) 
	{
		Vector vecForward, vecRight, vecUp;
		AngleVectors( pOwner->GetAbsAngles(), &vecForward, &vecRight, &vecUp );
		vecMuzzlePos = pOwner->Weapon_ShootPosition();
		vecMuzzlePos +=  vecRight * TF_FLAMETHROWER_MUZZLEPOS_RIGHT;
		// if asking for visual position of muzzle, include the forward component
		if ( bVisualPos )
		{
			vecMuzzlePos +=  vecForward * TF_FLAMETHROWER_MUZZLEPOS_FORWARD;
		}
	}
	return vecMuzzlePos;
}

#if defined( CLIENT_DLL )

bool CTFFlameThrower::Deploy( void )
{
	StartPilotLight();

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);

	if ( IsCarrierAlive() && ( WeaponState() == WEAPON_IS_ACTIVE ) && ( GetPlayerOwner()->GetAmmoCount( m_iPrimaryAmmoType ) > 0 ) )
	{
		if ( m_iWeaponState > FT_STATE_IDLE )
		{
			if ( m_iWeaponState != FT_STATE_AIRBLASTING || !GetPlayerOwner()->IsLocalPlayer() )
			{
				StartFlame();
			}
		}
		else
		{
			StartPilotLight();
		}		
	}
	else 
	{
		StopFlame();
		StopPilotLight();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::UpdateOnRemove( void )
{
	StopFlame();
	StopPilotLight();

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::SetDormant( bool bDormant )
{
	// If I'm going from active to dormant and I'm carried by another player, stop our firing sound.
	if ( !IsCarriedByLocalPlayer() )
	{
		if ( !IsDormant() && bDormant )
		{
			StopFlame();
			StopPilotLight();
		}
	}

	// Deliberately skip base combat weapon to avoid being holstered
	C_BaseEntity::SetDormant( bDormant );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::StartFlame()
{
	if ( m_iWeaponState == FT_STATE_AIRBLASTING )
	{
		C_BaseEntity *pModel = GetWeaponForEffect();

		if ( pModel )
		{
			pModel->ParticleProp()->Create( "pyro_blast", PATTACH_POINT_FOLLOW, "muzzle" );
		}

		//CLocalPlayerFilter filter;
		//EmitSound( filter, entindex(), "Weapon_FlameThrower.AirBurstAttack" );
	}
	else
	{
		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

		// normally, crossfade between start sound & firing loop in 3.5 sec
		float flCrossfadeTime = 3.5;

		if ( m_pFiringLoop && ( m_bCritFire != m_bFiringLoopCritical ) )
		{
			// If we're firing and changing between critical & noncritical, just need to change the firing loop.
			// Set crossfade time to zero so we skip the start sound and go to the loop immediately.

			flCrossfadeTime = 0;
			StopFlame( true );
		}

		StopPilotLight();

		if ( !m_pFiringStartSound && !m_pFiringLoop )
		{
			RestartParticleEffect();
			CLocalPlayerFilter filter;

			// Play the fire start sound
			const char *shootsound = GetShootSound( SINGLE );
			if ( flCrossfadeTime > 0.0 )
			{
				// play the firing start sound and fade it out
				m_pFiringStartSound = controller.SoundCreate( filter, entindex(), shootsound );
				controller.Play( m_pFiringStartSound, 1.0, 100 );
				controller.SoundChangeVolume( m_pFiringStartSound, 0.0, flCrossfadeTime );
			}

			// Start the fire sound loop and fade it in
			if ( m_bCritFire )
			{
				shootsound = GetShootSound( BURST );
			}
			else
			{
				shootsound = GetShootSound( SPECIAL1 );
			}
			m_pFiringLoop = controller.SoundCreate( filter, entindex(), shootsound );
			m_bFiringLoopCritical = m_bCritFire;

			// play the firing loop sound and fade it in
			if ( flCrossfadeTime > 0.0 )
			{
				controller.Play( m_pFiringLoop, 0.0, 100 );
				controller.SoundChangeVolume( m_pFiringLoop, 1.0, flCrossfadeTime );
			}
			else
			{
				controller.Play( m_pFiringLoop, 1.0, 100 );
			}
		}

		if( m_bHitTarget != m_bOldHitTarget )
		{
			if ( m_bHitTarget )
			{
				CLocalPlayerFilter filter;
				m_pHitTargetSound = controller.SoundCreate( filter, entindex(), "Weapon_FlameThrower.FireHit" );
				controller.Play( m_pHitTargetSound, 1.0f, 100.0f );
			}
			else if ( m_pHitTargetSound )
			{
				controller.SoundDestroy( m_pHitTargetSound );
				m_pHitTargetSound = NULL;
			}

			m_bOldHitTarget = m_bHitTarget;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::StopFlame( bool bAbrupt /* = false */ )
{
	if ( ( m_pFiringLoop || m_pFiringStartSound ) && !bAbrupt )
	{
		// play a quick wind-down poof when the flame stops
		CLocalPlayerFilter filter;
		const char *shootsound = GetShootSound( SPECIAL3 );
		EmitSound( filter, entindex(), shootsound );
	}

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( m_pFiringLoop )
	{
		controller.SoundDestroy( m_pFiringLoop );
		m_pFiringLoop = NULL;
	}

	if ( m_pFiringStartSound )
	{
		controller.SoundDestroy( m_pFiringStartSound );
		m_pFiringStartSound = NULL;
	}

	if ( m_pFlameEffect )
	{
		if ( m_hFlameEffectHost.Get() )
		{
			m_hFlameEffectHost->ParticleProp()->StopEmission( m_pFlameEffect );
			m_hFlameEffectHost = NULL;
		}

		m_pFlameEffect = NULL;
	}

	if ( !bAbrupt )
	{
		if ( m_pHitTargetSound )
		{
			controller.SoundDestroy( m_pHitTargetSound );
			m_pHitTargetSound = NULL;
		}

		m_bOldHitTarget = false;
		m_bHitTarget = false;
	}

	m_iParticleWaterLevel = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::StartPilotLight()
{
	if ( !m_pPilotLightSound )
	{
		StopFlame();

		// Create the looping pilot light sound
		const char *pilotlightsound = GetShootSound( SPECIAL2 );
		CLocalPlayerFilter filter;

		CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
		m_pPilotLightSound = controller.SoundCreate( filter, entindex(), pilotlightsound );

		controller.Play( m_pPilotLightSound, 1.0, 100 );
	}	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::StopPilotLight()
{
	if ( m_pPilotLightSound )
	{
		CSoundEnvelopeController::GetController().SoundDestroy( m_pPilotLightSound );
		m_pPilotLightSound = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameThrower::RestartParticleEffect( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( !pOwner )
		return;

	if ( m_pFlameEffect && m_hFlameEffectHost.Get() )
	{
		m_hFlameEffectHost->ParticleProp()->StopEmission( m_pFlameEffect );
	}

	m_iParticleWaterLevel = pOwner->GetWaterLevel();
	int iTeam = pOwner->GetTeamNumber();

	// Start the appropriate particle effect
	const char *pszParticleEffect;
	if ( pOwner->GetWaterLevel() == WL_Eyes )
	{
		pszParticleEffect = "flamethrower_underwater";
	}
	else
	{
		if ( m_bCritFire )
		{
			// GREEN FLAMES
			int nGreenFlames = 0;
			CALL_ATTRIB_HOOK_INT( nGreenFlames, halloween_green_flames__halloween );
			if ( nGreenFlames != 0 )
			{
				pszParticleEffect = ConstructTeamParticle( "flamethrower_halloween_crit_%s", iTeam, true );
			}
			else
			{
				pszParticleEffect = ConstructTeamParticle( "flamethrower_crit_%s", iTeam, true );
			}

			// THE NO SKILL FLAMES
			int nPhlog = 0;
			CALL_ATTRIB_HOOK_INT( nPhlog, burn_damage_earns_rage );
			if ( nPhlog != 0 )
			{
				pszParticleEffect = "flamethrower_crit";
			}
			else
			{
				pszParticleEffect = ConstructTeamParticle( "flamethrower_crit_%s", iTeam, true );
			}
		}
		else 
		{
			// GREEN FLAMES
			int nGreenFlames = 0;
			CALL_ATTRIB_HOOK_INT( nGreenFlames, halloween_green_flames__halloween );
			if ( nGreenFlames == 1 )
			{
				pszParticleEffect = "flamethrower_halloween";
			}
			else
			{
				pszParticleEffect = "flamethrower";
			}

			// THE NO SKILL FLAMES
			int nPhlog = 0;
			CALL_ATTRIB_HOOK_INT( nPhlog, burn_damage_earns_rage );
			if ( nPhlog == 1 )
			{
				pszParticleEffect = "flamethrower";
			}
			else
			{
				pszParticleEffect = "flamethrower";
			}
		}		
	}

	// Start the effect on the viewmodel if our owner is the local player
	C_BaseEntity *pModel = GetWeaponForEffect();

	if ( pModel )
	{
		m_pFlameEffect = pModel->ParticleProp()->Create( pszParticleEffect, PATTACH_POINT_FOLLOW, "muzzle" );
		m_hFlameEffectHost = pModel;
	}
}

#else
//-----------------------------------------------------------------------------
// Purpose: Notify client that we're hitting an enemy.
//-----------------------------------------------------------------------------
void CTFFlameThrower::SetHitTarget( void )
{
	if ( m_iWeaponState > FT_STATE_IDLE )
	{
		if ( !m_bHitTarget )
			m_bHitTarget = true;

		m_flStopHitSoundTime = gpGlobals->curtime + 0.2f;
		SetContextThink( &CTFFlameThrower::HitTargetThink, gpGlobals->curtime + 0.1f, "FlameThrowerHitTargetThink" );
	}
}

void CTFFlameThrower::HitTargetThink( void )
{
	if ( m_flStopHitSoundTime != 0.0f && m_flStopHitSoundTime > gpGlobals->curtime )
	{
		m_bHitTarget = false;
		m_flStopHitSoundTime = 0.0f;
		SetContextThink( NULL, 0, "FlameThrowerHitTargetThink" );
	}
	else
	{
		SetContextThink( &CTFFlameThrower::HitTargetThink, gpGlobals->curtime + 0.1f, "FlameThrowerHitTargetThink" );
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFFlameThrower::IsBehindAndFacingTarget( CBaseEntity *pTarget )
{
	Assert( pTarget );

	// Get the forward view vector of the target, ignore Z
	Vector vecVictimForward;
	AngleVectors( pTarget->EyeAngles(), &vecVictimForward );
	vecVictimForward.z = 0.0f;
	vecVictimForward.NormalizeInPlace();

	// Get a vector from my origin to my targets origin
	Vector vecToTarget;
	vecToTarget = pTarget->WorldSpaceCenter() - GetOwner()->WorldSpaceCenter();
	vecToTarget.z = 0.0f;
	vecToTarget.NormalizeInPlace();

	// Get a forward vector of the attacker.
	Vector vecOwnerForward;
	AngleVectors( GetOwner()->EyeAngles(), &vecOwnerForward );
	vecOwnerForward.z = 0.0f;
	vecOwnerForward.NormalizeInPlace();

	float flDotOwner = DotProduct( vecOwnerForward, vecToTarget );
	float flDotVictim = DotProduct( vecVictimForward, vecToTarget );

	// Make sure they're actually facing the target.
	// This needs to be done because lag compensation can place target slightly behind the attacker.
	if ( flDotOwner > 1.0 )
		return ( flDotVictim > -0.1 );

	return false;
}
