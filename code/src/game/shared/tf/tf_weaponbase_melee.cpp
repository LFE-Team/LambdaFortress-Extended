//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weaponbase_melee.h"
#include "effect_dispatch_data.h"
#include "tf_gamerules.h"
#include "ai_basenpc_shared.h"

// Server specific.
#if !defined( CLIENT_DLL )
#include "tf_player.h"
#include "tf_gamestats.h"
#include "ilagcompensationmanager.h"
// Client specific.
#else
#include "c_tf_player.h"
#endif

//=============================================================================
//
// TFWeaponBase Melee tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponBaseMelee, DT_TFWeaponBaseMelee )

BEGIN_NETWORK_TABLE( CTFWeaponBaseMelee, DT_TFWeaponBaseMelee )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponBaseMelee )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weaponbase_melee, CTFWeaponBaseMelee );

// Server specific.
#if !defined( CLIENT_DLL ) 
BEGIN_DATADESC( CTFWeaponBaseMelee )
	DEFINE_THINKFUNC( Smack )
END_DATADESC()
#endif

#ifndef CLIENT_DLL
ConVar tf_meleeattackforcescale( "tf_meleeattackforcescale", "80.0", FCVAR_CHEAT | FCVAR_GAMEDLL | FCVAR_DEVELOPMENTONLY );
#endif

ConVar tf_weapon_criticals_melee( "tf_weapon_criticals_melee", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Controls random crits for melee weapons.\n0 - Melee weapons do not randomly crit. \n1 - Melee weapons can randomly crit only if tf_weapon_criticals is also enabled. \n2 - Melee weapons can always randomly crit regardless of the tf_weapon_criticals setting.", true, 0, true, 2 );
extern ConVar tf_weapon_criticals;
extern ConVar tf_debug_criticals;

//=============================================================================
//
// TFWeaponBase Melee functions.
//

// -----------------------------------------------------------------------------
// Purpose: Constructor.
// -----------------------------------------------------------------------------
CTFWeaponBaseMelee::CTFWeaponBaseMelee()
{
	WeaponReset();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
	m_flSmackTime = -1.0f;
	m_bConnected = false;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::Precache()
{
	BaseClass::Precache();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::Spawn()
{
	Precache();

	// Get the weapon information.
	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( GetClassname() );
	Assert( hWpnInfo != GetInvalidWeaponInfoHandle() );
	CTFWeaponInfo *pWeaponInfo = dynamic_cast< CTFWeaponInfo* >( GetFileWeaponInfoFromHandle( hWpnInfo ) );
	Assert( pWeaponInfo && "Failed to get CTFWeaponInfo in melee weapon spawn" );
	m_pWeaponInfo = pWeaponInfo;
	Assert( m_pWeaponInfo );

	// No ammo.
	m_iClip1 = -1;

	BaseClass::Spawn();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBaseMelee::CanHolster( void ) const
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner && pOwner->m_Shared.InCond( TF_COND_CANNOT_SWITCH_FROM_MELEE ) )
		return false;

	return BaseClass::CanHolster();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBaseMelee::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_flSmackTime = -1.0f;
	if ( GetPlayerOwner() )
	{
		GetPlayerOwner()->m_flNextAttack = gpGlobals->curtime + 0.5;
	}
		
	return BaseClass::Holster( pSwitchingTo );
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::PrimaryAttack()
{
	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Set the weapon usage mode - primary, secondary.
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
	m_bConnected = false;

	// Swing the weapon.
	Swing( pPlayer );

#if !defined( CLIENT_DLL ) 
	pPlayer->RemoveInvisibility();
	pPlayer->RemoveDisguise();

	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACritical() );
#endif
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::SecondaryAttack()
{
	// semi-auto behaviour
	if ( m_bInAttack2 )
		return;

	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	pPlayer->DoClassSpecialSkill();

	m_bInAttack2 = true;

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CTFWeaponBaseMelee::Swing( CTFPlayer *pPlayer )
{
	CalcIsAttackCritical();
	CalcIsAttackMiniCritical();

	// Play the melee swing and miss (whoosh) always.
	SendPlayerAnimEvent( pPlayer );

	DoViewModelAnimation();

	// Set next attack times.
	float flFireDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flFireDelay, mult_postfiredelay );

	m_flNextPrimaryAttack = gpGlobals->curtime + flFireDelay;

	SetWeaponIdleTime( m_flNextPrimaryAttack + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeIdleEmpty );
	
	if ( IsCurrentAttackACrit() )
	{
		WeaponSound( BURST );
	}
	else
	{
		WeaponSound( MELEE_MISS );
	}

	QAngle punchAng;
	punchAng.x = random->RandomFloat( 0.2f, 1.2f );
	punchAng.y = random->RandomFloat( -1.2f, -0.2f );
	punchAng.z = 0.0f;
	
	pPlayer->ViewPunch( punchAng ); 

	m_flSmackTime = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSmackDelay;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBaseMelee::DoViewModelAnimation( void )
{
	Activity act = ( m_iWeaponMode == TF_WEAPON_PRIMARY_MODE ) ? ACT_VM_HITCENTER : ACT_VM_SWINGHARD;

	if ( IsCurrentAttackACritical() )
		act = ACT_VM_SWINGHARD;

	if ( IsCurrentAttackAMiniCrit() )
		act = ACT_VM_SWINGHARD;

	SendWeaponAnim( act );
}

//-----------------------------------------------------------------------------
// Purpose: Allow melee weapons to send different anim events
// Input  :  - 
//-----------------------------------------------------------------------------
void CTFWeaponBaseMelee::SendPlayerAnimEvent( CTFPlayer *pPlayer )
{
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
void CTFWeaponBaseMelee::ItemPostFrame()
{
	// Check for smack.
	if ( m_flSmackTime > 0.0f && gpGlobals->curtime > m_flSmackTime )
	{
		Smack();
		m_flSmackTime = -1.0f;
	}

	BaseClass::ItemPostFrame();
}

bool CTFWeaponBaseMelee::DoSwingTrace( trace_t &trace )
{
	// Setup a volume for the melee weapon to be swung - approx size, so all melee behave the same.
	static Vector vecSwingMins( -18, -18, -18 );
	static Vector vecSwingMaxs( 18, 18, 18 );

	float flBoundsMultiplier = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT( flBoundsMultiplier, melee_bounds_multiplier );

	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return false;

	// Setup the swing range.
	int nRangeMultiplier = 48;
	CALL_ATTRIB_HOOK_INT( nRangeMultiplier, melee_range_multiplier );

	Vector vecForward; 
	AngleVectors( pPlayer->EyeAngles(), &vecForward );
	Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
	Vector vecSwingEnd = vecSwingStart + vecForward * nRangeMultiplier;

	int iCleaveAttack = 0;
	CALL_ATTRIB_HOOK_INT( iCleaveAttack, melee_cleave_attack );
	if ( iCleaveAttack != 1 )
	{
		// See if we hit anything.
		UTIL_TraceLine( vecSwingStart, vecSwingEnd, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &trace );
	}

	if ( trace.fraction >= 1.0 )
	{
		UTIL_TraceHull( vecSwingStart, vecSwingEnd, vecSwingMins * flBoundsMultiplier, vecSwingMaxs * flBoundsMultiplier, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &trace );
		if ( trace.fraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = trace.m_pEnt;
			if ( !pHit || pHit->IsBSPModel() )
			{
				// Why duck hull min/max?
				FindHullIntersection( vecSwingStart, trace, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, pPlayer );
			}

			// This is the point on the actual surface (the hull could have hit space)
			vecSwingEnd = trace.endpos;	
		}
	}

	return ( trace.fraction < 1.0f );
}

// -----------------------------------------------------------------------------
// Purpose:
// Note: Think function to delay the impact decal until the animation is finished 
//       playing.
// -----------------------------------------------------------------------------
void CTFWeaponBaseMelee::Smack( void )
{
	trace_t trace;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

#if !defined (CLIENT_DLL)
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( pPlayer, pPlayer->GetCurrentCommand() );
#endif


#ifdef GAME_DLL
	// Do Damage.
	int iCustomDamage = GetDamageCustom();
	float flDamage = GetMeleeDamage( trace.m_pEnt, iCustomDamage );
	int iDmgType = DMG_NEVERGIB | DMG_CLUB;
	if ( IsCurrentAttackACrit() )
	{
		// TODO: Not removing the old critical path yet, but the new custom damage is marking criticals as well for melee now.
		iDmgType |= DMG_CRITICAL;
	}
 	if ( IsCurrentAttackAMiniCrit() )
	{
		iDmgType |= DMG_MINICRITICAL;
	}
#endif

	// We hit, setup the smack.
	if ( DoSwingTrace( trace ) )
	{
		// Hit sound - immediate.
		if( trace.m_pEnt->IsPlayer() || ( trace.m_pEnt->IsNPC() && !trace.m_pEnt->MyNPCPointer()->IsMech() ) )
		{
			WeaponSound( MELEE_HIT );
		}
		else
		{
			WeaponSound( MELEE_HIT_WORLD );
		}

		Vector vecForward; 
		AngleVectors( pPlayer->EyeAngles(), &vecForward );
		Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
		Vector vecSwingEnd = vecSwingStart + vecForward * 48;

#ifdef GAME_DLL
		CTakeDamageInfo info( pPlayer, pPlayer, this, flDamage, iDmgType, iCustomDamage );
		CalculateMeleeDamageForce( &info, vecForward, vecSwingEnd, 1.0f / flDamage * GetForceScale() );

		trace.m_pEnt->DispatchTraceAttack( info, vecForward, &trace ); 

		ApplyMultiDamage();

		OnEntityHit( trace.m_pEnt, &info );
#endif
		// Don't impact trace friendly players or objects
		if ( trace.m_pEnt && trace.m_pEnt->GetTeamNumber() != pPlayer->GetTeamNumber() )
		{
#ifdef CLIENT_DLL
			UTIL_ImpactTrace( &trace, DMG_CLUB );
#endif
			m_bConnected = true;
		}
	}
	else
	{
#ifdef GAME_DLL
		CTakeDamageInfo info( pPlayer, pPlayer, this, flDamage, iDmgType, iCustomDamage );
		int iHitSelfOnMiss = 0;
		CALL_ATTRIB_HOOK_INT( iHitSelfOnMiss, hit_self_on_miss );
		if ( iHitSelfOnMiss )
		{
			ApplyMultiDamage();
			pPlayer->TakeDamage( info );
		}
#endif
	}

#if !defined (CLIENT_DLL)
	lagcompensation->FinishLagCompensation( pPlayer );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CTFWeaponBaseMelee::GetMeleeDamage( CBaseEntity *pTarget, int &iCustomDamage )
{
	float flDamage = (float)m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_nDamage;

	CALL_ATTRIB_HOOK_FLOAT( flDamage, mult_dmg );

	return flDamage;
}

void CTFWeaponBaseMelee::OnEntityHit( CBaseEntity *pEntity, CTakeDamageInfo *pInfo )
{
	CTFPlayer *pOwner = ToTFPlayer( GetPlayerOwner() );
	if ( !pOwner )
		return;

	CTFPlayer *pPlayer = ToTFPlayer( pEntity );
	CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC *>( pEntity );

	int nSpeedBuff = 0;
	CALL_ATTRIB_HOOK_INT( nSpeedBuff, speed_buff_ally );
	if ( nSpeedBuff )
	{
		if ( pEntity->InSameTeam( pOwner ) )
		{
			if ( pEntity->IsPlayer() )
			{
				pPlayer->m_Shared.AddCond( TF_COND_SPEED_BOOST, 2 );
				pOwner->m_Shared.AddCond( TF_COND_SPEED_BOOST, 2 );
			}
			else if ( pEntity->IsNPC() )
			{
				pNPC->AddCond( TF_COND_SPEED_BOOST, 2 );
				pOwner->m_Shared.AddCond( TF_COND_SPEED_BOOST, 2 );
			}
		}
	}
	
	int nMiniCritAndConsumesBurning = 0;
	CALL_ATTRIB_HOOK_INT( nMiniCritAndConsumesBurning, attack_minicrits_and_consumes_burning );
	if ( nMiniCritAndConsumesBurning )
	{
		if ( pEntity->IsPlayer() && pPlayer->m_Shared.InCond( TF_COND_BURNING ) )
		{
			pPlayer->m_Shared.RemoveCond( TF_COND_BURNING );
			pOwner->m_Shared.AddCond( TF_COND_SPEED_BOOST, 4 );
		}
		else if ( pEntity->IsNPC() && pNPC->InCond( TF_COND_BURNING ) )
		{
			pNPC->RemoveCond( TF_COND_BURNING );
			pOwner->m_Shared.AddCond( TF_COND_SPEED_BOOST, 4 );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBaseMelee::CalcIsAttackCriticalHelper( void )
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return false;

	int nCvarValue = tf_weapon_criticals_melee.GetInt();

	if ( nCvarValue == 0 )
		return false;

	if ( nCvarValue == 1 && !tf_weapon_criticals.GetBool() )
		return false;

	float flPlayerCritMult = pPlayer->GetCritMult();

	float flCritChance = TF_DAMAGE_CRIT_CHANCE_MELEE * flPlayerCritMult;
	CALL_ATTRIB_HOOK_FLOAT( flCritChance, mult_crit_chance );

	// If the chance is 0, just bail.
	if ( flCritChance == 0.0f )
		return false;

#ifdef GAME_DLL
	if ( tf_debug_criticals.GetBool() )
	{
		Msg( "Rolling crit: %.02f%% chance... ", flCritChance * 100.0f );
	}
#endif

	bool bSuccess = ( RandomInt( 0, WEAPON_RANDOM_RANGE - 1 ) <= flCritChance * WEAPON_RANDOM_RANGE );

#ifdef GAME_DLL
	if ( tf_debug_criticals.GetBool() )
	{
		Msg( "%s\n", bSuccess ? "SUCCESS" : "FAILURE" );
	}
#endif

	return bSuccess;
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWeaponBaseMelee::GetForceScale( void )
{
	return tf_meleeattackforcescale.GetFloat();
}
#endif