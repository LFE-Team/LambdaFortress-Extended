//=============================================================================//
//
// Purpose: Plasma blast used by MLG.
//
//=============================================================================//
#include "cbase.h"
#include "tf_projectile_plasma.h"

#ifdef CLIENT_DLL
#include "c_tf_player.h"
#else
#include "tf_player.h"
#include "tf_fx.h"
#include "tf_gamerules.h"
#endif

#define TF_WEAPON_PLASMA_MODEL "models/weapons/w_models/w_rocket.mdl"

ConVar tf_debug_plasma( "tf_debug_plasma", "0", FCVAR_CHEAT );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Plasma, DT_TFProjectile_Plasma );
BEGIN_NETWORK_TABLE( CTFProjectile_Plasma, DT_TFProjectile_Plasma )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_projectile_plasma, CTFProjectile_Plasma );
PRECACHE_REGISTER( tf_projectile_plasma );

CTFProjectile_Plasma::CTFProjectile_Plasma()
{
}

CTFProjectile_Plasma::~CTFProjectile_Plasma()
{
#ifdef GAME_DLL
	m_bCollideWithTeammates = false;
#else
	ParticleProp()->StopEmission();
#endif
}

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Plasma *CTFProjectile_Plasma::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Plasma *pRocket = static_cast<CTFProjectile_Plasma *>( CTFBaseRocket::Create( pWeapon, "tf_projectile_plasma", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Spawn()
{
	// Setting a model here because baseclass wants it.
	SetModel( TF_WEAPON_PLASMA_MODEL );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Precache()
{
	PrecacheModel( TF_WEAPON_PLASMA_MODEL );

	PrecacheParticleSystem( "mlg_trail_primary" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	BaseClass::Explode( pTrace, pOther );

	// Create the bomblets.
	const int nBombs = 20;
	const float flStep = 360.0f / (float)nBombs;

	for ( int i = 0; i < nBombs; i++ )
	{
		CTFProjectile_PlasmaBomb *pBomb = CTFProjectile_PlasmaBomb::Create( m_hLauncher.Get(), GetAbsOrigin(), GetAbsAngles(), GetOwnerEntity(), GetScorer() );

		// Get direction and apply deviation
		Vector vecDir;
		QAngle angles( RandomFloat( -20.f, -70.f ), (float)i * flStep + RandomFloat( -flStep / 2, flStep / 2 ), 0.0f );
		AngleVectors( angles, &vecDir );

		// Correct the rotation
		VMatrix matAdjust;
		MatrixBuildRotation(matAdjust, Vector(0, 0, 1), pTrace->plane.normal);
		vecDir = matAdjust * vecDir;
		
		// Pull it out a bit so it doesn't collide with other bomblets.
		pBomb->SetAbsOrigin( GetAbsOrigin() + vecDir * 1.0f );
		pBomb->SetAbsVelocity( vecDir * RandomFloat( 250, 750 ) );
		pBomb->SetAbsAngles( angles );

		if ( tf_debug_plasma.GetBool() )
		{
			NDebugOverlay::Line( pBomb->GetAbsOrigin(), pBomb->GetAbsOrigin() + vecDir * 50.0f, 0, 255, 0, true, 10.0f );
		}

		pBomb->SetDamage( GetDamage() / 3 );
		pBomb->SetCritical( m_bCritical );
	}
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::OnDataChanged( DataUpdateType_t updateType )
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
void CTFProjectile_Plasma::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	CNewParticleEffect *pParticle = ParticleProp()->Create( "mlg_trail_primary", PATTACH_ABSORIGIN_FOLLOW );

	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner )
	{
		pOwner->m_Shared.SetParticleToMercColor( pParticle );
	}
}

#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_PlasmaBomb, DT_TFProjectile_PlasmaBomb );
BEGIN_NETWORK_TABLE( CTFProjectile_PlasmaBomb, DT_TFProjectile_PlasmaBomb )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_projectile_plasma_bomb, CTFProjectile_PlasmaBomb );
PRECACHE_REGISTER( tf_projectile_plasma_bomb );

#define TF_WEAPON_PLASMA_BOMB_MODEL "models/weapons/w_models/w_flaregun_shell.mdl"

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_PlasmaBomb *CTFProjectile_PlasmaBomb::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_PlasmaBomb *pBomb = static_cast<CTFProjectile_PlasmaBomb *>( CTFBaseRocket::Create( pWeapon, "tf_projectile_plasma_bomb", vecOrigin, vecAngles, pOwner ) );

	if ( pBomb )
	{
		pBomb->SetScorer( pScorer );
	}

	return pBomb;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomb::Precache()
{
	PrecacheModel( TF_WEAPON_PLASMA_BOMB_MODEL );

	PrecacheParticleSystem( "mlg_trail_secondary" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomb::Spawn( void )
{
	SetModel( TF_WEAPON_PLASMA_BOMB_MODEL );
	BaseClass::Spawn();
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 1.0f );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomb::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Need to think of something here.
	BaseClass::Explode( pTrace, pOther );
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomb::OnDataChanged( DataUpdateType_t updateType )
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
void CTFProjectile_PlasmaBomb::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	CNewParticleEffect *pParticle = ParticleProp()->Create( "mlg_trail_secondary", PATTACH_ABSORIGIN_FOLLOW );

	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner )
	{
		pOwner->m_Shared.SetParticleToMercColor( pParticle );
	}
}

#endif
