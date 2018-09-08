//=========== Copyright © 2018, LFE-Team, Not All rights reserved. ============
//
// Purpose:
//
//=============================================================================
#include "cbase.h"
#include "tf_fx_shared.h"
#include "ammodef.h"
#include "tf_gamerules.h"
#include "tf_player_shared.h"
#include "props_shared.h"
#include "tf_weapon_compound_bow.h"
#include "tf_projectile_arrow.h"
#include "eventlist.h"
#include "props_shared.h"
#include "tf_flame.h"
#include "particle_parse.h"
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
	#include "ilagcompensationmanager.h"
	#include "collisionutils.h"
	#include "tf_team.h"
	#include "tf_obj.h"
	#include "ai_basenpc.h"
	#include "npc_antlion.h"
	#include "props.h"
	#include "soundent.h"

	extern ConVar	tf_debug_flamethrower;
	extern ConVar	tf_flamethrower_velocity;
	extern ConVar	tf_flamethrower_drag;
	extern ConVar	tf_flamethrower_float;
	extern ConVar	tf_flamethrower_flametime;
	extern ConVar	tf_flamethrower_vecrand;
	extern ConVar	tf_flamethrower_boxsize;
	extern ConVar	tf_flamethrower_maxdamagedist;
	extern ConVar	tf_flamethrower_shortrangedamagemultiplier;
	extern ConVar	tf_flamethrower_velocityfadestart;
	extern ConVar	tf_flamethrower_velocityfadeend;
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
//
// TF Flame tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFFlameEntity, DT_TFFlameEntity )

BEGIN_NETWORK_TABLE( CTFFlameEntity, DT_TFFlameEntity )
END_NETWORK_TABLE()

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFFlameEntity )
	DEFINE_THINKFUNC( FlameThink ),
END_DATADESC()
#endif

IMPLEMENT_AUTO_LIST( ITFFlameEntityAutoList );

LINK_ENTITY_TO_CLASS( tf_flame, CTFFlameEntity );

//=============================================================================
//
// Shared (client/server) functions.
//
//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFFlameEntity::CTFFlameEntity()
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFFlameEntity::~CTFFlameEntity()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameEntity::Precache( void )
{
	BaseClass::Precache();

	PrecacheParticleSystem( "new_flame_core" );
}

//-----------------------------------------------------------------------------
// Purpose: Spawns this entity
//-----------------------------------------------------------------------------
void CTFFlameEntity::Spawn( void )
{
	BaseClass::Spawn();

	// Precache.
	Precache();

#ifdef GAME_DLL
	// don't collide with anything, we do our own collision detection in our think method
	SetSolid( SOLID_NONE );
	SetSolidFlags( FSOLID_NOT_SOLID );
	SetCollisionGroup( COLLISION_GROUP_NONE );
	AddEFlags( EFL_NO_WATER_VELOCITY_CHANGE );

	float flGravity = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flGravity, flame_gravity );
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_SLIDE );
	SetGravity( flGravity );

	float flBoxSize = tf_flamethrower_boxsize.GetFloat();
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flBoxSize, mult_flame_size );
	UTIL_SetSize( this, -Vector( flBoxSize, flBoxSize, flBoxSize ), Vector( flBoxSize, flBoxSize, flBoxSize ) );

	// Setup attributes.
	m_takedamage = DAMAGE_NO;
	m_vecInitialPos = GetAbsOrigin();
	m_vecPrevPos = m_vecInitialPos;

	float flLifeTime = tf_flamethrower_flametime.GetFloat();
	float flLifeTimeRandom = random->RandomFloat( 0.9, 1.1 );
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flLifeTime, flame_lifetime );
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flLifeTimeRandom, flame_random_life_time_offset );
	m_flTimeRemove = gpGlobals->curtime + ( flLifeTime * flLifeTimeRandom );

	m_hLauncher = dynamic_cast<CTFFlameThrower *>( GetOwnerEntity() );

	// Setup the think function.
	SetThink( &CTFFlameEntity::FlameThink );
	SetNextThink( gpGlobals->curtime );
#endif
}

//=============================================================================
//
// Client specific functions.
//
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameEntity::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateTrails();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlameEntity::CreateTrails( void )
{
	if ( IsDormant() )
		return;
/*
	const char *pszParticleEffect = "new_flame";
 	// Halloween Spell Effect Check
	int iGreenFlames = 0;
	CALL_ATTRIB_HOOK_INT_ON_OTHER( GetOwnerEntity(), iGreenFlames, halloween_green_flames__halloween );

 	if ( iGreenFlames != 0 )
	{
		pszParticleEffect = "new_flame_fan";
	}

	CNewParticleEffect *pFlameEffect = ParticleProp()->Create( pszParticleEffect, PATTACH_ABSORIGIN_FOLLOW );
	ParticleProp()->AddControlPoint( pFlameEffect, 1, this, PATTACH_ABSORIGIN_FOLLOW );
*/
}

//=============================================================================
//
// Server specific functions.
//
#else
//-----------------------------------------------------------------------------
// Purpose: Creates an instance of this entity
//-----------------------------------------------------------------------------
CTFFlameEntity *CTFFlameEntity::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, int iDmgType, float flDmgAmount )
{
	CTFFlameEntity *pFlame = static_cast<CTFFlameEntity*>( CBaseEntity::Create( "tf_flame", vecOrigin, vecAngles, pOwner ) );
	if ( !pFlame )
		return NULL;

	// Initialize the owner.
	pFlame->SetOwnerEntity( pOwner );
	pFlame->m_hAttacker = pOwner->GetOwnerEntity();
	CBaseEntity *pAttacker = (CBaseEntity *) pFlame->m_hAttacker;
	if ( pAttacker )
	{
		pFlame->m_iAttackerTeam = pAttacker->GetTeamNumber();
	}

	// Set team.
	pFlame->ChangeTeam( pOwner->GetTeamNumber() );
	pFlame->m_iDmgType = iDmgType;
	pFlame->m_flDmgAmount = flDmgAmount;

	// Setup the initial velocity.
	Vector vecForward, vecRight, vecUp;
	AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

	float flVelocity = tf_flamethrower_velocity.GetFloat();
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pOwner, flVelocity, flame_speed );
	pFlame->m_vecBaseVelocity = vecForward * flVelocity;
	pFlame->m_vecAttackerVelocity = pOwner->GetOwnerEntity()->GetAbsVelocity();
	pFlame->SetAbsVelocity( pFlame->m_vecBaseVelocity );	
	// Setup the initial angles.
	pFlame->SetAbsAngles( vecAngles );

	return pFlame;
}

//-----------------------------------------------------------------------------
// Purpose: Think method
//-----------------------------------------------------------------------------
void CTFFlameEntity::FlameThink( void )
{
	// if we've expired, remove ourselves
	if ( gpGlobals->curtime >= m_flTimeRemove )
	{
		UTIL_Remove( this );
		return;
	}

	// Do collision detection.  We do custom collision detection because we can do it more cheaply than the
	// standard collision detection (don't need to check against world unless we might have hit an enemy) and
	// flame entity collision detection w/o this was a bottleneck on the X360 server
	if ( GetAbsOrigin() != m_vecPrevPos )
	{
		CTFPlayer *pAttacker = dynamic_cast<CTFPlayer *>( (CBaseEntity *) m_hAttacker );
		if ( !pAttacker )
			return;

		CUtlVector<CTFTeam *> pTeamList, pTeamListOther;
		CTFTeam *pTeam = pAttacker->GetTFTeam(), *pTeamOther = pAttacker->GetOpposingTFTeam();
		if ( pTeam && pTeamOther )
		{
			pTeam->GetOpposingTFTeamList(&pTeamList);
			pTeamOther->GetOpposingTFTeamList(&pTeamListOther);
		}
		else
			return;

		bool bHitWorld = false;

		for (int i = 0; i < pTeamList.Size(); i++)
		{
			if (pTeamList[i])
			{
				// check collision against all enemy players
				for (int iPlayer = 0; iPlayer < pTeamList[i]->GetNumPlayers(); iPlayer++)
				{
					CBasePlayer *pPlayer = pTeamList[i]->GetPlayer(iPlayer);
					// Is this player connected, alive, and not us?
					if (pPlayer && pPlayer->IsConnected() && pPlayer->IsAlive() && pPlayer!=pAttacker)
					{
						CheckCollision(pPlayer, &bHitWorld);
						if (bHitWorld)
							return;
					}
				}

				// check collision against all enemy objects
				for (int iObject = 0; iObject < pTeamList[i]->GetNumObjects(); iObject++)
				{
					CBaseObject *pObject = pTeamList[i]->GetObject(iObject);
					if (pObject)
					{
						CheckCollision(pObject, &bHitWorld);
						if (bHitWorld)
							return;
					}
				}

				// check collision against all enemy NPCs
				for (int iNPC = 0; iNPC < pTeamList[i]->GetNumNPCs(); iNPC++)
				{
					CAI_BaseNPC *pNPC = pTeamList[i]->GetNPC(iNPC);
					// Is this NPC alive and an enemy?
					if ( pNPC && pNPC->IsAlive() )
					{
						CheckCollision( pNPC, &bHitWorld );
						if ( bHitWorld )
							return;
					}
				}

				// check collision against all players on our team
				for ( int iPlayer = 0; iPlayer < pTeamListOther[i]->GetNumPlayers(); iPlayer++ )
				{
					CBasePlayer *pPlayer = pTeamListOther[i]->GetPlayer( iPlayer );
					// Is this player connected, alive, and an enemy?
					if ( pPlayer && pPlayer->IsConnected() && pPlayer->IsAlive() && pPlayer!=pAttacker )
					{
						CheckCollision( pPlayer, &bHitWorld );
						if ( bHitWorld )
							return;
					}
				}
			}
		}
	}

	// Calculate how long the flame has been alive for
	float flFlameElapsedTime = tf_flamethrower_flametime.GetFloat() - ( m_flTimeRemove - gpGlobals->curtime );
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flFlameElapsedTime, mult_flame_life );
	// Calculate how much of the attacker's velocity to blend in to the flame's velocity.  The flame gets the attacker's velocity
	// added right when the flame is fired, but that velocity addition fades quickly to zero.
	float flAttackerVelocityBlend = RemapValClamped( flFlameElapsedTime, tf_flamethrower_velocityfadestart.GetFloat(), 
		tf_flamethrower_velocityfadeend.GetFloat(), 1.0, 0 );

	// Reduce our base velocity by the air drag constant
	m_vecBaseVelocity *= tf_flamethrower_drag.GetFloat();

	// Add our float upward velocity
	float flUpSpeed = tf_flamethrower_float.GetFloat();
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flUpSpeed, flame_up_speed );
	Vector vecVelocity = m_vecBaseVelocity + Vector( 0, 0, flUpSpeed ) + ( flAttackerVelocityBlend * m_vecAttackerVelocity );

	// Update our velocity
	SetAbsVelocity( vecVelocity += RandomVector( -flAttackerVelocityBlend * tf_flamethrower_vecrand.GetFloat(), flAttackerVelocityBlend * tf_flamethrower_vecrand.GetFloat() ) );

	// Render debug visualization if convar on
	if ( tf_debug_flamethrower.GetInt() )
	{
		if ( m_hEntitiesBurnt.Count() > 0 )
		{
			int val = ( (int) ( gpGlobals->curtime * 10 ) ) % 255;
			NDebugOverlay::EntityBounds(this, val, 255, val, 0 ,0 );
		} 
		else 
		{
			NDebugOverlay::EntityBounds(this, 0, 100, 255, 0 ,0) ;
		}
	}

	SetNextThink( gpGlobals->curtime );

	m_vecPrevPos = GetAbsOrigin();
}

//-----------------------------------------------------------------------------
// Purpose: Checks collisions against other entities
//-----------------------------------------------------------------------------
void CTFFlameEntity::CheckCollision( CBaseEntity *pOther, bool *pbHitWorld )
{
	CTFCompoundBow *pBow = NULL;
	CBreakableProp *pProp = NULL;
	*pbHitWorld = false;

	// if we've already burnt this entity, don't do more damage, so skip even checking for collision with the entity
	int iIndex = m_hEntitiesBurnt.Find( pOther );
	if ( iIndex != m_hEntitiesBurnt.InvalidIndex() )
		return;

	// if the entity is on our team check if it's a player carrying a bow
	if ( pOther->GetTeam() == GetTeam() )
	{
		CTFPlayer *pPlayer = ToTFPlayer( pOther );
		pBow = dynamic_cast<CTFCompoundBow *>( pPlayer->GetActiveTFWeapon() );
		if( !pBow )
		{
			// not a valid target
			return;
		}
	}

	// Do a bounding box check against the entity
	Vector vecMins, vecMaxs;
	pOther->GetCollideable()->WorldSpaceSurroundingBounds( &vecMins, &vecMaxs );
	CBaseTrace trace;
	Ray_t ray;
	float flFractionLeftSolid;				
	ray.Init( m_vecPrevPos, GetAbsOrigin(), WorldAlignMins(), WorldAlignMaxs() );
	if ( IntersectRayWithBox( ray, vecMins, vecMaxs, 0.0, &trace, &flFractionLeftSolid ) )
	{
		// if bounding box check passes, check player hitboxes
		trace_t trHitbox;
		trace_t trWorld;
		//bool bTested = pOther->GetCollideable()->TestHitboxes( ray, MASK_SOLID | CONTENTS_HITBOX, trHitbox );
		bool bTested = pOther->GetCollideable()->TestHitboxes( ray, MASK_SHOT, trHitbox );
		if ( !bTested || !trHitbox.DidHit() )
			return;

		// now, let's see if the flame visual could have actually hit this player.  Trace backward from the
		// point of impact to where the flame was fired, see if we hit anything.  Since the point of impact was
		// determined using the flame's bounding box and we're just doing a ray test here, we extend the
		// start point out by the radius of the box.
		Vector vDir = ray.m_Delta;
		vDir.NormalizeInPlace();
		UTIL_TraceLine( GetAbsOrigin() + vDir * WorldAlignMaxs().x, m_vecInitialPos, MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &trWorld );			

		if ( tf_debug_flamethrower.GetInt() )
		{
			NDebugOverlay::Line( trWorld.startpos, trWorld.endpos, 0, 255, 0, true, 3.0f );
		}
		
		if ( trWorld.fraction == 1.0 )
		{
			if ( pBow )
			{
				m_hEntitiesBurnt.AddToTail( pOther );
				pBow->LightArrow();
				return;
			}

			// if there is nothing solid in the way, damage the entity
			OnCollide( pOther );
		}
		else
		{
			// we hit the world
			*pbHitWorld = true;

			if ( pProp )
			{
				// If we won't be able to break it, don't burn
				if ( pProp->m_takedamage == DAMAGE_YES )
				{
					m_hEntitiesBurnt.AddToTail( pOther );
					pProp->IgniteLifetime( TF_BURNING_FLAME_LIFE );
					//pProp->ApplyMultiDamage();
					return;
				}
			}

			OnCollide( pOther );
			//UTIL_Remove( this );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when we've collided with another entity
//-----------------------------------------------------------------------------
void CTFFlameEntity::OnCollide( CBaseEntity *pOther )
{
	// remember that we've burnt this player
 	m_hEntitiesBurnt.AddToTail( pOther );
 
	float flDistance = GetAbsOrigin().DistTo( m_vecInitialPos );
	float flMultiplier;
	if ( flDistance <= 125 )
 	{
		// at very short range, apply short range damage multiplier
		flMultiplier = tf_flamethrower_shortrangedamagemultiplier.GetFloat();
 	}
	else
	{
		// make damage ramp down from 100% to 60% from half the max dist to the max dist
		flMultiplier = RemapValClamped( flDistance, tf_flamethrower_maxdamagedist.GetFloat()/2, tf_flamethrower_maxdamagedist.GetFloat(), 1.0, 0.6 );
	}

	float flDamage = m_flDmgAmount * flMultiplier;
	flDamage = max( flDamage, 1.0 );
	if ( tf_debug_flamethrower.GetInt() )
	{
		Msg( "Flame touch dmg: %.1f\n", flDamage );
	}

	CBaseEntity *pAttacker = m_hAttacker;
	if ( !pAttacker )
		return;

	SetHitTarget();

	// if we're behind the target and we're close enough.
	int nBackCrit = 0;
	CALL_ATTRIB_HOOK_INT_ON_OTHER( GetOwnerEntity(), nBackCrit, set_flamethrower_back_crit );
	if ( nBackCrit != 0 )
	{
		trace_t tr;
		if ( tr.m_pEnt->IsPlayer() && tr.m_pEnt->GetTeamNumber() != GetOwnerEntity()->GetTeamNumber() &&
			m_hLauncher->IsBehindAndFacingTarget( tr.m_pEnt ) )
			{
				CTakeDamageInfo info( GetOwnerEntity(), pAttacker, GetOwnerEntity(), flDamage, DMG_CRITICAL, TF_DMG_CUSTOM_BURNING );
				info.SetReportedPosition( pAttacker->GetAbsOrigin() );	
				
				// We collided with pOther, so try to find a place on their surface to show blood
				trace_t pTrace;
				UTIL_TraceLine( WorldSpaceCenter(), pOther->WorldSpaceCenter(), MASK_SOLID|CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &pTrace );

				pOther->DispatchTraceAttack( info, GetAbsVelocity(), &pTrace );
			}
	}
	else
	{
		CTakeDamageInfo info( GetOwnerEntity(), pAttacker, GetOwnerEntity(), flDamage, m_iDmgType, TF_DMG_CUSTOM_BURNING );
		info.SetReportedPosition( pAttacker->GetAbsOrigin() );	
		
		// We collided with pOther, so try to find a place on their surface to show blood
		trace_t pTrace;
		UTIL_TraceLine( WorldSpaceCenter(), pOther->WorldSpaceCenter(), MASK_SOLID|CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &pTrace );

		pOther->DispatchTraceAttack( info, GetAbsVelocity(), &pTrace );
	}
	ApplyMultiDamage();

	// It's better to ignite NPC here rather than NPC code.
	CAI_BaseNPC *pNPC = pOther->MyNPCPointer();
	if ( pNPC )
	{
		pNPC->Ignite( TF_BURNING_FLAME_LIFE );
		// I don't like this but Ignite doesn't allow us to set attacker so we have to do it separately. -nicknine?
		pNPC->SetBurnAttacker( pAttacker );
	}

	CBreakableProp *pProp = dynamic_cast< CBreakableProp * >( pOther );
	if ( pProp )
	{
		// If we won't be able to break it, don't burn
		if ( pProp->m_takedamage == DAMAGE_YES )
		{
			pProp->IgniteLifetime( TF_BURNING_FLAME_LIFE );
			//pProp->ApplyMultiDamage();
		}
	}
}

void CTFFlameEntity::SetHitTarget( void )
{
	if ( m_hLauncher.Get() )
	{
		m_hLauncher->SetHitTarget();
	}
}

#endif // GAME_DLL
