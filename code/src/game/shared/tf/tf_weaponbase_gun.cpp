//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Weapon Base Gun 
//
//=============================================================================

#include "cbase.h"
#include "tf_weaponbase_gun.h"
#include "tf_fx_shared.h"
#include "effect_dispatch_data.h"
#include "takedamageinfo.h"
#include "tf_projectile_nail.h"
#include "tf_projectile_arrow.h"
#include "in_buttons.h"
#include "tf_weapon_jar.h"
#if !defined( CLIENT_DLL )	// Server specific.

	#include "tf_gamestats.h"
	#include "tf_player.h"
	#include "tf_fx.h"
	#include "te_effect_dispatch.h"

	#include "tf_projectile_rocket.h"
	#include "tf_weapon_grenade_pipebomb.h"
	#include "tf_weapon_grenade_stickybomb.h"
	#include "tf_projectile_flare.h"
	#include "tf_projectile_dragons_fury.h"
	#include "tf_weapon_grenade_mirv.h"
	#include "te.h"
	#include "soundent.h"
	#include "grenade_frag.h"
	#include "prop_combine_ball.h"
	#include "grenade_ar2.h"
	#include "grenade_spit.h"

#else	// Client specific.

	#include "c_tf_player.h"
	#include "c_te_effect_dispatch.h"

#endif

//=============================================================================
//
// TFWeaponBase Gun tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponBaseGun, DT_TFWeaponBaseGun )

BEGIN_NETWORK_TABLE( CTFWeaponBaseGun, DT_TFWeaponBaseGun )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iBurstSize ) ),
#else
	SendPropInt( SENDINFO( m_iBurstSize ), -1, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponBaseGun )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_iBurstSize, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

// Server specific.
#if !defined( CLIENT_DLL ) 
BEGIN_DATADESC( CTFWeaponBaseGun )
	DEFINE_THINKFUNC( ZoomOutIn ),
	DEFINE_THINKFUNC( ZoomOut ),
	DEFINE_THINKFUNC( ZoomIn ),
END_DATADESC()
#endif

//=============================================================================
//
// TFWeaponBase Gun functions.
//

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFWeaponBaseGun::CTFWeaponBaseGun()
{
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::ItemPostFrame( void )
{
	int iOldBurstSize = m_iBurstSize;
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner )
	{
		if ( m_iBurstSize > 0 )
		{
			// Fake the fire button.
			pOwner->m_nButtons |= IN_ATTACK;
		}
	}

	BaseClass::ItemPostFrame();

	// Stop burst if we run out of ammo.
	if ( ( UsesClipsForAmmo1() && m_iClip1 <= 0 ) ||
		( !UsesClipsForAmmo1() && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 ) ) 
	{
		m_iBurstSize = 0;
	}

	if ( iOldBurstSize > 0 && m_iBurstSize == 0 )
	{
		// Delay the next burst.
		m_flNextPrimaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( TF_WEAPON_PRIMARY_MODE ).m_flBurstDelay;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::PrimaryAttack( void )
{
	// Check for ammunition.
	if ( m_iClip1 <= 0 && UsesClipsForAmmo1() )
		return;

	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	if ( m_pWeaponInfo->GetWeaponData( TF_WEAPON_PRIMARY_MODE ).m_nBurstSize > 0 && m_iBurstSize == 0 )
	{
		// Start the burst.
		m_iBurstSize = m_pWeaponInfo->GetWeaponData( TF_WEAPON_PRIMARY_MODE ).m_nBurstSize;
	}

	if ( m_iBurstSize > 0 )
	{
		m_iBurstSize--;
	}

	CalcIsAttackCritical();

#ifndef CLIENT_DLL
	pPlayer->RemoveInvisibility();
	pPlayer->RemoveDisguise();

	// Minigun has custom handling
	if ( GetWeaponID() != TF_WEAPON_MINIGUN )
	{
		pPlayer->SpeakWeaponFire();
	}
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set the weapon mode.
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

#if !defined( CLIENT_DLL )
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pPlayer );
#endif

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	FireProjectile( pPlayer );

	m_flLastFireTime = gpGlobals->curtime;

	// Set next attack times.
	float flFireDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flFireDelay, mult_postfiredelay );

	m_flNextPrimaryAttack = gpGlobals->curtime + flFireDelay;

	// Don't push out secondary attack, because our secondary fire
	// systems are all separate from primary fire (sniper zooming, demoman pipebomb detonating, etc)
	//m_flNextSecondaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;

	// Set the idle animation times based on the sequence duration, so that we play full fire animations
	// that last longer than the refire rate may allow.
	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	AbortReload();
}	

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::SecondaryAttack( void )
{
	// semi-auto behaviour
	if ( m_bInAttack2 )
		return;

	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	pPlayer->DoClassSpecialSkill();

	m_bInAttack2 = true;

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
}

CBaseEntity *CTFWeaponBaseGun::FireProjectile( CTFPlayer *pPlayer )
{
	int iProjectile = TF_PROJECTILE_NONE;

	CALL_ATTRIB_HOOK_INT( iProjectile, override_projectile_type );

	if ( !iProjectile )
		iProjectile = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iProjectile;

	CBaseEntity *pProjectile = NULL;

	switch( iProjectile )
	{
	case TF_PROJECTILE_BULLET:
		FireBullet( pPlayer );
		break;

	case TF_PROJECTILE_ROCKET:
		pProjectile = FireRocket( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_SYRINGE:
		pProjectile = FireNail( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_CANNONBALL:
		pProjectile = FirePipeBomb( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_REMOTE_PRACTICE:
		pProjectile = FirePipeBomb( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_FLARE:
		pProjectile = FireFlare( pPlayer );
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case TF_PROJECTILE_JAR:
	case TF_PROJECTILE_JAR_MILK:
	case TF_PROJECTILE_FESTITIVE_URINE:
	case TF_PROJECTILE_BREADMONSTER_JARATE:
	case TF_PROJECTILE_BREADMONSTER_MADMILK:
		pProjectile = FireJar( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_CLEAVER:
	case TF_PROJECTILE_THROWABLE:
		pProjectile = FireJar( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_ARROW:
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
	case TF_PROJECTILE_FESTITIVE_ARROW:
	case TF_PROJECTILE_FESTITIVE_HEALING_BOLT:
	case TF_PROJECTILE_GRAPPLINGHOOK:
		pProjectile = FireArrow(pPlayer, iProjectile);
		pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);
		break;

	case LFE_HL2_PROJECTILE_FRAG:
		pProjectile = FireHLFrag( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case LFE_HL2_PROJECTILE_COMBINEBALL:
		pProjectile = FireHLCombineBall( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case LFE_HL2_PROJECTILE_AR2:
		pProjectile = FireHLAR2Grenade( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case LFE_HL2_PROJECTILE_SPIT:
		pProjectile = FireHLSpit( pPlayer, iProjectile );
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
		break;

	case TF_PROJECTILE_NONE:
	default:
		// do nothing!
		DevMsg( "Weapon does not have a projectile type set\n" );
		break;
	}

	if ( UsesClipsForAmmo1() )
	{
		m_iClip1 -= m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iAmmoPerShot;
	}
	else
	{
		if ( m_iWeaponMode == TF_WEAPON_PRIMARY_MODE )
		{
			pPlayer->RemoveAmmo( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iAmmoPerShot, m_iPrimaryAmmoType );
		}
		else
		{
			pPlayer->RemoveAmmo( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iAmmoPerShot, m_iSecondaryAmmoType );
		}
	}

	DoFireEffects();

	UpdatePunchAngles( pPlayer );

	return pProjectile;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::UpdatePunchAngles( CTFPlayer *pPlayer )
{
	// Update the player's punch angle.
	QAngle angle = pPlayer->GetPunchAngle();
	float flPunchAngle = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flPunchAngle;

	if ( flPunchAngle > 0 )
	{
		angle.x -= SharedRandomInt( "ShotgunPunchAngle", ( flPunchAngle - 1 ), ( flPunchAngle + 1 ) );
		pPlayer->SetPunchAngle( angle );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Fire a bullet!
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::FireBullet( CTFPlayer *pPlayer )
{
	PlayWeaponShootSound();

	FX_FireBullets(
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + pPlayer->GetPunchAngle(),
		GetWeaponID(),
		m_iWeaponMode,
		CBaseEntity::GetPredictionRandomSeed() & 255,
		GetWeaponSpread(),
		GetProjectileDamage(),
		IsCurrentAttackACrit() );
}

class CTraceFilterIgnoreTeammates : public CTraceFilterSimple
{
public:
	// It does have a base, but we'll never network anything below here..
	DECLARE_CLASS( CTraceFilterIgnoreTeammates, CTraceFilterSimple );

	CTraceFilterIgnoreTeammates( const IHandleEntity *passentity, int collisionGroup, int iIgnoreTeam )
		: CTraceFilterSimple( passentity, collisionGroup ), m_iIgnoreTeam( iIgnoreTeam )
	{
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		CBaseEntity *pEntity = EntityFromEntityHandle( pServerEntity );

		if ( pEntity->IsPlayer() || pEntity->IsNPC() && pEntity->GetTeamNumber() == m_iIgnoreTeam )
		{
			return false;
		}

		return true;
	}

	int m_iIgnoreTeam;
};

//-----------------------------------------------------------------------------
// Purpose: Return angles for a projectile reflected by airblast
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::GetProjectileReflectSetup( CTFPlayer *pPlayer, const Vector &vecPos, Vector *vecDeflect, bool bHitTeammates /* = true */, bool bUseHitboxes /* = false */ )
{
	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	Vector vecShootPos = pPlayer->Weapon_ShootPosition();

	// Estimate end point
	Vector endPos = vecShootPos + vecForward * 2000;

	// Trace forward and find what's in front of us, and aim at that
	trace_t tr;
	int nMask = bUseHitboxes ? MASK_SOLID | CONTENTS_HITBOX : MASK_SOLID;

	if ( bHitTeammates )
	{
		CTraceFilterSimple filter( pPlayer, COLLISION_GROUP_NONE );
		UTIL_TraceLine( vecShootPos, endPos, nMask, &filter, &tr );
	}
	else
	{
		CTraceFilterIgnoreTeammates filter( pPlayer, COLLISION_GROUP_NONE, pPlayer->GetTeamNumber() );
		UTIL_TraceLine( vecShootPos, endPos, nMask, &filter, &tr );
	}

	// vecPos is projectile's current position. Use that to find angles.

	// Find angles that will get us to our desired end point
	// Only use the trace end if it wasn't too close, which results
	// in visually bizarre forward angles
	if ( tr.fraction > 0.1 || bUseHitboxes )
	{
		*vecDeflect = tr.endpos - vecPos;
	}
	else
	{
		*vecDeflect = endPos - vecPos;
	}

	VectorNormalize( *vecDeflect );
}

//-----------------------------------------------------------------------------
// Purpose: Return the origin & angles for a projectile fired from the player's gun
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::GetProjectileFireSetup( CTFPlayer *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates /* = true */, bool bUseHitboxes /* = false */ )
{
	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	Vector vecShootPos = pPlayer->Weapon_ShootPosition();

	// Estimate end point
	Vector endPos = vecShootPos + vecForward * 2000;	

	// Trace forward and find what's in front of us, and aim at that
	trace_t tr;
	int nMask = bUseHitboxes ? MASK_SOLID | CONTENTS_HITBOX : MASK_SOLID;

	if ( bHitTeammates )
	{
		CTraceFilterSimple filter( pPlayer, COLLISION_GROUP_NONE );
		UTIL_TraceLine( vecShootPos, endPos, nMask, &filter, &tr );
	}
	else
	{
		CTraceFilterIgnoreTeammates filter( pPlayer, COLLISION_GROUP_NONE, pPlayer->GetTeamNumber() );
		UTIL_TraceLine( vecShootPos, endPos, nMask, &filter, &tr );
	}

#ifndef CLIENT_DLL
	// If viewmodel is flipped fire from the other side.
	if ( IsViewModelFlipped() )
	{
		vecOffset.y *= -1.0f;
	}

	// Offset actual start point
	*vecSrc = vecShootPos + (vecForward * vecOffset.x) + (vecRight * vecOffset.y) + (vecUp * vecOffset.z);
#else
	// If we're seeing another player shooting the projectile, move their start point to the weapon origin
	if ( pPlayer )
	{
		if ( !UsingViewModel() )
		{
			GetAttachment( "muzzle", *vecSrc );
		}
		else
		{
			C_BaseEntity *pViewModel = pPlayer->GetViewModel();

			if ( pViewModel )
			{
				QAngle vecAngles;
				int iMuzzleFlashAttachment = pViewModel->LookupAttachment( "muzzle" );
				pViewModel->GetAttachment( iMuzzleFlashAttachment, *vecSrc, vecAngles );

				Vector vForward;
				AngleVectors( vecAngles, &vForward );

				trace_t trace;	
				UTIL_TraceLine( *vecSrc + vForward * -50, *vecSrc, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &trace );

				*vecSrc = trace.endpos;
			}
		}
	}
#endif

	// Find angles that will get us to our desired end point
	// Only use the trace end if it wasn't too close, which results
	// in visually bizarre forward angles
	if ( tr.fraction > 0.1 || bUseHitboxes )
	{
		VectorAngles( tr.endpos - *vecSrc, *angForward );
	}
	else
	{
		VectorAngles( endPos - *vecSrc, *angForward );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Fire a rocket
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireRocket( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

	// Server only - create the rocket.
#ifdef GAME_DLL

	Vector vecSrc;
	QAngle angForward;
	Vector vecOffset( 23.5f, 12.0f, -3.0f );
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		vecOffset.z = 8.0f;
	}

	bool bUseHitboxes = ( iType == TF_PROJECTILE_ARROW || iType == TF_PROJECTILE_FESTITIVE_ARROW );

	GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false, bUseHitboxes );

	CTFBaseRocket *pProjectile = NULL;

	switch ( iType )
	{
	case TF_PROJECTILE_ROCKET:
		pProjectile = CTFProjectile_Rocket::Create( this, vecSrc, angForward, pPlayer, pPlayer );
		break;
	default:
		Assert( false );
		break;
	}

	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetDamage( GetProjectileDamage() );
	}

	return pProjectile;

#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Fire a projectile nail
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireNail( CTFPlayer *pPlayer, int iSpecificNail )
{
	PlayWeaponShootSound();

	Vector vecSrc;
	QAngle angForward;
	GetProjectileFireSetup( pPlayer, Vector(16,6,-8), &vecSrc, &angForward );

	float flSpread = 1.5;

	angForward.x += RandomFloat(-flSpread, flSpread);
	angForward.y += RandomFloat(-flSpread, flSpread);

	CTFBaseProjectile *pProjectile = NULL;
	switch( iSpecificNail )
	{
	case TF_PROJECTILE_SYRINGE:
		pProjectile = CTFProjectile_Syringe::Create( vecSrc, angForward, pPlayer, pPlayer, IsCurrentAttackACrit() );
		break;

	default:
		Assert(0);
	}

	if ( pProjectile )
	{
		pProjectile->SetWeaponID( GetWeaponID() );
		pProjectile->SetCritical( IsCurrentAttackACrit() );
#ifdef GAME_DLL
		pProjectile->SetDamage( GetProjectileDamage() );
#endif
	}
	
	return pProjectile;
}

//-----------------------------------------------------------------------------
// Purpose: Fire a  pipe bomb
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FirePipeBomb( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	/*
	int iMode = TF_GL_MODE_REGULAR;
	CALL_ATTRIB_HOOK_INT( iMode, set_detonate_mode );

	if ( bRemoteDetonate )
	{
		iMode = TF_GL_MODE_REMOTE_DETONATE;
	}
	*/
	int iNoSpin = 0;
	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	// Create grenades here!!
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	vecSrc +=  vecForward * 16.0f + vecRight * 8.0f + vecUp * -6.0f;
	
	Vector vecVelocity = ( vecForward * GetProjectileSpeed() ) + ( vecUp * 200.0f ) + ( random->RandomFloat( -10.0f, 10.0f ) * vecRight ) +		
		( random->RandomFloat( -10.0f, 10.0f ) * vecUp );

	CALL_ATTRIB_HOOK_INT ( iNoSpin, grenade_no_spin );

	AngularImpulse spin( 600, 0, 0 );

	if ( iNoSpin == 0 )
	{
		spin = AngularImpulse( 600, random->RandomInt( -1200, 1200 ), 0 );
	}

	CTFWeaponBaseGrenadeProj *pProjectile = NULL;

	float flDamageMult = 1.0f;
	CALL_ATTRIB_HOOK_FLOAT( flDamageMult, mult_dmg );

	switch( iType )
	if ( pProjectile )
	{
	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_REMOTE_PRACTICE:
		pProjectile = CTFGrenadeStickybombProjectile::Create( vecSrc, pPlayer->EyeAngles(), vecVelocity,
			spin,
			pPlayer, this );
		break;
	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_CANNONBALL:
		pProjectile = CTFGrenadePipebombProjectile::Create( vecSrc, pPlayer->EyeAngles(), vecVelocity,
			spin,
			pPlayer, this );
		break;
	}

	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetDamage( GetProjectileDamage() );
	}

	return pProjectile;

#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Fire a flare
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireFlare(CTFPlayer *pPlayer)
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	Vector vecSrc;
	QAngle angForward;
	Vector vecOffset( 23.5f, 12.0f, -3.0f );
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		vecOffset.z = 8.0f;
	}
	GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false );

	CTFProjectile_Flare *pProjectile = CTFProjectile_Flare::Create( this, vecSrc, angForward, pPlayer, pPlayer );
	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetDamage( GetProjectileDamage() );
	}
	return pProjectile;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Fire an Arrow
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireArrow( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	Vector vecSrc;
	QAngle angForward;
	Vector vecOffset(23.5f, 12.0f, -3.0f);
	if (pPlayer->GetFlags() & FL_DUCKING)
	{
		vecOffset.z = 8.0f;
	}

	GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false, true );

	CTFProjectile_Arrow *pProjectile = CTFProjectile_Arrow::Create( this, vecSrc, angForward, GetProjectileSpeed(), GetProjectileGravity(), false, pPlayer, pPlayer, iType );
	if (pProjectile)
	{
		pProjectile->SetCritical(IsCurrentAttackACrit());
		pProjectile->SetDamage(GetProjectileDamage());
	}
	return pProjectile;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Fire some piss
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireJar( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	AngularImpulse spin = AngularImpulse( 600, random->RandomInt( -1200, 1200 ), 0 );

	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	// Create grenades here!!
	// Set the starting position a bit behind the player so the projectile
	// launches out of the players view
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	vecSrc +=  vecForward * -32.0f + vecRight * 8.0f + vecUp * -6.0f;

	Vector vecVelocity = ( vecForward * GetProjectileSpeed() ) + ( vecUp * 200.0f ) + ( random->RandomFloat( -10.0f, 10.0f ) * vecRight ) +		
		( random->RandomFloat( -10.0f, 10.0f ) * vecUp );

	//GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false, false );
	CTFProjectile_Jar *pProjectile = CTFProjectile_Jar::Create( this, vecSrc, pPlayer->EyeAngles(), vecVelocity, pPlayer, pPlayer, spin, GetTFWpnData() );
	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetDamage( GetProjectileDamage() );
	}
	return pProjectile;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Use this for any old grenades: MIRV, Frag, etc
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireGrenade( CTFPlayer *pPlayer )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL

	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	// Create grenades here!!
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	vecSrc += vecForward * 16.0f + vecRight * 8.0f + vecUp * -6.0f;

	Vector vecVelocity = ( vecForward * GetProjectileSpeed() ) + ( vecUp * 200.0f ) + ( random->RandomFloat( -10.0f, 10.0f ) * vecRight ) +
		( random->RandomFloat(-10.0f, 10.0f) * vecUp );

	float flDamageMult = 1.0f;
	CALL_ATTRIB_HOOK_FLOAT(flDamageMult, mult_dmg);

	char szEntName[256];
	V_snprintf( szEntName, sizeof(szEntName), "%s_projectile", WeaponIdToClassname( GetWeaponID() ) );

	CTFWeaponBaseGrenadeProj *pProjectile = CTFWeaponBaseGrenadeProj::Create( szEntName, vecSrc, pPlayer->EyeAngles(), vecVelocity,
		AngularImpulse( 600, random->RandomInt(-1200, 1200), 0 ),
		pPlayer, this /*GetTFWpnData(), 3.0f, flDamageMult*/ );

	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetLauncher( this );
	}
	return pProjectile;

#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: test on tf weapons
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireHLFrag( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	AngularImpulse spin = AngularImpulse( 600, random->RandomInt( -1200, 1200 ), 0 );

	Vector vecForward, vecRight, vecUp;
	AngleVectors( pPlayer->EyeAngles(), &vecForward, &vecRight, &vecUp );

	// Create grenades here!!
	// Set the starting position a bit behind the player so the projectile
	// launches out of the players view
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	vecSrc +=  vecForward * -32.0f + vecRight * 8.0f + vecUp * -6.0f;

	Vector vecVelocity = ( vecForward * GetProjectileSpeed() ) + ( vecUp * 200.0f ) + ( random->RandomFloat( -10.0f, 10.0f ) * vecRight ) +		
		( random->RandomFloat( -10.0f, 10.0f ) * vecUp );

	return Fraggrenade_Create( vecSrc, vec3_angle, vecVelocity, spin, pPlayer, 3.0f, false );
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireHLCombineBall( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

	// Server only - create the rocket.
#ifdef GAME_DLL

	Vector vecSrc;
	QAngle angForward;
	Vector vecOffset( 23.5f, 12.0f, -3.0f );
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		vecOffset.z = 8.0f;
	}

	Vector vecAiming = pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	Vector vecVelocity = vecAiming * 1000.0f;

	GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false, false );
	return CreateCombineBall( vecSrc, vecVelocity, 10.0f, 150.0f, 2.0f, pPlayer );
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireHLAR2Grenade( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	//pPlayer->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAGS_NONE );

	float flDamageMult = 100.0f;
	CALL_ATTRIB_HOOK_FLOAT(flDamageMult, mult_dmg);

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create( "grenade_ar2", vecSrc, angles, pPlayer );
	pGrenade->SetAbsVelocity( vecThrow );

	pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE ); 
	pGrenade->SetThrower( GetOwner() );
	pGrenade->SetDamage( flDamageMult );
	
	return pGrenade;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBaseGun::FireHLSpit( CTFPlayer *pPlayer, int iType )
{
	PlayWeaponShootSound();

#ifdef GAME_DLL
	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeSpit *pGrenade = (CGrenadeSpit*)Create( "grenade_spit", vecSrc, angles, pPlayer );
	pGrenade->SetThrower( GetOwner() );
	pGrenade->ApplyAbsVelocityImpulse( ( vecThrow + RandomVector( -0.035f, 0.035f ) ) /* flVelocity*/ );
	pGrenade->SetSpitSize( random->RandomInt( SPIT_SMALL, SPIT_MEDIUM ) );

	/*QAngle angles( random->RandomFloat( -250, -500 ),
				random->RandomFloat( -250, -500 ),
				random->RandomFloat( -250, -500 ) );*/
		
	//VectorAngles( vecVelocity, angles );
	pGrenade->SetLocalAngularVelocity( angles );

	return pGrenade;
#endif

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::PlayWeaponShootSound( void )
{
	if ( IsCurrentAttackACrit() )
	{
		WeaponSound( BURST );
	}
	else
	{
		WeaponSound( SINGLE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Accessor for damage, so sniper etc can modify damage
//-----------------------------------------------------------------------------
float CTFWeaponBaseGun::GetProjectileSpeed( void )
{
	// placeholder for now
	// grenade launcher and pipebomb launcher hook this to set variable pipebomb speed

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Accessor for damage, so sniper etc can modify damage
//-----------------------------------------------------------------------------
float CTFWeaponBaseGun::GetProjectileGravity( void )
{
	return 0.001f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWeaponBaseGun::GetWeaponSpread( void )
{
	float flSpread = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSpread;
	CALL_ATTRIB_HOOK_FLOAT( flSpread, mult_spread_scale );
	return flSpread;
}

//-----------------------------------------------------------------------------
// Purpose: Accessor for damage, so sniper etc can modify damage
//-----------------------------------------------------------------------------
float CTFWeaponBaseGun::GetProjectileDamage( void )
{
	float flDamage = (float)m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_nDamage;
	CALL_ATTRIB_HOOK_FLOAT( flDamage, mult_dmg );
	return flDamage;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponBaseGun::Holster( CBaseCombatWeapon *pSwitchingTo )
{
// Server specific.
#if !defined( CLIENT_DLL )

	// Make sure to zoom out before we holster the weapon.
	ZoomOut();
	SetContextThink( NULL, 0, ZOOM_CONTEXT );

#endif

	// Stop the burst.
	m_iBurstSize = 0;

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::WeaponReset( void )
{
	// Stop the burst.
	m_iBurstSize = 0;

	BaseClass::WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose:
// NOTE: Should this be put into fire gun
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::DoFireEffects()
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	// Muzzle flash on weapon.
	bool bMuzzleFlash = true;


	if ( pPlayer->IsPlayerClass( TF_CLASS_SNIPER ) )
	{
		if ( pPlayer->IsActiveTFWeapon( TF_WEAPON_COMPOUND_BOW ) )
		{
			bMuzzleFlash = false;
		}
	}

	if ( bMuzzleFlash )
	{
		pPlayer->DoMuzzleFlash();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::ToggleZoom( void )
{
	// Toggle the zoom.
	CBasePlayer *pPlayer = GetPlayerOwner();
	if ( pPlayer )
	{
		if( pPlayer->GetFOV() >= 75 )
		{
			ZoomIn();
		}
		else
		{
			ZoomOut();
		}
	}

	// Get the zoom animation time.
	m_flNextSecondaryAttack = gpGlobals->curtime + 1.2;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::ZoomIn( void )
{
	// The the owning player.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	// Set the weapon zoom.
	// TODO: The weapon fov should be gotten from the script file.
	pPlayer->SetFOV( pPlayer, TF_WEAPON_ZOOM_FOV, 0.1f );
	pPlayer->m_Shared.AddCond( TF_COND_ZOOMED );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::ZoomOut( void )
{
	// The the owning player.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	if ( pPlayer->m_Shared.InCond( TF_COND_ZOOMED ) )
	{
		// Set the FOV to 0 set the default FOV.
		pPlayer->SetFOV( pPlayer, 0, 0.1f );
		pPlayer->m_Shared.RemoveCond( TF_COND_ZOOMED );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBaseGun::ZoomOutIn( void )
{
	//Zoom out, set think to zoom back in.
	ZoomOut();
	SetContextThink( &CTFWeaponBaseGun::ZoomIn, gpGlobals->curtime + ZOOM_REZOOM_TIME, ZOOM_CONTEXT );
}
