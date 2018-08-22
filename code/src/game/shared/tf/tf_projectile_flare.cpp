//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#include "cbase.h"
#include "tf_projectile_flare.h"
#include "tf_weapon_compound_bow.h"
#include "tf_projectile_arrow.h"
// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "particles_new.h"
#include "iefx.h"
#include "dlight.h"
#include "tempent.h"
#include "c_te_legacytempents.h"
#else
#include "tf_player.h"
#include "tf_fx.h"
#include "ai_basenpc.h"
#include "effect_dispatch_data.h"
#include "collisionutils.h"
#include "tf_team.h"
#include "props.h"
#endif

#ifdef CLIENT_DLL
	extern ConVar lfe_muzzlelight;
#endif

#ifdef GAME_DLL
ConVar  tf_fireball_burn_duration("tf_fireball_burn_duration", "2.0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_burning_bonus("tf_fireball_burning_bonus", "3.0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_damage("tf_fireball_damage", "25.0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_distance("tf_fireball_distance", "500.0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_draw_debug_radius("tf_fireball_draw_debug_radius", "0", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_radius("tf_fireball_radius", "16", FCVAR_CHEAT | FCVAR_REPLICATED, "" ); // 22.5
ConVar  tf_fireball_speed("tf_fireball_speed", "3000", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
ConVar  tf_fireball_flametime("tf_fireball_flametime", "0.15", FCVAR_CHEAT | FCVAR_REPLICATED, "" );
#endif

#define TF_WEAPON_FLARE_MODEL		"models/weapons/w_models/w_flaregun_shell.mdl"
#define TF_WEAPON_FIREBALL_MODEL	"models/empty.mdl"

BEGIN_DATADESC( CTFProjectile_Flare )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_flare, CTFProjectile_Flare );
PRECACHE_REGISTER( tf_projectile_flare );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Flare, DT_TFProjectile_Flare )
BEGIN_NETWORK_TABLE( CTFProjectile_Flare, DT_TFProjectile_Flare )
#ifdef GAME_DLL
	SendPropBool( SENDINFO( m_bCritical ) ),
#else
	RecvPropBool( RECVINFO( m_bCritical ) ),
#endif
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFProjectile_Flare::CTFProjectile_Flare()
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFProjectile_Flare::~CTFProjectile_Flare()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#else
	m_bCollideWithTeammates = false;
#endif
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Precache()
{
	PrecacheModel( TF_WEAPON_FLARE_MODEL );

	PrecacheParticleSystem( "rockettrail_waterbubbles" );
	PrecacheTeamParticles( "flaregun_trail_%s", true );
	PrecacheTeamParticles( "flaregun_trail_crit_%s", true );

	PrecacheScriptSound( "TFPlayer.FlareImpact" );
	BaseClass::Precache();
}



//-----------------------------------------------------------------------------
// Purpose: Spawn function
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Spawn()
{
	SetModel( TF_WEAPON_FLARE_MODEL );
	BaseClass::Spawn();
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 0.3f );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Flare::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Flare::GetDamageType() 
{ 
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}
	if ( m_iDeflected > 0 )
	{
		iDmgType |= DMG_MINICRITICAL;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// Get rocket's speed.
	float flVel = GetAbsVelocity().Length();

	QAngle angForward;
	VectorAngles( vecDir, angForward );

	// Now change rocket's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );

	// And change owner.
	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Don't decal players with scorch.
	if ( !pOther->IsPlayer() )
	{
		UTIL_DecalTrace( pTrace, "Scorch" );
	}

	// Damage.
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	CTFPlayer *pPlayer = ToTFPlayer( pOther );
	CAI_BaseNPC *pNPC = pOther->MyNPCPointer();

	if ( pPlayer )
	{
		if ( !pPlayer )
			return;

		// Hit player, do impact sound
		if ( pPlayer->m_Shared.InCond( TF_COND_BURNING ) )
		{
			// Jeez, hardcoding this doesn't seem like a good idea.
			m_bCritical = true;
		}
		
		CPVSFilter filter( vecOrigin );
		EmitSound( filter, pPlayer->entindex(), "TFPlayer.FlareImpact" );
	}
	else if ( pNPC )
	{
		if ( !pNPC )
			return;
		// Hit npc, also do impact sound
		if ( pNPC->InCond( TF_COND_BURNING ) )
		{
			// Jeez, hardcoding this doesn't seem like a good idea.
			m_bCritical = true;
		}
		
		CPVSFilter filter( vecOrigin );
		EmitSound( filter, pNPC->entindex(), "TFPlayer.FlareImpact" );
	}
	else
	{
		// Hit world, do the explosion effect.
		CPVSFilter filter( vecOrigin );
		TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, GetWeaponID(), pOther->entindex() );
	}

	CTakeDamageInfo info( this, pAttacker, m_hLauncher.Get(), GetDamage(), GetDamageType(), TF_DMG_CUSTOM_BURNING_FLARE );
	info.SetReportedPosition( pAttacker ? pAttacker->GetAbsOrigin() : vec3_origin );
	pOther->TakeDamage( info );

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Flare *CTFProjectile_Flare::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Flare *pFlare = static_cast<CTFProjectile_Flare*>( CBaseEntity::CreateNoSpawn( "tf_projectile_flare", vecOrigin, vecAngles, pOwner ) );

	if ( pFlare )
	{
		// Set team.
		pFlare->ChangeTeam( pOwner->GetTeamNumber() );

		// Set scorer.
		pFlare->SetScorer( pScorer );

		// Set firing weapon.
		pFlare->SetLauncher( pWeapon );

		// Spawn.
		DispatchSpawn( pFlare );

		// Setup the initial velocity.
		Vector vecForward, vecRight, vecUp;
		AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

		float flVelocity = 2000.0f;
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flVelocity, mult_projectile_speed );

		Vector vecVelocity = vecForward * flVelocity;
		pFlare->SetAbsVelocity( vecVelocity );
		pFlare->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

		// Setup the initial angles.
		QAngle angles;
		VectorAngles( vecVelocity, angles );
		pFlare->SetAbsAngles( angles );
		return pFlare;
	}

	return pFlare;
}
#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateTrails();		
		CreateLight();
	}

	// Watch team changes and change trail accordingly.
	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
		CreateLight();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	if ( enginetrace->GetPointContents( GetAbsOrigin() ) & MASK_WATER )
	{
		ParticleProp()->Create( "rockettrail_waterbubbles", PATTACH_ABSORIGIN_FOLLOW );
	}
	else
	{
		const char *pszFormat = m_bCritical ? "flaregun_trail_crit_%s" : "flaregun_trail_%s";
		const char *pszEffectName = ConstructTeamParticle( pszFormat, GetTeamNumber(), false );

		ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
	}
}

void CTFProjectile_Flare::CreateLight( void )
{
	if ( IsEffectActive( EF_DIMLIGHT ) )
	{	
		dlight_t *dl = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );
		dl->origin = GetAbsOrigin();
		dl->color.r = 255;
		dl->color.g = 100;
		dl->color.b = 10;
		dl->radius = 128;
		dl->die = gpGlobals->curtime + 0.001;

		tempents->RocketFlare( GetAbsOrigin() );
	}
}

#endif

//=============================================================================
//
// Dragon's Fury Projectile
//

BEGIN_DATADESC( CTFProjectile_BallOfFire )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_balloffire, CTFProjectile_BallOfFire );
PRECACHE_REGISTER( tf_projectile_balloffire );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_BallOfFire, DT_TFProjectile_BallOfFire )
BEGIN_NETWORK_TABLE( CTFProjectile_BallOfFire, DT_TFProjectile_BallOfFire )
#ifdef GAME_DLL
	SendPropBool( SENDINFO( m_bCritical ) ),
#else
	RecvPropBool( RECVINFO( m_bCritical ) ),
#endif
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFProjectile_BallOfFire::CTFProjectile_BallOfFire()
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFProjectile_BallOfFire::~CTFProjectile_BallOfFire()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#else
	m_bCollideWithTeammates = false;
#endif
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::Precache()
{
	PrecacheModel( TF_WEAPON_FIREBALL_MODEL );

	PrecacheParticleSystem( "projectile_fireball" );
	PrecacheParticleSystem( "rockettrail_waterbubbles" );
	PrecacheTeamParticles( "projectile_fireball_crit_%s", true );

	PrecacheScriptSound( "Weapon_DragonsFury.Impact" );
	PrecacheScriptSound( "Weapon_DragonsFury.BonusDamageHit" );
	BaseClass::Precache();
}



//-----------------------------------------------------------------------------
// Purpose: Spawn function
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::Spawn()
{
	SetModel( TF_WEAPON_FIREBALL_MODEL );
	BaseClass::Spawn();
	SetMoveType( MOVETYPE_FLY, MOVECOLLIDE_FLY_CUSTOM );
	AddEFlags( EFL_NO_WATER_VELOCITY_CHANGE );
	SetGravity( 0.0f );

	SetSolid( SOLID_NONE );
	SetSolidFlags( FSOLID_NOT_SOLID );
	SetCollisionGroup( COLLISION_GROUP_NONE );

	float flRadius = tf_fireball_radius.GetFloat();
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( GetOwnerEntity(), flRadius, mult_flame_size );
	UTIL_SetSize( this, -Vector( flRadius, flRadius, flRadius ), Vector( flRadius, flRadius, flRadius ) );

	// Setup attributes.
	m_takedamage = DAMAGE_NO;
	m_vecInitialPos = GetOwnerEntity()->GetAbsOrigin();
	m_vecPrevPos = m_vecInitialPos;

	// Setup the think function.
	SetThink( &CTFProjectile_BallOfFire::FlameThink );
	SetNextThink( gpGlobals->curtime );

	float flLifeTime = tf_fireball_flametime.GetFloat();
	m_flTimeRemove = gpGlobals->curtime + ( flLifeTime );
}

//-----------------------------------------------------------------------------
// Purpose: Think method
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::FlameThink( void )
{
	// Render debug visualization if convar on
	if ( tf_fireball_draw_debug_radius.GetInt() )
	{
		NDebugOverlay::EntityBounds(this, 0, 100, 255, 0 ,0) ;
	}

	SetNextThink( gpGlobals->curtime );

	m_vecPrevPos = GetAbsOrigin();

	// Do collision detection.  We do custom collision detection because we can do it more cheaply than the
	// standard collision detection (don't need to check against world unless we might have hit an enemy) and
	// flame entity collision detection w/o this was a bottleneck on the X360 server
	//if ( GetAbsOrigin() != m_vecPrevPos )
	//{
		CBaseEntity *pBaseAttacker = GetOwnerEntity();
		IScorer *pScorerInterface = dynamic_cast<IScorer*>( pBaseAttacker );
		if ( pScorerInterface )
		{
			pBaseAttacker = pScorerInterface->GetScorer();
		}
		CTFPlayer *pAttacker = dynamic_cast<CTFPlayer *>( pBaseAttacker );
		if ( !pAttacker )
			return;

		CUtlVector<CTFTeam *> pTeamList, pTeamListOther;
		CTFTeam *pTeam = pAttacker->GetTFTeam(), *pTeamOther = pAttacker->GetOpposingTFTeam();
		if ( pTeam && pTeamOther )
		{
			pTeam->GetOpposingTFTeamList( &pTeamList );
			pTeamOther->GetOpposingTFTeamList( &pTeamListOther );
		}
		else
			return;

		bool bHitWorld = false;

		for ( int i = 0; i < pTeamList.Size(); i++ )
		{
			if (pTeamList[i])
			{
				// check collision against all enemy players
				for ( int iPlayer = 0; iPlayer < pTeamList[i]->GetNumPlayers(); iPlayer++ )
				{
					CBasePlayer *pPlayer = pTeamList[i]->GetPlayer(iPlayer);
					// Is this player connected, alive, and not us?
					if ( pPlayer && pPlayer->IsConnected() && pPlayer->IsAlive() && pPlayer!=pAttacker )
					{
						CheckCollision(pPlayer, &bHitWorld);
						if (bHitWorld)
							return;
					}
				}
/*
				// check collision against all enemy objects
				for ( int iObject = 0; iObject < pTeamList[i]->GetNumObjects(); iObject++ )
				{
					CBaseObject *pObject = pTeamList[i]->GetObject(iObject);
					if ( pObject )
					{
						CheckCollision(pObject, &bHitWorld);
						if (bHitWorld)
							return;
					}
				}
*/
				// check collision against all enemy NPCs
				for ( int iNPC = 0; iNPC < pTeamList[i]->GetNumNPCs(); iNPC++ )
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
	//}

	// Render debug visualization if convar on
	if ( tf_fireball_draw_debug_radius.GetInt() )
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

	/*float flDistance = GetAbsOrigin().DistTo( m_vecInitialPos );
	if ( flDistance <= tf_fireball_distance.GetFloat() )
 	{
		// can't go beyond limited distance
		UTIL_Remove( this );
 	}*/

	// if we've expired, remove ourselves
	if ( gpGlobals->curtime >= m_flTimeRemove )
	{
		#ifdef GAME_DLL
		CEffectData	data;
		data.m_nHitBox = GetParticleSystemIndex( "projectile_fireball_end" );
		data.m_vOrigin = WorldSpaceCenter();
		data.m_vAngles = vec3_angle;
		data.m_nEntIndex = 0;

		CPVSFilter filter( WorldSpaceCenter() );
		te->DispatchEffect( filter, 0.0, data.m_vOrigin, "ParticleEffect", data );
		#endif
		UTIL_Remove( this );
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::RocketTouch( CBaseEntity *pOther )
{
	BaseClass::RocketTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_BallOfFire::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_BallOfFire::GetDamageType() 
{ 
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}
	if ( m_iDeflected > 0 )
	{
		iDmgType |= DMG_MINICRITICAL;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// Get rocket's speed.
	float flVel = GetAbsVelocity().Length();

	QAngle angForward;
	VectorAngles( vecDir, angForward );

	// Now change rocket's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );

	// And change owner.
	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Don't decal players with scorch.
	if ( !pOther->IsPlayer() )
	{
		UTIL_DecalTrace( pTrace, "Scorch" );
	}

	#ifdef GAME_DLL
	CEffectData	data;
	data.m_nHitBox = GetParticleSystemIndex( "projectile_fireball_end" );
	data.m_vOrigin = WorldSpaceCenter();
	data.m_vAngles = vec3_angle;
	data.m_nEntIndex = 0;

	CPVSFilter filter( WorldSpaceCenter() );
	te->DispatchEffect( filter, 0.0, data.m_vOrigin, "ParticleEffect", data );
	#endif

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_BallOfFire *CTFProjectile_BallOfFire::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_BallOfFire *pFireball = static_cast<CTFProjectile_BallOfFire*>( CBaseEntity::CreateNoSpawn( "tf_projectile_balloffire", vecOrigin, vecAngles, pOwner ) );

	if ( pFireball )
	{
		// Set team.
		pFireball->ChangeTeam( pOwner->GetTeamNumber() );

		// Set scorer.
		pFireball->SetScorer( pScorer );

		// Set firing weapon.
		pFireball->SetLauncher( pWeapon );

		// Spawn.
		DispatchSpawn( pFireball );

		// Setup the initial velocity.
		Vector vecForward, vecRight, vecUp;
		AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

		float flVelocity = tf_fireball_speed.GetFloat();
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flVelocity, mult_projectile_speed );

		Vector vecVelocity = vecForward * flVelocity;
		pFireball->SetAbsVelocity( vecVelocity );
		pFireball->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

		// Setup the initial angles.
		QAngle angles;
		VectorAngles( vecVelocity, angles );
		pFireball->SetAbsAngles( angles );
		
		float flGravity = 0.0f;
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flGravity, mod_rocket_gravity );
		if ( flGravity )
		{
			pFireball->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
			pFireball->SetGravity( flGravity );
		}

		return pFireball;
	}

	return pFireball;
}
#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateTrails();
		CreateLight();
	}

	// Watch team changes and change trail accordingly.
	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
		CreateLight();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	if ( enginetrace->GetPointContents( GetAbsOrigin() ) & MASK_WATER )
	{
		ParticleProp()->Create( "rockettrail_waterbubbles", PATTACH_ABSORIGIN_FOLLOW );
	}
	else
	{
		const char *pszFormat = m_bCritical ? "projectile_fireball_crit_%s" : "projectile_fireball";
		const char *pszEffectName = ConstructTeamParticle( pszFormat, GetTeamNumber(), false );

		ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
	}
}

void CTFProjectile_BallOfFire::CreateLight( void )
{
	if ( IsEffectActive( EF_DIMLIGHT ) )
	{	
		dlight_t *dl = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );
		dl->origin = GetAbsOrigin();
		dl->color.r = 255;
		dl->color.g = 100;
		dl->color.b = 10;
		dl->radius = 256;
		dl->die = gpGlobals->curtime + 0.001;

		tempents->RocketFlare( GetAbsOrigin() );
	}
}
#endif

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: Checks collisions against other entities
//-----------------------------------------------------------------------------
void CTFProjectile_BallOfFire::CheckCollision( CBaseEntity *pOther, bool *pbHitWorld )
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
	ray.Init( GetAbsOrigin(), GetAbsOrigin(), WorldAlignMins(), WorldAlignMaxs() );
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
		UTIL_TraceLine( GetAbsOrigin() + vDir * WorldAlignMaxs().x, GetAbsOrigin(), MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &trWorld );			

		if ( tf_fireball_draw_debug_radius.GetInt() )
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
void CTFProjectile_BallOfFire::OnCollide( CBaseEntity *pOther )
{
	// remember that we've burnt this one
 	m_hEntitiesBurnt.AddToTail( pOther );

	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Damage.
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	CTFPlayer *pPlayer = ToTFPlayer( pOther );
	CAI_BaseNPC *pNPC = pOther->MyNPCPointer();

	float flDamage = 0;
	int iDamageCustom = TF_DMG_CUSTOM_DRAGONS_FURY_IGNITE;

	if ( pPlayer )
	{
		if ( !pPlayer )
			return;

		flDamage = tf_fireball_damage.GetFloat();

		// Hit player, do impact sound and more damage
		if (  m_hEntitiesBurnt.Count() > 0 )
		{
			// gotta recharge fast
			SetHitTarget();

			if ( pPlayer->m_Shared.InCond( TF_COND_BURNING ) )
			{
				flDamage = tf_fireball_damage.GetFloat() * tf_fireball_burning_bonus.GetFloat();

				iDamageCustom = TF_DMG_CUSTOM_DRAGONS_FURY_BONUS_BURNING;
				#ifdef GAME_DLL
				CEffectData	data;
				data.m_nHitBox = GetParticleSystemIndex( "dragons_fury_effect_parent" );
				data.m_vOrigin = pPlayer->GetAbsOrigin();
				data.m_vAngles = vec3_angle;
				data.m_nEntIndex = 0;

				CPVSFilter filter( vecOrigin );
				te->DispatchEffect( filter, 0.0, data.m_vOrigin, "ParticleEffect", data );

				EmitSound_t params;
				params.m_flSoundTime = 0;
				params.m_pSoundName = "Weapon_DragonsFury.BonusDamageHit";
				EmitSound( filter, pAttacker->entindex(), params );
				#endif
			}
		}

		iDamageCustom = TF_DMG_CUSTOM_DRAGONS_FURY_IGNITE;
	}
	else if ( pNPC )
	{
		if ( !pNPC )
			return;

		flDamage = tf_fireball_damage.GetFloat();

		// Hit npc, also do impact sound and more damage
		if ( m_hEntitiesBurnt.Count() > 0 )
		{
			// i said gotta recharge fast
			SetHitTarget();
			if ( pNPC->InCond( TF_COND_BURNING ) )
			{
				flDamage = tf_fireball_damage.GetFloat() * tf_fireball_burning_bonus.GetFloat();

				iDamageCustom = TF_DMG_CUSTOM_DRAGONS_FURY_BONUS_BURNING;
				#ifdef GAME_DLL
				CEffectData	data;
				data.m_nHitBox = GetParticleSystemIndex( "dragons_fury_effect_parent" );
				data.m_vOrigin = pNPC->GetAbsOrigin();
				data.m_vAngles = vec3_angle;
				data.m_nEntIndex = 0;

				CPVSFilter filter( vecOrigin );
				te->DispatchEffect( filter, 0.0, data.m_vOrigin, "ParticleEffect", data );

				EmitSound_t params;
				params.m_flSoundTime = 0;
				params.m_pSoundName = "Weapon_DragonsFury.BonusDamageHit";
				EmitSound( filter, pAttacker->entindex(), params );
				#endif
			}
		}

		iDamageCustom = TF_DMG_CUSTOM_DRAGONS_FURY_IGNITE;
	}
	else
	{
		// Hit world, delet this.
		//UTIL_Remove( this );
		ApplyMultiDamage();
	}

	CTakeDamageInfo info( GetOwnerEntity(), pAttacker, m_hLauncher.Get(), flDamage, GetDamageType(), iDamageCustom );
	pOther->TakeDamage( info );

	// Remove.
	//UTIL_Remove( this );

	SetHitTarget();

	info.SetReportedPosition( pAttacker->GetAbsOrigin() );	
		
	// We collided with pOther, so try to find a place on their surface to show blood
	trace_t pTrace;
	UTIL_TraceLine( WorldSpaceCenter(), pOther->WorldSpaceCenter(), MASK_SOLID|CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &pTrace );

	pOther->DispatchTraceAttack( info, GetAbsVelocity(), &pTrace );

	ApplyMultiDamage();

	CBreakableProp *pProp = dynamic_cast< CBreakableProp * >( pOther );
	if ( pProp )
	{
		// If we won't be able to break it, don't burn
		if ( pProp->m_takedamage == DAMAGE_YES )
		{
			pProp->IgniteLifetime( TF_BURNING_FLAME_LIFE );
			ApplyMultiDamage();
		}
	}
}

void CTFProjectile_BallOfFire::SetHitTarget( void )
{
	if ( m_hLauncher.Get() )
	{
		m_hLauncher->SetHitTarget();
	}
}
#endif