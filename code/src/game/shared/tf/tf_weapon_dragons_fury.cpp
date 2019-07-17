//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_dragons_fury.h"
#include "tf_fx_shared.h"
#include "tf_weaponbase_rocket.h"
#include "tf_projectile_dragons_fury.h"
// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "tf_viewmodel.h"
#include "c_tf_viewmodeladdon.h"
// Server specific.
#else
#include "tf_player.h"
#include "soundent.h"
#include "tf_gamerules.h"
#include "tf_gamestats.h"
#include "ilagcompensationmanager.h"
#include "ai_basenpc.h"
#include "npc_antlion.h"
#include "props.h"
#endif

extern ConVar lfe_force_legacy;
extern ConVar lfe_allow_airblast_physics;
#ifdef GAME_DLL
	extern ConVar lfe_debug_airblast_physics_distance;
	extern ConVar lfe_debug_airblast_physics_force;
	extern ConVar lfe_debug_airblast_physics_mass;
	extern ConVar lfe_debug_airblast;
	extern ConVar tf_airblast_cray;
	extern ConVar tf_airblast_cray_debug;
#endif

#ifdef GAME_DLL
ConVar  tf_fireball_airblast_recharge_penalty("tf_fireball_airblast_recharge_penalty", "0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_hit_recharge_boost("tf_fireball_hit_recharge_boost", "1", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
#endif

#define TF_FLAMEBALL_AMMO_PER_SECONDARY_ATTACK	20

//CREATE_SIMPLE_WEAPON_TABLE( TFWeaponFlameBall, tf_weapon_rocketlauncher_fireball ) // DRAGON'S FURY
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponFlameBall, DT_WeaponFlameBall )

BEGIN_NETWORK_TABLE( CTFWeaponFlameBall, DT_WeaponFlameBall )
#if !defined( CLIENT_DLL )
	SendPropTime( SENDINFO( m_flRechargeScale ) ),
#else
	RecvPropTime( RECVINFO( m_flRechargeScale ) ),
#endif
END_NETWORK_TABLE()

#if defined( CLIENT_DLL )
BEGIN_PREDICTION_DATA( CTFWeaponFlameBall )
	DEFINE_PRED_FIELD_TOL( m_flRechargeScale, FIELD_FLOAT, FTYPEDESC_INSENDTABLE, TD_MSECTOLERANCE ),	
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( tf_weapon_rocketlauncher_fireball, CTFWeaponFlameBall );
PRECACHE_WEAPON_REGISTER( tf_weapon_rocketlauncher_fireball );

BEGIN_DATADESC( CTFWeaponFlameBall )
	DEFINE_FIELD( m_flRechargeScale, FIELD_TIME ),
END_DATADESC()

//=============================================================================
//
// Weapon Dragon's Fury i guess?
//

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFWeaponFlameBall::CTFWeaponFlameBall()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFWeaponFlameBall::~CTFWeaponFlameBall()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::Precache( void )
{
	BaseClass::Precache();
	PrecacheParticleSystem( "pyro_blast" );
	PrecacheScriptSound( "Weapon_FlameThrower.AirBurstAttack" );
	PrecacheScriptSound( "TFPlayer.AirBlastImpact" );
	PrecacheScriptSound( "TFPlayer.FlameOut" );
	PrecacheScriptSound( "Weapon_FlameThrower.AirBurstAttackDeflect" );
	PrecacheScriptSound( "Weapon_DragonsFury.Single" );
	PrecacheScriptSound( "Weapon_DragonsFury.SingleCrit" );
	PrecacheScriptSound( "Weapon_DragonsFury.PressureBuild" );
	PrecacheScriptSound( "Weapon_DragonsFury.PressureBuildStop" );
	PrecacheParticleSystem( "deflect_fx" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponFlameBall::FireProjectile( CTFPlayer *pPlayer )
{
	if ( IsCurrentAttackACrit() )
	{
		EmitSound( "Weapon_DragonsFury.SingleCrit" );
	}
	else
	{
		EmitSound( "Weapon_DragonsFury.Single" );
	}

	// Server only - create the rocket.
#ifdef GAME_DLL

	Vector vecSrc;
	QAngle angForward;
	Vector vecOffset( 23.5f, 12.0f, -3.0f );
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		vecOffset.z = 8.0f;
	}

	GetProjectileFireSetup( pPlayer, vecOffset, &vecSrc, &angForward, false, false );
		
	CTFProjectile_BallOfFire *pProjectile = CTFProjectile_BallOfFire::Create( this, vecSrc, angForward, pPlayer, pPlayer );
	if ( pProjectile )
	{
		pProjectile->SetCritical( IsCurrentAttackACrit() );
		pProjectile->SetDamage( GetProjectileDamage() );
	}

#endif

	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
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
#ifdef GAME_DLL
	return pProjectile;
#endif
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponFlameBall::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::PrimaryAttack()
{
	m_bReloadedThroughAnimEvent = false;

	BaseClass::PrimaryAttack();

#ifdef GAME_DLL
	StartPressureSound();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::SecondaryAttack()
{
	if ( lfe_force_legacy.GetBool() )
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
		return;

	// i dunno how will this thing work since it won't even work in live tf2.
	int iChargedAirblast = 0;
	CALL_ATTRIB_HOOK_FLOAT( iChargedAirblast, set_charged_airblast );
	if ( iChargedAirblast != 1 )
	{
	SendWeaponAnim( ACT_VM_SECONDARYATTACK );
	WeaponSound( WPN_DOUBLE );

#ifdef GAME_DLL

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

	if ( lfe_debug_airblast.GetBool() )
	{
		NDebugOverlay::Box( vecOrigin, -vecBlastSize, vecBlastSize, 0, 0, 255, 100, 2.0 );
	}
	else if ( tf_airblast_cray_debug.GetBool() )
	{
		Vector vecConeOrigin = pOwner->WorldSpaceCenter();
		float flConeDot = 0.8f;
		float flConeDepth = 180.0;
		int r = 255;
		int g = 255;
		int b = 255;
		int a = 30;
		float duration = 5.0;

		// Circles
		float delta = 15.0;
		
		Vector vecCirclefwd;
		AngleVectors(angDir, &vecCirclefwd);
		
		for (float dist = 1.0f; dist <= flConeDepth; dist += delta)
		{
			Vector vecCirclecenter = vecConeOrigin + (vecCirclefwd * dist);
			float radius = dist * tan(acos(flConeDot));
			
			NDebugOverlay::Circle(vecCirclecenter, angDir, radius, r, g, b, a, false, duration);
		}

		// Lines
		Vector vecLinefwd, vecLinert, vecLineup;
		AngleVectors(angDir, &vecLinefwd, &vecLinert, &vecLineup);
		
		Vector center = vecConeOrigin + (vecLinefwd * flConeDepth);
		
		int num_lines = 60;
		for (int i = 0; i < num_lines; ++i) {
			// TODO
		}

		// 3D
		Vector vecFwd;
		AngleVectors(angDir, &vecFwd);
		Vector vecEndCenter = vecConeOrigin + (vecFwd * flConeDepth);
		
		matrix3x4_t xform;
		AngleMatrix(angDir, vecConeOrigin, xform);
		Vector xAxis;
		MatrixGetColumn(xform, 2, xAxis);
		Vector yAxis;
		MatrixGetColumn(xform, 1, yAxis);
		
		float radius = flConeDepth * tan(acos(flConeDot));
		
		Vector vecPosition;
		Vector vecLastPosition;
		
		int num_polys = 30;
		
		float flRadStep = (2.0f * M_PI) / (float)num_polys;
		
		for (int i = 1; i <= num_polys + 1; ++i) {
			vecLastPosition = vecPosition;
			
			float flSin, flCos;
			SinCos(i * flRadStep, &flSin, &flCos);
			
			vecPosition = vecEndCenter + (radius * flCos * xAxis) + (radius * flSin * yAxis);
			
			if (i > 1) {
				const Vector& p1 = vecConeOrigin;
				const Vector& p2 = vecLastPosition;
				const Vector& p3 = vecPosition;
				
				/* lines */
				NDebugOverlay::Line(vecConeOrigin,          vecPosition, r, g, b, false, duration);
				NDebugOverlay::Line(vecLastPosition, vecPosition, r, g, b, false, duration);
				
				if (a != 0) {
					/* forward and back faces */
					NDebugOverlay::Triangle(p1, p2, p3, r, g, b, a, false, duration);
					NDebugOverlay::Triangle(p1, p3, p2, r, g, b, a, false, duration);
				}
			}
		}
	}

	int count = 0;
	if ( tf_airblast_cray.GetBool() )
		count = UTIL_EntitiesInBox( pList, 64, vecOrigin - vecBlastSize, vecOrigin + vecBlastSize, 0 );
	else
		count = UTIL_EntitiesInBox( pList, 64, vecOrigin - vecBlastSize, vecOrigin + vecBlastSize, 0 );

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

			if ( tf_airblast_cray.GetBool() )
			{
				Vector vecPushDir = pOwner->EyeDirection3D();
				QAngle angPushDir = angDir;

				// Push them at least 45 degrees up.
				angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

				AngleVectors( angPushDir, &vecPushDir );
				DeflectPlayer( pTFPlayer, pOwner, vecPushDir );
			}
			else
			{
				Vector vecPushDir = pOwner->EyeDirection3D();
				QAngle angPushDir = angDir;

				// Push them at least 45 degrees up.
				angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

				AngleVectors( angPushDir, &vecPushDir );
				DeflectPlayer( pTFPlayer, pOwner, vecPushDir );
			}
		}
		else if ( pEntity->IsNPC() ) // push npcs
		{
			if ( !pEntity->IsAlive() )
				continue;

			CAI_BaseNPC *pNPC = pEntity->MyNPCPointer();

			if ( tf_airblast_cray.GetBool() )
			{
				Vector vecPushDir = pOwner->EyeDirection3D();
				QAngle angPushDir = angDir;

				// Push them at least 45 degrees up.
				angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

				AngleVectors( angPushDir, &vecPushDir );
				DeflectNPC( pNPC, pOwner, vecPushDir );
			}
			else
			{
				Vector vecPushDir = pOwner->EyeDirection3D();
				QAngle angPushDir = angDir;

				// Push them at least 45 degrees up.
				angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

				AngleVectors( angPushDir, &vecPushDir );
				DeflectNPC( pNPC, pOwner, vecPushDir );
			}
		}
		else if( pEntity->IsNPC() && pEntity->Classify() != CLASS_HEADCRAB && !FClassnameIs(pEntity, "npc_antlion") ) // gravity gun like  push antlions
		{
			if ( !pEntity->IsAlive() )
				continue;

			CNPC_Antlion *pAnt = dynamic_cast<CNPC_Antlion *>(pEntity);

			Vector	vecShoveDir = vecDir;
			vecShoveDir.z = 0.0f;
			pAnt->ApplyAbsVelocityImpulse( ( vecShoveDir * random->RandomInt( 50.0f, 100.0f ) ) + Vector(0,0,64.0f) );
			pAnt->SetGroundEntity( NULL );
			pAnt->Flip();
		}
		else if ( pEntity->VPhysicsGetObject() ) // push physics
		{
			Vector vecPushDir = pOwner->EyeDirection3D();
			QAngle angPushDir = angDir;

			// Push them at least 45 degrees up.
			angPushDir[PITCH] = min( -45, angPushDir[PITCH] );

			AngleVectors( angPushDir, &vecPushDir );

			DeflectPhysics( pEntity, pOwner, vecPushDir );
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
#else
	C_BaseEntity *pModel = GetWeaponForEffect();

	if ( pModel )
	{
		pModel->ParticleProp()->Create( "pyro_blast", PATTACH_POINT_FOLLOW, "muzzle" );
	}
#endif

	float flAmmoPerSecondaryAttack = TF_FLAMEBALL_AMMO_PER_SECONDARY_ATTACK;
	CALL_ATTRIB_HOOK_FLOAT( flAmmoPerSecondaryAttack, mult_airblast_cost );

	pOwner->RemoveAmmo( flAmmoPerSecondaryAttack, m_iPrimaryAmmoType );

	// Don't allow firing immediately after airblasting.
	m_flRechargeScale = m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 1.6f;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFWeaponFlameBall::GetEffectBarProgress( void )
{
	return m_flRechargeScale / 0.8f;
}

#ifdef GAME_DLL
void CTFWeaponFlameBall::StartPressureSound( void )
{
	EmitSound( "Weapon_DragonsFury.PressureBuild" );
}

void CTFWeaponFlameBall::OnResourceMeterFilled( void )
{
	EmitSound( "Weapon_DragonsFury.PressureBuildStop" );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::GetProjectileFireSetup( CTFPlayer *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates, bool bUseHitboxes )
{
	BaseClass::GetProjectileFireSetup( pPlayer, vecOffset, vecSrc, angForward, bHitTeammates, bUseHitboxes );
}

#ifdef CLIENT_DLL
void CTFWeaponFlameBall::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	UpdatePoseParam();
}

bool CTFWeaponFlameBall::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		UpdatePoseParam();
		return true;
	}

	return false;
}

void CTFWeaponFlameBall::UpdatePoseParam( void )
{
	SetPoseParameter( "reload", m_flRechargeScale = m_flNextSecondaryAttack );
	SetPoseParameter( "charge_level", m_flRechargeScale = m_flNextSecondaryAttack );

	C_ViewmodelAttachmentModel *pAttachment = GetViewmodelAddon();
	if ( pAttachment )
 	{
		pAttachment->SetPoseParameter( "reload", m_flRechargeScale = m_flNextSecondaryAttack );
		pAttachment->SetPoseParameter( "charge_level", m_flRechargeScale = m_flNextSecondaryAttack );
	}
}
#endif