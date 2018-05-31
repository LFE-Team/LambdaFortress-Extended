//=============================================================================
//
// Purpose: MLG
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_displacer.h"
#include "tf_gamerules.h"
#include "particle_parse.h"

#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif

//=============================================================================
//
// Weapon Displacer tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponDisplacer, DT_WeaponDisplacer );
BEGIN_NETWORK_TABLE( CTFWeaponDisplacer, DT_WeaponDisplacer )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flBlastTime ) ),
	RecvPropTime( RECVINFO( m_flTeleportTime ) ),
	RecvPropBool( RECVINFO( m_bLockedOn ) ),
#else
	SendPropTime( SENDINFO( m_flBlastTime ) ),
	SendPropTime( SENDINFO( m_flTeleportTime ) ),
	SendPropBool( SENDINFO( m_bLockedOn ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponDisplacer )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_flBlastTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flTeleportTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_displacer, CTFWeaponDisplacer );
PRECACHE_WEAPON_REGISTER( tf_weapon_displacer );

#ifdef GAME_DLL
extern EHANDLE g_pLastSpawnPoints[TF_TEAM_COUNT];
#endif

//=============================================================================
//
// Weapon Displacer functions.
//

CTFWeaponDisplacer::CTFWeaponDisplacer()
{
	m_bPlayedTeleportEffect = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::Precache( void )
{
#ifdef GAME_DLL
	PrecacheParticleSystem( "mlg_chargeup" );
	PrecacheParticleSystem( "mlg_teleported_away" );

	// TEMP!!!
	PrecacheTeamParticles( "teleported_%s", true );
	PrecacheTeamParticles( "teleportedin_%s", true );
#endif

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::WeaponReset( void )
{
	m_flBlastTime = 0.0f;
	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;
	m_bPlayedTeleportEffect = false;

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	BaseClass::WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::CanHolster( void ) const
{
	// Can't holster while charging up a shot.
	if ( IsChargingAnyShot() )
	{
		return false;
	}

	return BaseClass::CanHolster();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_flBlastTime = 0.0f;
	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::ItemPostFrame( void )
{
	if ( m_flBlastTime != 0.0f && gpGlobals->curtime >= m_flBlastTime )
	{
		// Deliberately skipping to base class since our function starts charging.
		m_flBlastTime = 0.0f;
		BaseClass::PrimaryAttack();
	}
	else if ( m_flTeleportTime != 0.0f )
	{
		if ( gpGlobals->curtime >= m_flTeleportTime )
		{
			FinishTeleport();
		}
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::PrimaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Already charging up a shot?
	if ( IsChargingAnyShot() )
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	// Start charging.
	WeaponSound( SPECIAL2 );
	m_flBlastTime = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSmackDelay;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::SecondaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Already charging up a shot?
	if ( IsChargingAnyShot() )
		return;

	// Do we have enough ammo?
	if ( pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) < m_pWeaponInfo->GetWeaponData( TF_WEAPON_SECONDARY_MODE ).m_iAmmoPerShot )
		return;

	m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;

#ifdef GAME_DLL
	// Find a furthest possible respawn point.
	CBaseEntity *pSpot = g_pLastSpawnPoints[pPlayer->GetTeamNumber()];

	if ( pPlayer->SelectFurthestSpawnSpot( "info_player_deathmatch", pSpot, false ) )
	{
		// Gotta remove prediction filtering since this code runs on server side only.
		CDisablePredictionFiltering disabler;

		// Create a warning effect for other players at the chosen destination.
		const char *pszTeleportedEffect = ConstructTeamParticle( "teleported_%s", pPlayer->GetTeamNumber(), true );
		DispatchParticleEffect( pszTeleportedEffect, pSpot->GetAbsOrigin(), vec3_angle, pPlayer->m_vecPlayerColor, vec3_origin, true );

		g_pLastSpawnPoints[pPlayer->GetTeamNumber()] = pSpot;
		m_hTeleportSpot = pSpot;
		m_bLockedOn = true;
	}
#endif

	WeaponSound( SPECIAL3 );

	m_bPlayedTeleportEffect = false;
	m_flTeleportTime = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSmackDelay;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::IsChargingAnyShot( void ) const
{
	return ( m_flBlastTime || m_flTeleportTime );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::FinishTeleport( void )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
#ifdef GAME_DLL
		AssertMsg( m_hTeleportSpot.Get() != NULL, "Displacer teleport was triggered with no destination." );

		if ( m_hTeleportSpot.Get() )
#else
		if ( m_bLockedOn )
#endif
		{
#ifdef GAME_DLL
			// Gotta remove prediction filtering since this code runs on server side only.
			CDisablePredictionFiltering *pDisabler = new CDisablePredictionFiltering();

			// Telelefrag anyone in the way.
			CBaseEntity *pList[MAX_PLAYERS];
			Vector vecMins = m_hTeleportSpot->GetAbsOrigin() + VEC_HULL_MIN_SCALED( pPlayer );
			Vector vecMaxs = m_hTeleportSpot->GetAbsOrigin() + VEC_HULL_MAX_SCALED( pPlayer );
			int count = UTIL_EntitiesInBox( pList, MAX_PLAYERS, vecMins, vecMaxs, FL_CLIENT );

			for ( int i = 0; i < count; i++ )
			{
				CBaseEntity *pEntity = pList[i];
				if ( pEntity != this && ( !InSameTeam( pEntity ) || TFGameRules()->IsDeathmatch() ) )
				{
					CTakeDamageInfo info( this, this, 1000, DMG_CRUSH, TF_DMG_TELEFRAG );
					pEntity->TakeDamage( info );
				}
			}

			// Play departure effect.
			DispatchParticleEffect( "mlg_teleported_away", pPlayer->GetAbsOrigin(), vec3_angle, pPlayer->m_vecPlayerColor, vec3_origin, true );

			pPlayer->Teleport( &m_hTeleportSpot->GetAbsOrigin(), &m_hTeleportSpot->GetAbsAngles(), &vec3_origin );

			// Play arrival effect.
			const char *pszTeleportedEffect = ConstructTeamParticle( "teleportedin_%s", pPlayer->GetTeamNumber(), true );
			DispatchParticleEffect( pszTeleportedEffect, m_hTeleportSpot->GetAbsOrigin(), vec3_angle, pPlayer->m_vecPlayerColor, vec3_origin, true );

			delete pDisabler;
#endif

			WeaponSound( WPN_DOUBLE );
			pPlayer->RemoveAmmo( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iAmmoPerShot, m_iSecondaryAmmoType );
		}
	}

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;
	m_flNextSecondaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::PreDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PreDataUpdate( updateType );

	m_bWasChargingBlast = ( m_flBlastTime != 0.0f );
	m_bWasChargingTeleport = ( m_flTeleportTime != 0.0f );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( !m_bWasChargingBlast && m_flBlastTime != 0.0f )
	{
		CreateWarmupEffect( false );
	}
}

void CTFWeaponDisplacer::CreateWarmupEffect( bool bSecondary )
{
	C_BaseEntity *pEntity = GetWeaponForEffect();
	pEntity->ParticleProp()->Create( "mlg_chargeup", PATTACH_POINT_FOLLOW, "muzzle" );
}

#endif
