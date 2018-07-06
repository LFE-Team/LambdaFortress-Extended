//=============================================================================//
//
// Purpose: arrow from tf2vintage
//
//=============================================================================//

#include "cbase.h"
#include "tf_projectile_arrow.h"
#include "effect_dispatch_data.h"
#include "tf_weaponbase.h"

#ifdef GAME_DLL
#include "SpriteTrail.h"
#include "props_shared.h"
#include "tf_player.h"
#include "debugoverlay_shared.h"
#include "te_effect_dispatch.h"
#endif

#ifdef GAME_DLL
ConVar tf_debug_arrows( "tf_debug_arrows", "0", FCVAR_CHEAT );
#endif

const char *g_pszArrowModels[] =
{
	"models/weapons/w_models/w_arrow.mdl",
	"models/weapons/w_models/w_syringe_proj.mdl",
	"models/weapons/w_models/w_repair_claw.mdl",
	//"models/weapons/w_models/w_arrow_xmas.mdl",
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Arrow, DT_TFProjectile_Arrow )

BEGIN_NETWORK_TABLE( CTFProjectile_Arrow, DT_TFProjectile_Arrow )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bCritical ) ),
	RecvPropBool( RECVINFO( m_bFlame ) ),
	RecvPropInt( RECVINFO( m_iType ) ),
#else
	SendPropBool( SENDINFO( m_bCritical ) ),
	SendPropBool( SENDINFO( m_bFlame ) ),
	SendPropInt( SENDINFO( m_iType ), 3, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFProjectile_Arrow )
	DEFINE_ENTITYFUNC( ArrowTouch )
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( tf_projectile_arrow, CTFProjectile_Arrow );
PRECACHE_REGISTER( tf_projectile_arrow );

CTFProjectile_Arrow::CTFProjectile_Arrow()
{
}

CTFProjectile_Arrow::~CTFProjectile_Arrow()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#else
	m_bCollideWithTeammates = false;
#endif
}

#ifdef GAME_DLL

CTFProjectile_Arrow *CTFProjectile_Arrow::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, float flSpeed, float flGravity, bool bFlame, CBaseEntity *pOwner, CBaseEntity *pScorer, int iType )
{
	CTFProjectile_Arrow *pArrow = static_cast<CTFProjectile_Arrow *>( CBaseEntity::CreateNoSpawn( "tf_projectile_arrow", vecOrigin, vecAngles, pOwner ) );

	if ( pArrow )
	{
		// Set team.
		pArrow->ChangeTeam( pOwner->GetTeamNumber() );

		// Set scorer.
		pArrow->SetScorer( pScorer );

		// Set firing weapon.
		pArrow->SetLauncher( pWeapon );

		// Set arrow type.
		pArrow->SetType( iType );

		// Set flame arrow.
		pArrow->SetFlameArrow( bFlame );

		// Spawn.
		DispatchSpawn( pArrow );

		// Setup the initial velocity.
		Vector vecForward, vecRight, vecUp;
		AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flSpeed, mult_projectile_speed );

		Vector vecVelocity = vecForward * flSpeed;
		pArrow->SetAbsVelocity( vecVelocity );
		pArrow->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

		// Setup the initial angles.
		QAngle angles;
		VectorAngles( vecVelocity, angles );
		pArrow->SetAbsAngles( angles );

		pArrow->SetGravity( flGravity );

		return pArrow;
	}

	return pArrow;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::Precache( void )
{
	// Precache all arrow models we have.
	for ( int i = 0; i < ARRAYSIZE( g_pszArrowModels ); i++ )
	{
		int iIndex = PrecacheModel( g_pszArrowModels[i] );
		PrecacheGibsForModel( iIndex );
	}

	for ( int i = FIRST_GAME_TEAM; i < TF_TEAM_COUNT; i++ )
	{
		PrecacheModel( ConstructTeamParticle( "effects/arrowtrail_%s.vmt", i, false, g_aTeamNamesShort ) );
		PrecacheModel( ConstructTeamParticle( "effects/healingtrail_%s.vmt", i, false, g_aTeamNamesShort ) );
		PrecacheModel( ConstructTeamParticle( "effects/repair_claw_trail_%s.vmt", i, false, g_aTeamParticleNames ) );
	}

	// Precache flame effects
	PrecacheParticleSystem( "flying_flaming_arrow" );
	PrecacheParticleSystem( "flying_flaming_arrow_smoke" );

	PrecacheScriptSound( "Weapon_Arrow.ImpactFlesh" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactMetal" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactWood" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactConcrete" );
	PrecacheScriptSound( "Weapon_Arrow.Nearmiss" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::Spawn( void )
{
	switch ( m_iType )
	{
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
		SetModel( g_pszArrowModels[2] );
		break;
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_FESTITIVE_HEALING_BOLT:
		SetModel( g_pszArrowModels[1] );
		break;
	default:
		SetModel( g_pszArrowModels[0] );
		break;
	}

	BaseClass::Spawn();

#ifdef TF_ARROW_FIX
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );
#endif

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 0.3f ); // TODO: Check again later.

	UTIL_SetSize( this, -Vector( 1, 1, 1 ), Vector( 1, 1, 1 ) );

	//FIXME: this isn't working and I don't know why
	if( m_bFlame )
	{
		DispatchParticleEffect( "flying_flaming_arrow", PATTACH_POINT_FOLLOW, this, "muzzle" );
		DispatchParticleEffect( "flying_flaming_arrow_smoke", PATTACH_POINT_FOLLOW, this, "muzzle" );
	}

	CreateTrail();

	SetTouch( &CTFProjectile_Arrow::ArrowTouch );
	SetThink(&CTFProjectile_Arrow::FlyThink);
	SetNextThink(gpGlobals->curtime);

	// TODO: Set skin here...
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Arrow::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::ArrowTouch( CBaseEntity *pOther )
{
	// Verify a correct "other."
	Assert( pOther );
	if ( pOther->IsSolidFlagSet( FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS ) )
		return;

	// Handle hitting skybox (disappear).
	trace_t *pTrace = const_cast<trace_t *>( &CBaseEntity::GetTouchTrace() );
	if ( pTrace->surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

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

	Vector vecOrigin = GetAbsOrigin();
	Vector vecDir = GetAbsVelocity();
	//CTFPlayer *pPlayer = ToTFPlayer( pOther );
	CBaseCombatCharacter *pPlayer = dynamic_cast<CBaseCombatCharacter*>( pOther );
	CTFWeaponBase *pWeapon = dynamic_cast<CTFWeaponBase *>( m_hLauncher.Get() );
	trace_t trHit, tr;
	trHit = *pTrace;

	//CTakeDamageInfo info_modified( this, pAttacker, m_flDamage, DMG_BULLET );
	CTakeDamageInfo info_modified;

	if ( pPlayer )
	{
		//trace_t trPlayerHit;
		//Trace ahead to see if we're going to hit player's hitbox.
		UTIL_TraceLine( vecOrigin, vecOrigin + vecDir * gpGlobals->frametime, MASK_SHOT, this, COLLISION_GROUP_NONE, &trHit );
		if ( trHit.m_pEnt != pOther )
		{
			// Determine where we should land

			Vector vecDir = GetAbsVelocity();
			VectorNormalizeFast( vecDir );
			/*
			CStudioHdr *pStudioHdr = pPlayer->GetModelPtr();
			if ( !pStudioHdr )
				return;

			mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( pPlayer->GetHitboxSet() );
			if ( !set )
				return;
			*/
			
			CStudioHdr *pHdr = pPlayer->GetModelPtr();
			if ( pHdr )
			{
				int set = pPlayer->GetHitboxSet();
				for( int i = 0; i < pHdr->iHitboxCount(set); i++ )
				{
					mstudiobbox_t *pBox = pHdr->pHitbox( i, set );
					if ( !pBox )
						continue;

					if ( pBox->group == HITGROUP_HEAD )
					{
						float flDamage = info_modified.GetDamage();
						bool bCritical = true;

						if ( pWeapon && !pWeapon->CanFireCriticalShot( true ) )
						{
							bCritical = false;
						}

						if ( bCritical )
						{
							info_modified.AddDamageType( DMG_CRITICAL );
							info_modified.SetDamageCustom( TF_DMG_CUSTOM_HEADSHOT );

							// play the critical shot sound to the shooter	
							if ( pWeapon )
							{
								pWeapon->WeaponSound( BURST );
								CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flDamage, headshot_damage_modify );
							}
						}

						info_modified.SetDamage( flDamage );

						break;
					}
				}
			}
/*
			// Oh boy... we gotta figure out the closest hitbox on player model to land a hit on.
			// Trace a bit ahead, to get closer to player's body.
			//UTIL_TraceLine( vecOrigin, vecOrigin + vecDir * 16.0f, MASK_SHOT, this, COLLISION_GROUP_NONE, &trPlayerHit );

			QAngle angHit;
			float flClosest = FLT_MAX;
			mstudiobbox_t *pBox;
			//int group = 0;
			//Msg( "\nNum of Hitboxes: %i", set->numhitboxes );

			for ( int i = 0; i < set->numhitboxes; i++ )
			{
				pBox = set->pHitbox( i );
				//Msg( "\nGroup: %i", pBox->group );

				Vector boxPosition;
				QAngle boxAngles;
				pPlayer->GetBonePosition( pBox->bone, boxPosition, boxAngles );
				Vector vecCross = CrossProduct( vecOrigin + vecDir * 16, boxPosition );

				trace_t tr;
				UTIL_TraceLine( vecOrigin, boxPosition, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

				float flLengthSqr = ( boxPosition - vecCross ).LengthSqr();
				if ( flLengthSqr < flClosest )
				{
					//Msg( "\nCLOSER: %i", pBox->group );
					//group = pBox->group;
					flClosest = flLengthSqr;
					trHit = tr;
				}
			}
			//Msg("\nClosest: %i\n", group);

			if ( tf_debug_arrows.GetBool() )
			{
				//Msg("\nHitBox: %i\nHitgroup: %i\n", trHit.hitbox, trHit.hitgroup);
				NDebugOverlay::Line( trHit.startpos, trHit.endpos, 0, 255, 0, true, 5.0f );
				NDebugOverlay::Line( vecOrigin, vecOrigin + vecDir * 16, 255, 0, 0, true, 5.0f );
			}
*/
			// Place arrow at hitbox.
			/*SetAbsOrigin( trHit.endpos );

			Vector vecHitDir = trHit.plane.normal * -1.0f;
			AngleVectors( angHit, &vecHitDir );
			SetAbsAngles( angHit );*/
		}
		EmitSound( "Weapon_Arrow.ImpactFlesh" );

		Vector vForward;

		AngleVectors( GetAbsAngles(), &vForward );
		VectorNormalize ( vForward );

		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + vForward * 128, MASK_BLOCKLOS, pOther, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction != 1.0f )
		{
			//NDebugOverlay::Box( tr.endpos, Vector( -16, -16, -16 ), Vector( 16, 16, 16 ), 0, 255, 0, 0, 10 );
			//NDebugOverlay::Box( GetAbsOrigin(), Vector( -16, -16, -16 ), Vector( 16, 16, 16 ), 0, 0, 255, 0, 10 );

			if ( tr.m_pEnt == NULL || ( tr.m_pEnt && tr.m_pEnt->GetMoveType() == MOVETYPE_NONE ) )
			{
				CEffectData	data;

				data.m_vOrigin = tr.endpos;
				data.m_vNormal = vForward;
				data.m_nEntIndex = tr.fraction != 1.0f;
			
				DispatchEffect( "BoltImpact", data );
			}
		}

	}
	else if ( pOther->GetMoveType() == MOVETYPE_NONE )
	{
		if ( pOther->IsBaseObject() )
		{
			EmitSound( "Weapon_Arrow.ImpactMetal" );
		}
		else
		{
			EmitSound( "Weapon_Arrow.ImpactConcrete" );
		}			
		//FIXME: We actually want to stick (with hierarchy) to what we've hit
		SetMoveType( MOVETYPE_NONE );

		Vector vForward;

		AngleVectors( GetAbsAngles(), &vForward );
		VectorNormalize ( vForward );

		CEffectData	data;

		data.m_vOrigin = tr.endpos;
		data.m_vNormal = vForward;
		data.m_nEntIndex = 0;

		DispatchEffect( "BoltImpact", data );

		UTIL_ImpactTrace( &trHit, DMG_BULLET );
	}
	else
	{
		UTIL_ImpactTrace( &trHit, DMG_BULLET );
		//UTIL_Remove( this );
	}

	// Do damage.
	CTakeDamageInfo info( this, pAttacker, pWeapon, GetDamage(), GetDamageType() );
	CalculateBulletDamageForce( &info, pWeapon ? pWeapon->GetTFWpnData().iAmmoType : 0, vecDir, vecOrigin );
	info.SetReportedPosition( pAttacker ? pAttacker->GetAbsOrigin() : vec3_origin );

	pOther->DispatchTraceAttack( info, vecDir, &trHit );
	ApplyMultiDamage();

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::FlyThink(void)
{
	QAngle angles;

	VectorAngles(GetAbsVelocity(), angles);

	SetAbsAngles(angles);

	SetNextThink(gpGlobals->curtime + 0.1f);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Arrow::GetDamageType()
{
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}
	if ( CanHeadshot() )
	{
		iDmgType |= DMG_USE_HITLOCATIONS;
	}
	if ( m_bFlame == true )
	{
		iDmgType |= DMG_IGNITE;	
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// Get arrow's speed.
	float flVel = GetAbsVelocity().Length();

	QAngle angForward;
	VectorAngles( vecDir, angForward );

	// Now change arrow's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );

	// And change owner.
	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );

	// Change trail color.
	if ( m_hSpriteTrail.Get() )
	{
		UTIL_Remove( m_hSpriteTrail.Get() );
	}

	CreateTrail();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFProjectile_Arrow::CanHeadshot( void )
{
	return ( m_iType == TF_PROJECTILE_ARROW || m_iType == TF_PROJECTILE_FESTITIVE_ARROW );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFProjectile_Arrow::GetTrailParticleName( void )
{
	const char *pszFormat = NULL;
	bool bLongTeamName = false;

	switch( m_iType )
	{
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
		pszFormat = "effects/repair_claw_trail_%s.vmt";
		bLongTeamName = true;
		break;
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_FESTITIVE_HEALING_BOLT:
		pszFormat = "effects/healingtrail_%s.vmt";
		break;
	default:
		pszFormat = "effects/arrowtrail_%s.vmt";
		break;
	}

	return ConstructTeamParticle( pszFormat, GetTeamNumber(), false, bLongTeamName ? g_aTeamParticleNames : g_aTeamNamesShort );
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::CreateTrail( void )
{
	CSpriteTrail *pTrail = CSpriteTrail::SpriteTrailCreate( GetTrailParticleName(), GetAbsOrigin(), true );

	if ( pTrail )
	{
		pTrail->FollowEntity( this );
		pTrail->SetTransparency( kRenderTransAlpha, -1, -1, -1, 255, kRenderFxNone );
		pTrail->SetStartWidth( m_iType == TF_PROJECTILE_BUILDING_REPAIR_BOLT ? 5.0f : 3.0f );
		pTrail->SetTextureResolution( 0.01f );
		pTrail->SetLifeTime( 0.3f );
		pTrail->TurnOn();

		pTrail->SetContextThink( &CBaseEntity::SUB_Remove, gpGlobals->curtime + 3.0f, "RemoveThink" );

		m_hSpriteTrail.Set( pTrail );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::UpdateOnRemove( void )
{
	UTIL_Remove( m_hSpriteTrail.Get() );

	BaseClass::UpdateOnRemove();
}

#endif
