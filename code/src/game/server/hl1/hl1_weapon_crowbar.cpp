//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Crowbar - an old favorite
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hl1mp_basecombatweapon_shared.h"

#ifdef CLIENT_DLL
#include "c_baseplayer.h"
#else
#include "player.h"
#include "soundent.h"
#endif

#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"

#include "vstdlib/random.h"

extern ConVar sk_plr_dmg_crowbar;

#define	CROWBAR_RANGE		64.0f
#define	CROWBAR_REFIRE_MISS	0.5f
#define	CROWBAR_REFIRE_HIT	0.25f


#ifdef CLIENT_DLL
#define CWeaponHL1Crowbar C_WeaponHL1Crowbar
#endif

//-----------------------------------------------------------------------------
// CWeaponHL1Crowbar
//-----------------------------------------------------------------------------

class CWeaponHL1Crowbar : public CBaseHL1MPCombatWeapon
{
	DECLARE_CLASS( CWeaponHL1Crowbar, CBaseHL1MPCombatWeapon );
public:
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	CWeaponHL1Crowbar();

	void			Precache( void );
	virtual void	ItemPostFrame( void );
	void			PrimaryAttack( void );

public:
	trace_t		m_traceHit;
	Activity	m_nHitActivity;

private:
	virtual void		Swing( void );
	virtual	void		Hit( void );
	virtual	void		ImpactEffect( void );
	void		ImpactSound( CBaseEntity *pHitEntity );
	virtual Activity	ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner );

public:

};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponHL1Crowbar, DT_WeaponHL1Crowbar );

BEGIN_NETWORK_TABLE( CWeaponHL1Crowbar, DT_WeaponHL1Crowbar )
/// what
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponHL1Crowbar )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_crowbar_hl1, CWeaponHL1Crowbar );
PRECACHE_WEAPON_REGISTER( weapon_crowbar_hl1 );

#ifndef CLIENT_DLL
BEGIN_DATADESC( CWeaponHL1Crowbar )

	// DEFINE_FIELD( m_trLineHit, trace_t ),
	// DEFINE_FIELD( m_trHullHit, trace_t ),
	// DEFINE_FIELD( m_nHitActivity, FIELD_INTEGER ),
	// DEFINE_FIELD( m_traceHit, trace_t ),

	// Class CWeaponHL1Crowbar:
	// DEFINE_FIELD( m_nHitActivity, FIELD_INTEGER ),

	// Function Pointers
	DEFINE_FUNCTION( Hit ),

END_DATADESC()
#endif

#define BLUDGEON_HULL_DIM		16

static const Vector g_bludgeonMins(-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM);
static const Vector g_bludgeonMaxs(BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponHL1Crowbar::CWeaponHL1Crowbar()
{
	m_bFiresUnderwater = true;
}

//-----------------------------------------------------------------------------
// Purpose: Precache the weapon
//-----------------------------------------------------------------------------
void CWeaponHL1Crowbar::Precache( void )
{
	//Call base class first
	BaseClass::Precache();
}

//------------------------------------------------------------------------------
// Purpose : Update weapon
//------------------------------------------------------------------------------
void CWeaponHL1Crowbar::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	if ( (pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime) )
	{
		PrimaryAttack();
	} 
	else 
	{
		WeaponIdle();
		return;
	}
}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CWeaponHL1Crowbar::PrimaryAttack()
{
	Swing();
}


//------------------------------------------------------------------------------
// Purpose: Implement impact function
//------------------------------------------------------------------------------
void CWeaponHL1Crowbar::Hit( void )
{
	//Make sound for the AI
#ifndef CLIENT_DLL

	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	CSoundEnt::InsertSound( SOUND_BULLET_IMPACT, m_traceHit.endpos, 400, 0.2f, pPlayer );

	CBaseEntity	*pHitEntity = m_traceHit.m_pEnt;

	//Apply damage to a hit target
	if ( pHitEntity != NULL )
	{
		Vector hitDirection;
		pPlayer->EyeVectors( &hitDirection, NULL, NULL );
		VectorNormalize( hitDirection );

		ClearMultiDamage();
		CTakeDamageInfo info( GetOwner(), GetOwner(), sk_plr_dmg_crowbar.GetFloat(), DMG_CLUB );
		CalculateMeleeDamageForce( &info, hitDirection, m_traceHit.endpos );
		pHitEntity->DispatchTraceAttack( info, hitDirection, &m_traceHit ); 
		ApplyMultiDamage();

		// Now hit all triggers along the ray that... 
		TraceAttackToTriggers( CTakeDamageInfo( GetOwner(), GetOwner(), sk_plr_dmg_crowbar.GetFloat(), DMG_CLUB ), m_traceHit.startpos, m_traceHit.endpos, hitDirection );

		//Play an impact sound	
		ImpactSound( pHitEntity );
	}
#endif

	//Apply an impact effect
	ImpactEffect();
}

//-----------------------------------------------------------------------------
// Purpose: Play the impact sound
// Input  : pHitEntity - entity that we hit
// assumes pHitEntity is not null
//-----------------------------------------------------------------------------
void CWeaponHL1Crowbar::ImpactSound( CBaseEntity *pHitEntity )
{
	bool bIsWorld = ( pHitEntity->entindex() == 0 );

	if( bIsWorld )
	{
		WeaponSound( MELEE_HIT_WORLD );
	}
	else 
	{
		WeaponSound( MELEE_HIT );
	}
}

Activity CWeaponHL1Crowbar::ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner )
{
	int			i, j, k;
	float		distance;
	const float	*minmaxs[2] = {mins.Base(), maxs.Base()};
	trace_t		tmpTrace;
	Vector		vecHullEnd = hitTrace.endpos;
	Vector		vecEnd;

	distance = 1e6f;
	Vector vecSrc = hitTrace.startpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
	if ( tmpTrace.fraction == 1.0 )
	{
		for ( i = 0; i < 2; i++ )
		{
			for ( j = 0; j < 2; j++ )
			{
				for ( k = 0; k < 2; k++ )
				{
					vecEnd.x = vecHullEnd.x + minmaxs[i][0];
					vecEnd.y = vecHullEnd.y + minmaxs[j][1];
					vecEnd.z = vecHullEnd.z + minmaxs[k][2];

					UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
					if ( tmpTrace.fraction < 1.0 )
					{
						float thisDistance = (tmpTrace.endpos - vecSrc).Length();
						if ( thisDistance < distance )
						{
							hitTrace = tmpTrace;
							distance = thisDistance;
						}
					}
				}
			}
		}
	}
	else
	{
		hitTrace = tmpTrace;
	}


	return ACT_VM_HITCENTER;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHL1Crowbar::ImpactEffect( void )
{
	//FIXME: need new decals
	UTIL_ImpactTrace( &m_traceHit, DMG_CLUB );
}

//------------------------------------------------------------------------------
// Purpose : Starts the swing of the weapon and determines the animation
//------------------------------------------------------------------------------
void CWeaponHL1Crowbar::Swing( void )
{
	// Try a ray
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( !pOwner )
		return;

	Vector swingStart = pOwner->Weapon_ShootPosition( );
	Vector forward;

	pOwner->EyeVectors( &forward, NULL, NULL );

	Vector swingEnd = swingStart + forward * CROWBAR_RANGE;

	UTIL_TraceLine( swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &m_traceHit );
	m_nHitActivity = ACT_VM_HITCENTER;

	if ( m_traceHit.fraction == 1.0 )
	{
		float bludgeonHullRadius = 1.732f * BLUDGEON_HULL_DIM;  // hull is +/- 16, so use cuberoot of 2 to determine how big the hull is from center to the corner point

		// Back off by hull "radius"
		swingEnd -= forward * bludgeonHullRadius;

		UTIL_TraceHull( swingStart, swingEnd, g_bludgeonMins, g_bludgeonMaxs, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &m_traceHit );
		if ( m_traceHit.fraction < 1.0 )
		{
			m_nHitActivity = ChooseIntersectionPointAndActivity( m_traceHit, g_bludgeonMins, g_bludgeonMaxs, pOwner );
		}
	}


	// -------------------------
	//	Miss
	// -------------------------
	if ( m_traceHit.fraction == 1.0f )
	{
		m_nHitActivity = ACT_VM_MISSCENTER;

		//Play swing sound
		WeaponSound( SINGLE );

		//Setup our next attack times
		m_flNextPrimaryAttack = gpGlobals->curtime + CROWBAR_REFIRE_MISS;
	}
	else
	{
		Hit();

		//Setup our next attack times
		m_flNextPrimaryAttack = gpGlobals->curtime + CROWBAR_REFIRE_HIT;
	}

	//Send the anim
	SendWeaponAnim( m_nHitActivity );
	pOwner->SetAnimation( PLAYER_ATTACK1 );
}
