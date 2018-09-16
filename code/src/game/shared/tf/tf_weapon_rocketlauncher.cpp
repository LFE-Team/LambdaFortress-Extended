//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_rocketlauncher.h"
#include "tf_fx_shared.h"
#include "tf_weaponbase_rocket.h"

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


CREATE_SIMPLE_WEAPON_TABLE( TFRocketLauncher, tf_weapon_rocketlauncher )
CREATE_SIMPLE_WEAPON_TABLE( TFRocketLauncher_DirectHit, tf_weapon_rocketlauncher_directhit )
CREATE_SIMPLE_WEAPON_TABLE( TFRocketLauncher_AirStrike, tf_weapon_rocketlauncher_airstrike )

//=============================================================================
//
// Weapon Rocket Launcher tables.
//

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::CTFRocketLauncher()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::~CTFRocketLauncher()
{
}

#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::Precache()
{
	BaseClass::Precache();
	PrecacheParticleSystem( "rocketbackblast" );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFRocketLauncher::FireProjectile( CTFPlayer *pPlayer )
{
	m_flShowReloadHintAt = gpGlobals->curtime + 30;
	return BaseClass::FireProjectile( pPlayer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	BaseClass::ItemPostFrame();

#ifdef GAME_DLL

	if ( m_flShowReloadHintAt && m_flShowReloadHintAt < gpGlobals->curtime )
	{
		if ( Clip1() < GetMaxClip1() )
		{
			pOwner->HintMessage( HINT_SOLDIER_RPG_RELOAD );
		}
		m_flShowReloadHintAt = 0;
	}

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	m_flShowReloadHintAt = 0;
	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex )
{
	BaseClass::CreateMuzzleFlashEffects( pAttachEnt, nIndex );

	// Don't do backblast effects in first person
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner->IsLocalPlayer() )
		return;

	ParticleProp()->Create( "rocketbackblast", PATTACH_POINT_FOLLOW, "backblast" );
}

#endif

extern ConVar lfe_allow_airblast;
extern ConVar lfe_allow_airblast_physics;
#ifdef GAME_DLL
	extern ConVar lfe_debug_airblast_physics_distance;
	extern ConVar lfe_debug_airblast_physics_force;
	extern ConVar lfe_debug_airblast_physics_mass;
	extern ConVar lfe_debug_airblast;
#endif

#ifdef GAME_DLL
ConVar  tf_fireball_airblast_recharge_penalty("tf_fireball_airblast_recharge_penalty", "0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_hit_recharge_boost("tf_fireball_hit_recharge_boost", "1", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
#endif

#define TF_FLAMEBALL_AMMO_PER_SECONDARY_ATTACK	20

//CREATE_SIMPLE_WEAPON_TABLE( TFWeaponFlameBall, tf_weapon_rocketlauncher_fireball ) // DRAGON'S FURY
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponFlameBall, DT_TFWeaponFlameBall )

BEGIN_NETWORK_TABLE( CTFWeaponFlameBall, DT_TFWeaponFlameBall )
	#if defined( CLIENT_DLL )
		RecvPropBool( RECVINFO( m_bHitTarget ) )
	#else
		SendPropBool( SENDINFO( m_bHitTarget ) )
	#endif
END_NETWORK_TABLE()

#if defined( CLIENT_DLL )
BEGIN_PREDICTION_DATA( CTFWeaponFlameBall )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( tf_weapon_rocketlauncher_fireball, CTFWeaponFlameBall );
PRECACHE_WEAPON_REGISTER( tf_weapon_rocketlauncher_fireball );
/*
BEGIN_DATADESC( CTFWeaponFlameBall )
END_DATADESC()
*/
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
	PrecacheParticleSystem( "deflect_fx" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::Spawn( void )
{
	BaseClass::Spawn();

	m_bHitTarget = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFWeaponFlameBall::FireProjectile( CTFPlayer *pPlayer )
{
	return BaseClass::FireProjectile( pPlayer );
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
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::SecondaryAttack()
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
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 1.6f;
	}
}

#ifdef GAME_DLL
void CTFWeaponFlameBall::DeflectEntity( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir )
{
	if ( !TFGameRules() )
		return;

	if ( ( pEntity->GetTeamNumber() == pAttacker->GetTeamNumber() ) && !TFGameRules()->IsFriendlyFire() )
		return;

	pEntity->Deflected( pAttacker, vecDir );
	pEntity->EmitSound( "Weapon_FlameThrower.AirBurstAttackDeflect" );
}

void CTFWeaponFlameBall::DeflectPlayer( CTFPlayer *pVictim, CTFPlayer *pAttacker, Vector &vecDir )
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

void CTFWeaponFlameBall::DeflectNPC( CAI_BaseNPC *pVictim, CTFPlayer *pAttacker, Vector &vecDir )
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
			pVictim->SetAirblastState( true );

			// Add pusher as recent damager so he can get a kill credit for pushing a player to his death.
			pVictim->AddDamagerToHistory( pAttacker );
		}
		pVictim->Deflected( pAttacker, vecDir );
	}
}

void CTFWeaponFlameBall::DeflectPhysics( CBaseEntity *pEntity, CTFPlayer *pAttacker, Vector &vecDir )
{
	int nPushbackDisabled = 0;
	CALL_ATTRIB_HOOK_INT( nPushbackDisabled, airblast_pushback_disabled );

	if ( lfe_allow_airblast_physics.GetBool() && nPushbackDisabled != 1 )
	{
		float		flDist;
		while ((pEntity = gEntList.FindEntityInSphere( pEntity, GetAbsOrigin(), lfe_debug_airblast_physics_distance.GetFloat() )) != NULL)
		{
			flDist = (pEntity->WorldSpaceCenter() - GetAbsOrigin()).Length();

			CTakeDamageInfo info( this, this, 2, DMG_BLAST );
			CalculateExplosiveDamageForce( &info, (pEntity->GetAbsOrigin() - GetAbsOrigin()), pEntity->GetAbsOrigin() );

			if ( (pEntity->GetAbsOrigin() - GetAbsOrigin()).Length2D() <= lfe_debug_airblast_physics_distance.GetFloat() )
			{
				if ( pEntity->GetMoveType() == MOVETYPE_VPHYSICS || (pEntity->VPhysicsGetObject() && !pEntity->IsPlayer()) ) 
				{
					IPhysicsObject *pPhysObject = pEntity->VPhysicsGetObject();

					if ( pPhysObject )
					{
						float flMass = pPhysObject->GetMass();

						if ( flMass <= lfe_debug_airblast_physics_mass.GetFloat() )
						{
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
	}
}

//-----------------------------------------------------------------------------
// Purpose: Notify client that we're hitting an enemy.
//-----------------------------------------------------------------------------
void CTFWeaponFlameBall::SetHitTarget( void )
{
	//if ( m_bHitTarget )
	//{
		if ( tf_fireball_hit_recharge_boost.GetBool() )
		{
			// recharge faster when hit something.
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime - 0.5f;
		}
	//}
	//else
	//{
	//	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;
	//}

	SetContextThink( &CTFWeaponFlameBall::HitTargetThink, gpGlobals->curtime + 0.1f, "FlameBallHitTargetThink" );
}

void CTFWeaponFlameBall::HitTargetThink( void )
{
	if ( !m_bHitTarget )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;
	}
	else
	{
		SetContextThink( &CTFWeaponFlameBall::HitTargetThink, gpGlobals->curtime + 0.1f, "FlameBallHitTargetThink" );
	}
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
	SetPoseParameter( "reload", m_flNextPrimaryAttack = m_flNextSecondaryAttack );
	SetPoseParameter( "charge_level", m_flNextPrimaryAttack = m_flNextSecondaryAttack );

	C_ViewmodelAttachmentModel *pAttachment = GetViewmodelAddon();
	if ( pAttachment )
 	{
		pAttachment->SetPoseParameter( "reload", m_flNextPrimaryAttack = m_flNextSecondaryAttack );
		pAttachment->SetPoseParameter( "charge_level", m_flNextPrimaryAttack = m_flNextSecondaryAttack );
	}
}
#endif