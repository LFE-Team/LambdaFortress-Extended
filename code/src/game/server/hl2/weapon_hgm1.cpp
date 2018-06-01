//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ====================================================
//
// Purpose: M60
//		TODO: Create AI danger sounds at bullet impact points after sustained fire (Make AI react to suppressing fire)
//======================================================================================================================

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "NPCevent.h"
#include "basecombatcharacter.h"
#include "AI_BaseNPC.h"

ConVar	sk_hmg1_firerate ("sk_hmg1_firerate", "0.12" );


class CWeaponHMG1 : public CHLMachineGun
{
public:
	DECLARE_CLASS( CWeaponHMG1, CHLMachineGun );

	DECLARE_SERVERCLASS();

	CWeaponHMG1();
	
	void	Precache( void );
	bool	Deploy( void );

	
	void	AddViewKick( void );


	int		GetMinBurst() { return 50; } //min-max burst for NPC usage
	int		GetMaxBurst() { return 100; }

	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	float    GetFireRate( void ) { return sk_hmg1_firerate.GetFloat(); }


	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_15DEGREES; //originally 15 degrees
		return cone;
	}

	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
	{
		switch( pEvent->event )
		{
			case EVENT_WEAPON_AR2:
			{
				Vector vecShootOrigin, vecShootDir;
				vecShootOrigin = pOperator->Weapon_ShootPosition( );

				CAI_BaseNPC *npc = pOperator->MyNPCPointer();
				Vector vecSpread;
				if (npc)
				{
					vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );
					vecSpread = VECTOR_CONE_PRECALCULATED;
				}
				else
				{
					AngleVectors( pOperator->GetLocalAngles(), &vecShootDir );
					vecSpread = GetBulletSpread();
				}
				WeaponSound(SINGLE_NPC);
				pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 10 ); //last number is 2 by default. I have no idea what changing it to 10 will do. -Nullen
			    pOperator->DoMuzzleFlash();
			}
			break;
			default:
				CBaseCombatWeapon::Operator_HandleAnimEvent( pEvent, pOperator );
				break;
		}
	}
	DECLARE_ACTTABLE();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponHMG1, DT_WeaponHMG1)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_hmg1, CWeaponHMG1 );

acttable_t	CWeaponHMG1::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_AR2,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,				true },		
	{ ACT_IDLE,						ACT_IDLE_SMG1,					true },		
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			true },		

	{ ACT_WALK,						ACT_WALK_RIFLE,					true },

// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG1_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG1_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG1,			false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_RIFLE_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_RIFLE,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_RIFLE_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_RIFLE,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG1_RELAXED,			false },//never aims	
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_RIFLE_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG1,			false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_RIFLE_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_RIFLE,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_RIFLE_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_RIFLE,				false },//always aims
//End readiness activities

	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_RIFLE,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_AR2,	false },
	{ ACT_COVER_LOW,				ACT_COVER_SMG1_LOW,				false },		
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_AR2_LOW,			false },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG1_LOW,		true },		
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,		true },
};


IMPLEMENT_ACTTABLE(CWeaponHMG1);

//=========================================================
CWeaponHMG1::CWeaponHMG1( )
{
}

void CWeaponHMG1::Precache( void )
{
	BaseClass::Precache();
}

bool CWeaponHMG1::Deploy( void )
{
	//CBaseCombatCharacter *pOwner  = m_hOwner;
	return BaseClass::Deploy();
}

void CWeaponHMG1::AddViewKick( void )
{
	#define	EASY_DAMPEN			1.0f
	#define	MAX_VERTICAL_KICK	25.0f	//Degrees
	#define	SLIDE_LIMIT			5.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
		return;

	/*QAngle viewPunch;
	viewPunch.x = (random->RandomFloat(-2.5f, 0.5f ) );
	viewPunch.y = (random->RandomFloat( -1.1f, 1.1f ) );
	viewPunch.z = 0;
	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		viewPunch *= 0.25;
	}
	pPlayer->ViewPunch( viewPunch );*/
	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}
