
//=========================================================
// monster template
//=========================================================


//Stacker/Nullen - TODO: Make them go back to following the player after combat if player made him follow before initiating combat

//Stacker/Nullen - TODO #2: Give citizen medics the ability to heal them

//Stacker/Nullen - TODO #3: Fix bug where npc_maker'd conscripts appear as ERROR signs if sk_conscript_model is set to "Random"
//--------------------------Current workaround: Just use template npc_conscripts and npc_template_makers

//Stacker/Nullen - TODO #4: AI personality system ( Cautious, Balanced, & Aggressive) Affects how likely they are to take cover, maneuver, & attack
//--------------------------IMPLEMENTED AND FULLY WORKING!


//Stacker/Nullen - TODO #5: Fix this god awful scheduling problem that just started happening 
//-------------------------FIXED! Looks like NPCs with this outdated format don't like custom schedules being overriden with other custom ones
//Note for the future: CUSTOM SCHEDULES BEING TRANSLATED TO OTHER CUSTOM SCHEDULES IS A BIG FUCKING NO-NO!


//Stacker/Nullen - TODO #6: Implement gender-specific voice lines/sentences

#include	"cbase.h"

//#if 0

#include	"npc_talker.h"
#include	"ai_schedule.h"
#include	"scripted.h"
#include	"basecombatweapon.h"
#include	"soundent.h"
#include	"NPCEvent.h"
#include	"props.h"
#include	"npc_conscript.h"
#include	"activitylist.h"

#include "AI_Interactions.h"
#include "ai_navigator.h"
#include "ai_motor.h"
#include "ai_squadslot.h"
#include "ai_squad.h"
#include "ai_route.h"
#include "ai_tacticalservices.h"
#include	"AI_Hull.h"
#include	"AI_Node.h"
#include	"AI_Network.h"
#include	"ai_hint.h"

#include "basegrenade_shared.h"
#include "grenade_frag.h"


#include "IEffects.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"

ConVar	sk_conscript_health( "sk_conscript_health","100");
ConVar	sk_conscript_model( "sk_conscript_model", "random" );
ConVar	sk_conscript_hostile_model("sk_conscript_hostile_model", "random" ); //Not actually functional yet
ConVar	sk_conscript_ff_retaliation( "sk_conscript_ff_retaliation", "0"); //FULLY WORKING!
ConVar	sk_conscript_personality_colors( "sk_conscript_personality_colors", "0"); //FULLY WORKING!


#define CONSCRIPT_MAD 		"CONSCRIPT_MAD"
#define CONSCRIPT_SHOT 		"CONSCRIPT_SHOT"
#define CONSCRIPT_KILL 		"CONSCRIPT_KILL"
#define CONSCRIPT_OUT_AMMO 	"CONSCRIPT_OUT_AMMO"
#define CONSCRIPT_ATTACK 	"CONSCRIPT_ATTACK"
#define CONSCRIPT_LINE_FIRE "CONSCRIPT_LINE_FIRE"
#define CONSCRIPT_POK 		"CONSCRIPT_POK"

#define CONSCRIPT_PAIN1		"CONSCRIPT_PAIN1"
#define CONSCRIPT_PAIN2		"CONSCRIPT_PAIN2"
#define CONSCRIPT_PAIN3		"CONSCRIPT_PAIN3"

#define CONSCRIPT_DIE1		"CONSCRIPT_DIE1"
#define CONSCRIPT_DIE2		"CONSCRIPT_DIE2"
#define CONSCRIPT_DIE3		"CONSCRIPT_DIE3"

#define CONSCRIPT_SKIN_DEFAULT		0
#define CONSCRIPT_SKIN_HOSTILE		1
#define CONSCRIPT_SKIN_WASTELAND		2

#define CONSCRIPT_PERSONALITY_BALANCED		 10
#define CONSCRIPT_PERSONALITY_CAUTIOUS		 20
#define CONSCRIPT_PERSONALITY_AGGRESSIVE     30

//=========================================================
// Combine activities
//=========================================================
int	ACT_CONSCRIPT_AIM;
int	ACT_IDLE_AR1;
int	ACT_RUN_AR1;
int	ACT_IDLE_AR2;
int	ACT_IDLE_ANGRY_AR1;
int	ACT_CONSCRIPT_IDLE_ANGRY_SMG1;
int	ACT_CONSCRIPT_SHOOT_AR1;
int	ACT_CONSCRIPT_SHOOT_SNIPER_RIFLE;
int	ACT_CONSCRIPT_SHOOT_SMG1;
int	ACT_CONSCRIPT_RUN_SMG1;
int	ACT_CONSCRIPT_WALK_SMG1;
int	ACT_CONSCRIPT_WALK_PISTOL;
int	ACT_CONSCRIPT_WALK_AR1;
int	ACT_CONSCRIPT_IDLE_SMG1;
int ACT_CONSCRIPT_IDLE_RPG;
int ACT_CONSCRIPT_LOAD_RPG;
int ACT_CONSCRIPT_IDLE_SHOTGUN;
int ACT_CONSCRIPT_WALK_SHOTGUN;
int ACT_CONSCRIPT_RUN_SHOTGUN;
int ACT_CONSCRIPT_RELOAD_SMG1;

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
// first flag is barney dying for scripted sequences?
#define		CONSCRIPT_AE_RELOAD		( 1 )
#define		CONSCRIPT_AE_DRAW		( 2 )
#define		CONSCRIPT_AE_SHOOT		( 3 )
#define		CONSCRIPT_AE_HOLSTER	( 4 )
#define		CONSCRIPT_AE_SWATITEM	( 11 )

#define		CONSCRIPT_BODY_GUNHOLSTERED	0
#define		CONSCRIPT_BODY_GUNDRAWN		1
#define		CONSCRIPT_BODY_GUNGONE		2

//-----------------------------------------------------------------------------
// Purpose: Initialize the custom schedules
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Conscript::InitCustomSchedules(void) 
{
	INIT_CUSTOM_AI(CNPC_Conscript);

	ADD_CUSTOM_TASK(CNPC_Conscript,	TASK_CONSCRIPT_CROUCH);

	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FOLLOW);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_DRAW);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FACE_TARGET);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_STAND);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_AIM);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FIRE_RPG);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_ESTABLISH_RPG_LINE_OF_FIRE);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_SUPPRESSINGFIRE);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_HIT);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_PULL);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_CHOMP);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_CHEW);
	ADD_CUSTOM_SCHEDULE(CNPC_Conscript, SCHED_CONSCRIPT_CAUTIOUS_TAKECOVER);

	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_AIM);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_IDLE_AR1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_IDLE_AR2);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_IDLE_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_RUN_AR1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_SHOOT_AR1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_SHOOT_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_SHOOT_SNIPER_RIFLE);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_RUN_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_WALK_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_WALK_PISTOL);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_WALK_AR1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_LOAD_RPG);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_RELOAD_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_IDLE_ANGRY_AR1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_IDLE_ANGRY_SMG1);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_IDLE_SHOTGUN);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_WALK_SHOTGUN);
	ADD_CUSTOM_ACTIVITY(CNPC_Combine,	ACT_CONSCRIPT_RUN_SHOTGUN);

	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FOLLOW);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_DRAW);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FACE_TARGET);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_STAND);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_AIM);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_FIRE_RPG);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_ESTABLISH_RPG_LINE_OF_FIRE);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_SUPPRESSINGFIRE);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_HIT);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_PULL);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_CHOMP);
	AI_LOAD_SCHEDULE(CNPC_Conscript,	SCHED_CONSCRIPT_BARNACLE_CHEW);
}

LINK_ENTITY_TO_CLASS( npc_conscript, CNPC_Conscript );
LINK_ENTITY_TO_CLASS( npc_conscriptred, CNPC_Conscript ); //LF:E 
//LINK_ENTITY_TO_CLASS( npc_conscript_wasteland, CNPC_Conscript ); 
IMPLEMENT_CUSTOM_AI( npc_conscript, CNPC_Conscript );

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CNPC_Conscript )

	DEFINE_FIELD( m_fGunDrawn,			FIELD_BOOLEAN ),
	DEFINE_FIELD( m_painTime,			FIELD_TIME ),
	DEFINE_FIELD( m_checkAttackTime,	FIELD_TIME ),
	DEFINE_FIELD( m_nextLineFireTime,	FIELD_TIME ),
	DEFINE_FIELD( m_lastAttackCheck,	FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bInBarnacleMouth,	FIELD_BOOLEAN ),
	DEFINE_FIELD( m_hPhysicsEnt, FIELD_EHANDLE ),
	DEFINE_KEYFIELD(m_iPersonality, FIELD_INTEGER, "AIPersonality"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Conscript::RunTask( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_RANGE_ATTACK1:
		if (GetEnemy() != NULL && (GetEnemy()->IsPlayer()))
		{
			m_flPlaybackRate = 1.0;
		}
		BaseClass::RunTask( pTask );
		break;
	default:
		BaseClass::RunTask( pTask );
		break;
	}
}

bool CNPC_Conscript::CreateBehaviors()
{
		AddBehavior( &m_ActBusyBehavior ); //Doesn't work, there's probably more I need to add here -Stacker

		return BaseClass::CreateBehaviors();
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
Activity CNPC_Conscript::NPC_TranslateActivity( Activity eNewActivity )  //MESSY, MESSY, MESSY! LOOK AWAY IF YOU KNOW WHAT'S GOOD FOR YOU!!!! -Stacker
{

		if ( m_NPCState == NPC_STATE_COMBAT && eNewActivity == ACT_IDLE )
		{
			return ACT_IDLE_ANGRY;
		}

		//Override anims for new ones
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_CONSCRIPT_IDLE_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_IDLE_ANGRY ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_CONSCRIPT_RUN_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_RANGE_ATTACK_SMG1 ) { return (Activity)ACT_CONSCRIPT_SHOOT_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_RELOAD ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_RELOAD_SMG1 ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg1" ) && eNewActivity == ACT_IDLE_ANGRY_SMG1 ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }

		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_CONSCRIPT_IDLE_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_CONSCRIPT_RUN_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_RANGE_ATTACK_SMG1 ) { return (Activity)ACT_CONSCRIPT_SHOOT_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_RELOAD ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_RELOAD_SMG1 ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_IDLE_ANGRY ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }
		if( Weapon_OwnsThisType( "weapon_smg2" ) && eNewActivity == ACT_IDLE_ANGRY_SMG1 ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }
		
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_CONSCRIPT_IDLE_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_CONSCRIPT_RUN_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_RANGE_ATTACK_SMG1 ) { return (Activity)ACT_CONSCRIPT_SHOOT_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_RELOAD ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_RELOAD_SMG1 ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_IDLE_ANGRY ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }
		if( Weapon_OwnsThisType( "weapon_tommygun" ) && eNewActivity == ACT_IDLE_ANGRY_SMG1 ) { return (Activity)ACT_CONSCRIPT_IDLE_ANGRY_SMG1; }

		if( Weapon_OwnsThisType( "weapon_ar2" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_IDLE_AR2; }

		if( Weapon_OwnsThisType( "weapon_oicw" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_IDLE_AR2; }

		if( Weapon_OwnsThisType( "weapon_shotgun" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_CONSCRIPT_IDLE_SHOTGUN; }
		if( Weapon_OwnsThisType( "weapon_shotgun" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_SHOTGUN; }
		if( Weapon_OwnsThisType( "weapon_shotgun" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_CONSCRIPT_RUN_SHOTGUN; }

		if( Weapon_OwnsThisType( "weapon_dbarrel" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_CONSCRIPT_IDLE_SHOTGUN; }
		if( Weapon_OwnsThisType( "weapon_dbarrel" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_SHOTGUN; }
		if( Weapon_OwnsThisType( "weapon_dbarrel" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_CONSCRIPT_RUN_SHOTGUN; }
		if( Weapon_OwnsThisType( "weapon_dbarrel" ) && eNewActivity == ACT_RELOAD_SHOTGUN ) { return (Activity)ACT_CONSCRIPT_RELOAD_SMG1; }

		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_IDLE_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_RUN_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_RANGE_ATTACK_AR2 ) { return (Activity)ACT_CONSCRIPT_SHOOT_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_RANGE_ATTACK1 ) { return (Activity)ACT_CONSCRIPT_SHOOT_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_IDLE_ANGRY ) { return (Activity)ACT_IDLE_ANGRY_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_IDLE_ANGRY_SMG1 ) { return (Activity)ACT_IDLE_ANGRY_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_AR1; }
		if( Weapon_OwnsThisType( "weapon_ar1" ) && eNewActivity == ACT_RELOAD_SMG1 ) { return (Activity)ACT_RELOAD; }

		if( Weapon_OwnsThisType( "weapon_sniperrifle" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_IDLE_AR1; }
		if( Weapon_OwnsThisType( "weapon_sniperrifle" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_AR1; }
		if( Weapon_OwnsThisType( "weapon_sniperrifle" ) && eNewActivity == ACT_RANGE_ATTACK_AR2 ) { return (Activity)ACT_CONSCRIPT_SHOOT_SNIPER_RIFLE; }

		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_RANGE_ATTACK1 ) { return (Activity)ACT_CONSCRIPT_SHOOT_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_RANGE_ATTACK_AR2 ) { return (Activity)ACT_CONSCRIPT_SHOOT_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_IDLE_ANGRY ) { return (Activity)ACT_IDLE_ANGRY_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_IDLE_ANGRY_SMG1 ) { return (Activity)ACT_IDLE_ANGRY_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_IDLE ) { return (Activity)ACT_IDLE_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_RUN_AR1; }
		if( Weapon_OwnsThisType( "weapon_hmg1" ) && eNewActivity == ACT_WALK ) { return (Activity)ACT_CONSCRIPT_WALK_AR1; }

		if( Weapon_OwnsThisType( "weapon_annabelle" ) && eNewActivity == ACT_RANGE_ATTACK_SHOTGUN ) { return (Activity)ACT_CONSCRIPT_SHOOT_SNIPER_RIFLE; }

		if( Weapon_OwnsThisType( "weapon_garand" ) && eNewActivity == ACT_RANGE_ATTACK_SHOTGUN ) { return (Activity)ACT_CONSCRIPT_SHOOT_SNIPER_RIFLE; }
		if( Weapon_OwnsThisType( "weapon_garand" ) && eNewActivity == ACT_RUN ) { return (Activity)ACT_RUN_AR1; }

	return eNewActivity;

	return BaseClass::NPC_TranslateActivity( eNewActivity );
}


//=========================================================
// GetSoundInterests - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CNPC_Conscript::GetSoundInterests ( void) 
{
	return	SOUND_WORLD		|
			SOUND_COMBAT	|
			SOUND_CARCASS	|
			SOUND_MEAT		|
			SOUND_GARBAGE	|
			SOUND_DANGER	|
			SOUND_BUGBAIT	|
			SOUND_PLAYER;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
Class_T	CNPC_Conscript::Classify(void)
{
	if (FClassnameIs(this, "npc_conscript"))
	{
		return	CLASS_COMBINE; //LF:E
	}
	else (FClassnameIs(this, "npc_conscriptred")); //LF:E 
	{
		return	CLASS_PLAYER_ALLY;
	}
}

	/*if (FClassnameIs(this, "npc_conscript_wasteland")) //For the Coast chapters & Airex Warzone
	{
		return	CLASS_TEAM1; //Only hostile temporarily, will change to CLASS_CONSCRIPT
		//10-8-2018: NOTE TO DOWNLOADERS: You really, really, really want to change this if you don't have a CLASS_TEAM1 defined in gamerules
	}*/  

//=========================================================
// ALertSound - barney says "Freeze!"
//=========================================================
void CNPC_Conscript::AlertSound( void )
{
	if ( GetEnemy() != NULL )
	{
		if ( IsOkToCombatSpeak() )
		{
			Speak( CONSCRIPT_ATTACK );
		}
	}

}
//=========================================================
// MaxYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
float CNPC_Conscript::MaxYawSpeed ( void )
{
	switch ( GetActivity() )
	{
	case ACT_IDLE:		
		return 70;
		break;
	case ACT_WALK:
		return 70;
		break;
	case ACT_RUN:
		return 90;
		break;
	default:
		return 70;
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Set proper blend for shooting
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Conscript::ConscriptFirePistol ( void )
{
	Vector vecShootOrigin;

	vecShootOrigin = GetLocalOrigin() + Vector( 0, 0, 55 );
	Vector vecShootDir = GetShootEnemyDir( vecShootOrigin );

	QAngle angDir;
	VectorAngles( vecShootDir, angDir );
	SetPoseParameter( 0, angDir.x );
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================
void CNPC_Conscript::HandleAnimEvent( animevent_t *pEvent )
{

	switch( pEvent->event )
	{
	case CONSCRIPT_AE_RELOAD:
		
		// We never actually run out of ammo, just need to refill the clip
		if (GetActiveWeapon())
		{
			GetActiveWeapon()->WeaponSound( RELOAD_NPC );
			GetActiveWeapon()->m_iClip1 = GetActiveWeapon()->GetMaxClip1(); 
			GetActiveWeapon()->m_iClip2 = GetActiveWeapon()->GetMaxClip2(); 
		}
		ClearCondition(COND_NO_PRIMARY_AMMO);
		ClearCondition(COND_NO_SECONDARY_AMMO);
		break;

	case CONSCRIPT_AE_SHOOT:
		ConscriptFirePistol();
		break;

	case CONSCRIPT_AE_DRAW:
		// barney's bodygroup switches here so he can pull gun from holster
		m_nBody = CONSCRIPT_BODY_GUNDRAWN;
		m_fGunDrawn = true;
		break;

	case CONSCRIPT_AE_HOLSTER:
		// change bodygroup to replace gun in holster
		m_nBody = CONSCRIPT_BODY_GUNHOLSTERED;
		m_fGunDrawn = false;
		break;



	default:
		BaseClass::HandleAnimEvent( pEvent );
	}
}

//=========================================================
// Spawn
//=========================================================
void CNPC_Conscript::Spawn()
{
	Precache( );
	SetModel( STRING( GetModelName() ));


	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );
	SetBloodColor( BLOOD_COLOR_RED );
	m_iHealth			= sk_conscript_health.GetFloat();
	SetViewOffset( Vector ( 0, 0, 50 ) );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_NPCState			= NPC_STATE_NONE;

	m_nBody			= 0; // gun in holster
	m_fGunDrawn			= false;
	m_bInBarnacleMouth	= false;


	m_nextLineFireTime	= 0;

	m_HackedGunPos		= Vector ( 0, 0, 55 );

	CapabilitiesAdd( bits_CAP_TURN_HEAD | bits_CAP_MOVE_GROUND | bits_CAP_MOVE_JUMP | bits_CAP_MOVE_CLIMB | bits_CAP_NO_HIT_PLAYER | bits_CAP_FRIENDLY_DMG_IMMUNE | bits_CAP_ANIMATEDFACE );
	CapabilitiesAdd	( bits_CAP_USE_WEAPONS | bits_CAP_SQUAD | bits_CAP_DOORS_GROUP );
	CapabilitiesAdd	( bits_CAP_DUCK );			// In reloading and cover
	NPCInit();
	SetUse( &CNPCSimpleTalker::FollowerUse );

	//Had to do this awful hack because a model's eyes don't scale with it if scaled via .qc. Thanks a lot, Failve! -Stacker
	const char *pModelName = STRING( GetModelName() ); 	
	if( !Q_stricmp( pModelName, "models/conscripts/female_01.mdl" ) ) { SetModelScale (0.97); }
	if( !Q_stricmp( pModelName, "models/conscripts/female_02.mdl" ) ) { SetModelScale (0.97); }
	if( !Q_stricmp( pModelName, "models/conscripts/female_03.mdl" ) ) { SetModelScale (0.97); }
	if( !Q_stricmp( pModelName, "models/conscripts/female_04.mdl" ) ) { SetModelScale (0.97); }
	if( !Q_stricmp( pModelName, "models/conscripts/female_06.mdl" ) ) { SetModelScale (0.97); }
	if( !Q_stricmp( pModelName, "models/conscripts/female_07.mdl" ) ) { SetModelScale (0.97); }




	string_t iszModelName = BaseClass::GetModelName();

	if (!Q_strnicmp(STRING(iszModelName), "models/conscripts/female", 13))
	{
		m_bIsFemale = true; //Use female voice lines if set to true (Not yet implemented)
	}
	else
	{
		m_bIsFemale = false;
	}




	if (FClassnameIs(this, "npc_conscriptred")) //LF:E 
	{
	    SetModel( "models/conscript_red.mdl" );
	}

	/*if (FClassnameIs(this, "npc_conscript_wasteland")) 
	{
		m_nSkin = CONSCRIPT_SKIN_WASTELAND; //Use wasteland-themed outfit
	}*/ 

	if (m_iPersonality == 0)
	{
		int pmin = 1;
		int pmax = 3;
		double scaled = (double)rand()/RAND_MAX;
		int r = (pmax - pmin +1)*scaled + pmin;

		if (r == 1)	{ m_iPersonality = CONSCRIPT_PERSONALITY_BALANCED; }
		if (r == 2)	{ m_iPersonality = CONSCRIPT_PERSONALITY_CAUTIOUS; }
		if (r == 3)	{ m_iPersonality = CONSCRIPT_PERSONALITY_AGGRESSIVE; }
	}

if ( sk_conscript_personality_colors.GetInt() != 0 )
{
	if (m_iPersonality == CONSCRIPT_PERSONALITY_BALANCED )
	{
		SetRenderColor( 255, 255, 0, 255);
	}

	if (m_iPersonality == CONSCRIPT_PERSONALITY_CAUTIOUS )
	{
		SetRenderColor( 0, 0, 255, 255);
	}

	if (m_iPersonality == CONSCRIPT_PERSONALITY_AGGRESSIVE )
	{
		SetRenderColor( 255, 0, 0, 255);
	}
}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CNPC_Conscript::Precache()
{
	engine->PrecacheModel("models/conscript.mdl");
	engine->PrecacheModel("models/conscript_red.mdl"); //LF:E
	engine->PrecacheModel( sk_conscript_model.GetString() );
	engine->PrecacheModel("models/conscripts/male_01.mdl");
	engine->PrecacheModel("models/conscripts/male_02.mdl");
	engine->PrecacheModel("models/conscripts/male_03.mdl");
	engine->PrecacheModel("models/conscripts/male_04.mdl");
	engine->PrecacheModel("models/conscripts/male_05.mdl");
	engine->PrecacheModel("models/conscripts/male_06.mdl");
	engine->PrecacheModel("models/conscripts/male_07.mdl");
	engine->PrecacheModel("models/conscripts/male_08.mdl");
	engine->PrecacheModel("models/conscripts/male_09.mdl");
	engine->PrecacheModel("models/conscripts/female_01.mdl");
	engine->PrecacheModel("models/conscripts/female_02.mdl");
	engine->PrecacheModel("models/conscripts/female_03.mdl");
	engine->PrecacheModel("models/conscripts/female_04.mdl");
	engine->PrecacheModel("models/conscripts/female_06.mdl");
	engine->PrecacheModel("models/conscripts/female_07.mdl");
	engine->PrecacheModel("random"); //Evil fucking hack

	const char *pModelName = STRING( GetModelName() ); 	
	if( !Q_stricmp( pModelName, "random" ) )
	{
			int min = 1;
			int max = 15;
			double scaled = (double)rand()/RAND_MAX;
			int r = (max - min +1)*scaled + min;
	
			if		(r == 1)	{ SetModel( "models/conscripts/male_01.mdl"); }
			else if (r == 2)	{ SetModel( "models/conscripts/female_01.mdl"); }
			else if (r == 3)	{ SetModel( "models/conscripts/male_02.mdl"); }
			else if (r == 4)	{ SetModel( "models/conscripts/female_02.mdl"); }
			else if (r == 5)	{ SetModel( "models/conscripts/male_03.mdl"); }
			else if (r == 6)	{ SetModel( "models/conscripts/female_03.mdl"); }
			else if (r == 7)	{ SetModel( "models/conscripts/male_04.mdl"); }
			else if (r == 8)	{ SetModel( "models/conscripts/female_04.mdl"); }
			else if (r == 9)	{ SetModel( "models/conscripts/male_05.mdl"); }
			else if (r == 10)	{ SetModel( "models/conscripts/female_06.mdl"); }
			else if (r == 11)	{ SetModel( "models/conscripts/male_06.mdl"); }
			else if (r == 12)	{ SetModel( "models/conscripts/female_07.mdl"); }
			else if (r == 13)	{ SetModel( "models/conscripts/male_07.mdl"); }
			else if (r == 14)	{ SetModel( "models/conscripts/male_08.mdl"); }
			else if (r == 15)	{ SetModel( "models/conscripts/male_09.mdl"); }
	}

	if( !GetModelName() ) //revert to user-defined model (or the new models if set to "random") if no model is given
	{
		SetModelName( MAKE_STRING( sk_conscript_model.GetString() ) );
	}

	engine->PrecacheModel( STRING( GetModelName() ) );

	enginesound->PrecacheSound("barney/ba_pain1.wav");
	enginesound->PrecacheSound("barney/ba_pain2.wav");
	enginesound->PrecacheSound("barney/ba_pain3.wav");

	enginesound->PrecacheSound("barney/ba_die1.wav");
	enginesound->PrecacheSound("barney/ba_die2.wav");
	enginesound->PrecacheSound("barney/ba_die3.wav");
	
	enginesound->PrecacheSound("barney/ba_close.wav");

	// every new barney must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();
	BaseClass::Precache();
}	

// Init talk data
void CNPC_Conscript::TalkInit()
{
	
	BaseClass::TalkInit();

		// vortigaunt will try to talk to friends in this order:
	m_szFriends[0] = "npc_conscript";
	m_szFriends[1] = "npc_vortigaunt";

	// get voice for head - just one barney voice for now
	GetExpresser()->SetVoicePitch( 100 );
}


static bool IsFacing( CBaseCombatCharacter *pBCC, const Vector &reference )
{
	Vector vecDir = (reference - pBCC->GetLocalOrigin());
	vecDir.z = 0;
	VectorNormalize( vecDir );
	Vector vecForward = pBCC->BodyDirection2D( );

	// He's facing me, he meant it
	if ( DotProduct( vecForward, vecDir ) > 0.96 )	// +/- 15 degrees or so
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CNPC_Conscript::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	// make sure friends talk about it if player hurts talkmonsters...
	int ret = BaseClass::OnTakeDamage_Alive( info );
	if (!IsAlive())
	{
		return ret;
	}

	if ( m_NPCState != NPC_STATE_PRONE && (info.GetAttacker()->GetFlags() & FL_CLIENT) )
	{

		// This is a heurstic to determine if the player intended to harm me
		// If I have an enemy, we can't establish intent (may just be crossfire)
		if ( GetEnemy() == NULL )
		{
			// If I'm already suspicious, get mad
			if (m_afMemory & bits_MEMORY_SUSPICIOUS)
			{
				// Alright, now I'm pissed!
				Speak( CONSCRIPT_MAD );

				Remember( bits_MEMORY_PROVOKED );


				// Allowed to hit the player now!
				CapabilitiesRemove(bits_CAP_NO_HIT_PLAYER);

				StopFollowing();
				if ( sk_conscript_ff_retaliation.GetInt() != 0 ) //Only retaliate against the player if ff_retaliation is set to 1
				{
					AddRelationship( "player D_HT 1",	NULL ); 
				}
				else
				{
					return ret;
				}
			}
			else
			{
				// Hey, be careful with that
				Speak( CONSCRIPT_SHOT );
				Remember( bits_MEMORY_SUSPICIOUS );
			}
		}
		else if ( !(GetEnemy()->IsPlayer()) && (m_lifeState != LIFE_DEAD ))
		{
			Speak( CONSCRIPT_SHOT );
		}
	}
	return ret;
}

//------------------------------------------------------------------------------
// Purpose : Override to always shoot at eyes (for ducking behind things)
// Input   :
// Output  :
//------------------------------------------------------------------------------
Vector CNPC_Conscript::BodyTarget( const Vector &posSrc, bool bNoisy ) 
{
	return EyePosition();
}

//=========================================================
// PainSound
//=========================================================
void CNPC_Conscript::PainSound ( void )
{
	if (gpGlobals->curtime < m_painTime)
		return;

	AIConcept_t concepts[] =
	{
		CONSCRIPT_PAIN1,
		CONSCRIPT_PAIN2,
		CONSCRIPT_PAIN3,
	};
	
	m_painTime = gpGlobals->curtime + random->RandomFloat(0.5, 0.75);

	Speak( concepts[random->RandomInt(0,2)] );
}

//=========================================================
// DeathSound 
//=========================================================
void CNPC_Conscript::DeathSound ( void )
{
	AIConcept_t concepts[] =
	{
		CONSCRIPT_DIE1,
		CONSCRIPT_DIE2,
		CONSCRIPT_DIE3,
	};
	
	Speak( concepts[random->RandomInt(0,2)] );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Conscript::TraceAttack( const CTakeDamageInfo &inputInfo, const Vector &vecDir, trace_t *ptr )
{
	CTakeDamageInfo info = inputInfo;

	switch( ptr->hitgroup)
	{
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
		if (info.GetDamageType() & (DMG_BULLET | DMG_SLASH | DMG_BLAST))
		{
			info.ScaleDamage( 0.5f );
		}
		break;
	case 10:
		if (info.GetDamageType() & (DMG_BULLET | DMG_SLASH | DMG_CLUB))
		{
			info.SetDamage( info.GetDamage() - 20 );
			if (info.GetDamage() <= 0)
			{
				g_pEffects->Ricochet( ptr->endpos, (vecDir*-1.0f) );
				info.SetDamage( 0 );
			}
		}
		// always a head shot
		ptr->hitgroup = HITGROUP_HEAD;
		break;
	}

	//BaseClass::TraceAttack( info, vecDir, ptr );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int CNPC_Conscript::TranslateSchedule( int scheduleType )
{
	int baseType;

	switch( scheduleType )
	{
	case SCHED_CONSCRIPT_DRAW:
		if ( GetEnemy() != NULL )
		{
			// face enemy, then draw.
			return SCHED_CONSCRIPT_DRAW;
		}
		else
		{
			// BUGBUG: What is this code supposed to do when there isn't an enemy?
			Warning( "BUG IN CONSCRIPT AI!\n");
		}
		break;

	// Hook these to make a looping schedule
	case SCHED_TARGET_FACE:
		{
			// call base class default so that barney will talk
			// when 'used' 
			baseType = BaseClass::TranslateSchedule(scheduleType);

			if (baseType == SCHED_IDLE_STAND)
				return SCHED_CONSCRIPT_FACE_TARGET;	// override this for different target face behavior
			else
				return baseType;
		}
		break;

	case SCHED_CHASE_ENEMY:
		{
			// ---------------------------------------------
			// If I'm in ducking, cover pause for while
			// before running towards my enemy.  See if they
			// come out first as this is a good place to be!
			// ---------------------------------------------
			if (HasMemory(bits_MEMORY_INCOVER))
			{
				Forget( bits_MEMORY_INCOVER );
				return SCHED_COMBAT_SWEEP;
			}
		}
		break;
	case SCHED_TARGET_CHASE:
		return SCHED_CONSCRIPT_FOLLOW;
		break;

	case SCHED_IDLE_STAND:
		{
			// call base class default so that scientist will talk
			// when standing during idle
			baseType = BaseClass::TranslateSchedule(scheduleType);

			if (baseType == SCHED_IDLE_STAND)
			{
				// just look straight ahead
				return SCHED_CONSCRIPT_STAND;
			}
			return baseType;
			break;

		}
	case SCHED_RANGE_ATTACK1:
		{
			if( Weapon_OwnsThisType( "weapon_rpg" ) )
			{
				return SCHED_CONSCRIPT_FIRE_RPG;
				break;
			}
		}

	case SCHED_ESTABLISH_LINE_OF_FIRE:
		{
			if( Weapon_OwnsThisType( "weapon_rpg" ) )
			{
				return SCHED_CONSCRIPT_ESTABLISH_RPG_LINE_OF_FIRE;
			}
			break;
		}

	case SCHED_FAIL_ESTABLISH_LINE_OF_FIRE:
		{
			//return SCHED_CONSCRIPT_AIM;
			return SCHED_COMBAT_SWEEP;
			break;
		}
	}
	return BaseClass::TranslateSchedule( scheduleType );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int CNPC_Conscript::SelectSchedule ( void )
{
	// These things are done in any state but dead and prone
	if (m_NPCState != NPC_STATE_DEAD && m_NPCState != NPC_STATE_PRONE)
	{
		if ( HasCondition( COND_HEAR_DANGER ) )
		{
			return SCHED_TAKE_COVER_FROM_BEST_SOUND;
		}
		if( HasCondition( COND_HEAR_BUGBAIT ) )
		{
			return SCHED_INVESTIGATE_SOUND;
		}


		if ( HasCondition( COND_ENEMY_DEAD ) && IsOkToCombatSpeak() )
		{
			Speak( CONSCRIPT_KILL );
		}
	}
	switch( m_NPCState )
	{
	case NPC_STATE_PRONE:
		{
			if (m_bInBarnacleMouth)
			{
				return SCHED_CONSCRIPT_BARNACLE_CHOMP;
			}
			else
			{
				return SCHED_CONSCRIPT_BARNACLE_HIT;
			}
		}
	case NPC_STATE_COMBAT:
		{ //inb4 "haha yes look at this brainlet, he is not using cases!" -Stacker

// dead enemy
			if ( HasCondition( COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return BaseClass::SelectSchedule();
			}

			if (HasCondition(COND_REPEATED_DAMAGE))
			{
				int iPercent = random->RandomInt(0, 99);

				if (m_iPersonality == CONSCRIPT_PERSONALITY_BALANCED)
				{

					if (iPercent <= 25 && GetEnemy() != NULL)
					{
						return SCHED_TAKE_COVER_FROM_ENEMY;
					}
					else
					{
						return SCHED_SMALL_FLINCH;
					}
				}

				if (m_iPersonality == CONSCRIPT_PERSONALITY_CAUTIOUS) //twice as likely to take cover
				{

					if (iPercent <= 50 && GetEnemy() != NULL)
					{
						return SCHED_TAKE_COVER_FROM_ENEMY;
					}
					else
					{
						return SCHED_SMALL_FLINCH;
					}
				}

				if ( m_iPersonality == CONSCRIPT_PERSONALITY_AGGRESSIVE) //gets pissed off
				{

					if (iPercent <= 65 && GetEnemy() != NULL)
					{
						return SCHED_CHASE_ENEMY;
					}
					else
					{
						return SCHED_SMALL_FLINCH;
					}
				}
			}

			if (HasCondition(COND_NEW_ENEMY)) //Comment out or delete the Msgs here unless you like your dev console getting spammed -Stacker
			{
				int iPercent = random->RandomInt(0, 99);

				if ( m_iPersonality == CONSCRIPT_PERSONALITY_BALANCED)
				{
				if (iPercent <= 25 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (balanced): CHOSE FALLBACK!\n");
						return SCHED_RUN_FROM_ENEMY_FALLBACK;
					}
					else if (iPercent <= 50 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (balanced): CHOSE TO TAKE COVER!\n");
						return SCHED_TAKE_COVER_FROM_ENEMY;
					}
					else if (iPercent >= 75 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (balanced): CHOSE ESTABLISH LOF!\n");
						return SCHED_ESTABLISH_LINE_OF_FIRE;
					}
				}

				if ( m_iPersonality == CONSCRIPT_PERSONALITY_CAUTIOUS)
				{
				if (iPercent <= 30 && GetEnemy() != NULL)
					{
						Msg("CONSCRIPT (cautious): CHOSE TO TAKE COVER!\n");
						return SCHED_TAKE_COVER_FROM_ENEMY;
					}
					else if (iPercent <= 75 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (cautious): CHOSE CAUTIOUS TAKECOVER!\n");
						return SCHED_CONSCRIPT_CAUTIOUS_TAKECOVER;
					}
					else if (iPercent >= 25 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (cautious): CHOSE ESTABLISH LOF!\n");
						return SCHED_ESTABLISH_LINE_OF_FIRE;
					}
				}

				if  (m_iPersonality == CONSCRIPT_PERSONALITY_AGGRESSIVE) //More likely to chase enemy/establish LOF
				{
				if (iPercent <= 50 && GetEnemy() != NULL)
					{
						Msg("CONSCRIPT (aggressive): CHOSE CHASE ENEMY!\n");
						return SCHED_CHASE_ENEMY;
					}
					else if (iPercent <= 15 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (aggressive): CHOSE TO TAKE COVER!\n");
						return SCHED_TAKE_COVER_FROM_ENEMY;
					}
					else if (iPercent >= 75 && GetEnemy() != NULL) 
					{
						Msg("CONSCRIPT (aggressive): CHOSE ESTABLISH LOF!\n");
						return SCHED_ESTABLISH_LINE_OF_FIRE;
					}
				}


			}

			if (HasCondition(COND_ENEMY_OCCLUDED))
			{
				int iPercent = random->RandomInt(0, 99);

				if ( m_iPersonality == CONSCRIPT_PERSONALITY_BALANCED ) //Voicelines here are commented out due to not being implemented yet -Stacker
				{
					if (iPercent <= 30 && GetEnemy() != NULL)
					{
						return SCHED_ESTABLISH_LINE_OF_FIRE;
						Msg("CONSCRIPT: I'M PICKING ESTABLISH LOF AGAINST OCCLUDED ENEMY!\n");
						//Speak ( CONSCRIPT_GOINGAFTER);
					}
					else if (iPercent <= 60 && GetEnemy() != NULL)
					{
						return SCHED_CONSCRIPT_SUPPRESSINGFIRE;
						Msg("CONSCRIPT: I'M PICKING SUPPRESSING FIRE AGAINST OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_SUPPRESS);
					}
					else if (iPercent <= 40 && GetEnemy() != NULL)
					{
						return SCHED_STANDOFF;
						Msg("CONSCRIPT: I'M PICKING STANDOFF AGAINST OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_TAUNT);
					}
				}

				if (m_iPersonality == CONSCRIPT_PERSONALITY_CAUTIOUS) //more likely to use suppressing fire/standoff
				{
					if (iPercent <= 10 && GetEnemy() != NULL)
					{
						return SCHED_ESTABLISH_LINE_OF_FIRE;
						Msg("CONSCRIPT (cautious): I'M PICKING ESTABLISH LOF AGAINST OCCLUDED ENEMY!\n");
						//Speak ( CONSCRIPT_GOINGAFTER);
					}
					else if (iPercent <= 60 && GetEnemy() != NULL)
					{
						return SCHED_CONSCRIPT_SUPPRESSINGFIRE;
						Msg("CONSCRIPT (cautious): I'M PICKING SUPPRESSING FIRE AGAINST OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_SUPPRESS);
					}
					else if (iPercent <= 30 && GetEnemy() != NULL)
					{
						return SCHED_STANDOFF;
						Msg("CONSCRIPT (cautious): I'M PICKING STANDOFF AGAINST OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_TAUNT);
					}
				}

				if ( m_iPersonality == CONSCRIPT_PERSONALITY_AGGRESSIVE )//more likely to go after occluded enemies
				{
					if (iPercent <= 50 && GetEnemy() != NULL)
					{
						return SCHED_ESTABLISH_LINE_OF_FIRE;
						Msg("CONSCRIPT (aggressive): I'M PICKING ESTABLISH LOF AGAINST OCCLUDED ENEMY!\n");
						//Speak ( CONSCRIPT_GOINGAFTER);
					}
					else if (iPercent <= 20 && GetEnemy() != NULL)
					{
						return SCHED_CONSCRIPT_SUPPRESSINGFIRE;
						Msg("CONSCRIPT (aggressive): I'M PICKING SUPPRESSING FIRE AGAINST OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_SUPPRESS);
					}
					else if (iPercent <= 30 && GetEnemy() != NULL)
					{
						return SCHED_CHASE_ENEMY;
						Msg("CONSCRIPT (aggressive): I'M GOING AFTER OCCLUDED ENEMY!\n");
						//Speak (CONSCRIPT_TAUNT);
					}
				}
			
			}
			
			// always act surprized with a new enemy
			if ( HasCondition( COND_NEW_ENEMY ) && HasCondition( COND_LIGHT_DAMAGE) )
				return SCHED_SMALL_FLINCH;
				
			// wait for one schedule to draw gun
			if (!m_fGunDrawn )
				return SCHED_CONSCRIPT_DRAW;

			if ( HasCondition( COND_HEAVY_DAMAGE ) )
				return SCHED_TAKE_COVER_FROM_ENEMY;

			// ---------------------
			// no ammo
			// ---------------------
			if ( HasCondition ( COND_NO_PRIMARY_AMMO ) )
			{
				Speak( CONSCRIPT_OUT_AMMO );
				return SCHED_HIDE_AND_RELOAD;
			}
			else if (!HasCondition(COND_CAN_RANGE_ATTACK1) && HasCondition( COND_NO_SECONDARY_AMMO ))
			{
				Speak( CONSCRIPT_OUT_AMMO );
				return SCHED_HIDE_AND_RELOAD;
			}

			/* UNDONE: check distance for genade attacks...
			// If player is next to what I'm trying to attack...
			if ( HasCondition( COND_WEAPON_PLAYER_NEAR_TARGET ))
			{
				return SCHED_CONSCRIPT_AIM;
			}
			*/			

			// -------------------------------------------
			// If I might hit the player shooting...
			// -------------------------------------------
			if ( HasCondition( COND_WEAPON_PLAYER_IN_SPREAD ))
			{
				if ( IsOkToCombatSpeak() && m_nextLineFireTime	< gpGlobals->curtime)
				{
					Speak( CONSCRIPT_LINE_FIRE );
					m_nextLineFireTime = gpGlobals->curtime + 3.0f;
				}

				// Run to a new location or stand and aim
				if (random->RandomInt(0,2) == 0)
				{
					return SCHED_ESTABLISH_LINE_OF_FIRE;
				}
				else
				{
					return SCHED_CONSCRIPT_AIM;
				}
			}

			if ( HasCondition( COND_WEAPON_BLOCKED_BY_FRIEND ))
			{
				if ( IsOkToCombatSpeak() && m_nextLineFireTime	< gpGlobals->curtime)
				{
					Speak( CONSCRIPT_LINE_FIRE );
					m_nextLineFireTime = gpGlobals->curtime + 3.0f;
				}

				// Run to a new location or stand and aim
				if (random->RandomInt(0,2) == 0)
				{
					return SCHED_ESTABLISH_LINE_OF_FIRE;
				}
				else
				{
					return SCHED_CONSCRIPT_AIM;
				}
			}

			// -------------------------------------------
			// If I'm in cover and I don't have a line of
			// sight to my enemy, wait randomly before attacking
			// -------------------------------------------

		}
		break;

	case NPC_STATE_ALERT:	
		{
			if( HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE) )
			{
				AI_EnemyInfo_t *pDanger = GetEnemies()->GetDangerMemory();
				if( pDanger && FInViewCone(pDanger->vLastKnownLocation) && !BaseClass::FVisible(pDanger->vLastKnownLocation) )
				{
					// I've been hurt, I'm facing the danger, but I don't see it, so move from this position.
					return SCHED_TAKE_COVER_FROM_ORIGIN;
				}
			}

	
			if( HasCondition( COND_HEAR_COMBAT ) )
			{
				CSound *pSound = GetBestSound();

				if( pSound && pSound->IsSoundType( SOUND_COMBAT ) )
				{
					return SCHED_INVESTIGATE_SOUND;
				}
			}


		}
	case NPC_STATE_IDLE:
		if ( HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE))
		{
			// flinch if hurt
			return SCHED_SMALL_FLINCH;
		}


	
		// try to say something about smells
		TrySmellTalk();
		break;
	}
	
	return BaseClass::SelectSchedule();
}




//-----------------------------------------------------------------------------
// Purpose:  This is a generic function (to be implemented by sub-classes) to
//			 handle specific interactions between different types of characters
//			 (For example the barnacle grabbing an NPC)
// Input  :  Constant for the type of interaction
// Output :	 true  - if sub-class has a response for the interaction
//			 false - if sub-class has no response
//-----------------------------------------------------------------------------
bool CNPC_Conscript::HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt)
{
	if (interactionType == g_interactionBarnacleVictimDangle)
	{
		// Force choosing of a new schedule
		ClearSchedule("Released by Barnacle");
		m_bInBarnacleMouth	= true;
		return true;
	}
	else if ( interactionType == g_interactionBarnacleVictimReleased )
	{
		m_IdealNPCState = NPC_STATE_IDLE;

		CPASAttenuationFilter filter( this );

		m_bInBarnacleMouth	= false;
		SetAbsVelocity( vec3_origin );
		SetMoveType( MOVETYPE_STEP );
		return true;
	}
	else if ( interactionType == g_interactionBarnacleVictimGrab )
	{
		if ( GetFlags() & FL_ONGROUND )
		{
			RemoveFlag( FL_ONGROUND );
		}
		m_IdealNPCState = NPC_STATE_PRONE;
		PainSound();
		return true;
	}
	return false;
}

void CNPC_Conscript::DeclineFollowing( void )
{
	Speak( CONSCRIPT_POK );
}

WeaponProficiency_t CNPC_Conscript::CalcWeaponProficiency( CBaseCombatWeapon *pWeapon )
{
	if( FClassnameIs( pWeapon, "weapon_pistol" ) )
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}

	if( FClassnameIs( pWeapon, "weapon_smg1" ) )
	{
		return WEAPON_PROFICIENCY_VERY_GOOD;
	}

	if( FClassnameIs( pWeapon, "weapon_oicw" ) )
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}


	if( FClassnameIs( pWeapon, "weapon_tommygun" ) )
	{
		return WEAPON_PROFICIENCY_VERY_GOOD;
	}

	if( FClassnameIs( pWeapon, "weapon_dbarrel" ) || FClassnameIs( pWeapon, "weapon_shotgun" ) ) //Gotta have perfect proficiency with the shotguns or they'll be useless
	{
		return WEAPON_PROFICIENCY_PERFECT;
	}

	else
	{
		return WEAPON_PROFICIENCY_GOOD;
	}

	return BaseClass::CalcWeaponProficiency( pWeapon );
}


//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

//=========================================================
// > SCHED_CONSCRIPT_FOLLOW
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_FOLLOW,

	"	Tasks"
	"		TASK_GET_PATH_TO_TARGET			0"
	"		TASK_MOVE_TO_TARGET_RANGE		128"	// Move within 128 of target ent (client)
	"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_TARGET_FACE "
	""
	"	Interrupts"
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_HEAR_DANGER"
	"		COND_PROVOKED"
);

//=========================================================
//  > SCHED_CONSCRIPT_DRAW
//		much better looking draw schedule for when
//		conscript knows who he's gonna attack.
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_DRAW,

	"	Tasks"
	"		 TASK_STOP_MOVING					0"
	"		 TASK_FACE_ENEMY					0"
	"		 TASK_PLAY_SEQUENCE_FACE_ENEMY		ACTIVITY:ACT_ARM "
	""
	"	Interrupts"
);

//===============================================
//	> SCHED_CONSCRIPT_AIM
//
//	Stand in place and aim at enemy (used when
//  line of sight blocked by player)
//===============================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_AIM,

	"	Tasks"
	"		TASK_STOP_MOVING		0"
	"		TASK_FACE_ENEMY			0"
	"		TASK_PLAY_SEQUENCE		ACTIVITY:ACT_CONSCRIPT_AIM"
	""
	"	Interrupts"
	"		COND_NEW_ENEMY"
	"		COND_ENEMY_DEAD"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_NO_PRIMARY_AMMO"
	"		COND_WEAPON_HAS_LOS"
	"		COND_CAN_MELEE_ATTACK1 "
	"		COND_CAN_MELEE_ATTACK2 "
	"		COND_HEAR_DANGER"
);

//=========================================================
// > SCHED_CONSCRIPT_FACE_TARGET
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_FACE_TARGET,

	"	Tasks"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE"
	"		TASK_FACE_TARGET			0"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE"
	"		TASK_SET_SCHEDULE			SCHEDULE:SCHED_TARGET_CHASE"
	""
	"	Interrupts"
	//"		CLIENT_PUSH			<<TODO>>
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_HEAR_DANGER"
	"		COND_PROVOKED"
);


AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_ESTABLISH_RPG_LINE_OF_FIRE,

	"	Tasks "
	"		TASK_SET_FAIL_SCHEDULE			SCHEDULE:SCHED_ESTABLISH_LINE_OF_FIRE_FALLBACK"
	"		TASK_GET_PATH_TO_ENEMY_LOS		0"
	"		TASK_SPEAK_SENTENCE				1"
	"		TASK_RUN_PATH					0"
	"		TASK_WAIT_FOR_MOVEMENT			0"
	"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_CONSCRIPT_FIRE_RPG"
	""
	"	Interrupts "
	"		COND_NEW_ENEMY"
	"		COND_ENEMY_DEAD"
	"		COND_LOST_ENEMY"
	"		COND_CAN_RANGE_ATTACK1"
	"		COND_CAN_MELEE_ATTACK1"
	"		COND_CAN_RANGE_ATTACK2"
	"		COND_CAN_MELEE_ATTACK2"
	"		COND_HEAR_DANGER"
);

AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_FIRE_RPG,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_FACE_ENEMY				0"
	"		TASK_ANNOUNCE_ATTACK		1"	// 1 = primary attack
	"		TASK_PLAY_SEQUENCE_FACE_ENEMY	ACTIVITY:ACT_CONSCRIPT_LOAD_RPG"
	"		TASK_WAIT_RANDOM			0.75"
	"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_RANGE_ATTACK1"
	""
	"	Interrupts"
	""
	"	COND_ENEMY_DEAD"
);

//=========================================================
// > SCHED_CONSCRIPT_STAND
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_STAND,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_SET_ACTIVITY			ACTIVITY:ACT_IDLE "
	"		TASK_WAIT					2"	// repick IDLESTAND every two seconds.
	"		TASK_TALKER_HEADRESET		0"	// reset head position
	""
	"	Interrupts	 "
	"		COND_NEW_ENEMY"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_SMELL"
	"		COND_PROVOKED"
	"		COND_HEAR_COMBAT"
	"		COND_HEAR_DANGER"
);

//=========================================================
// > SCHED_CONSCRIPT_BARNACLE_HIT
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_BARNACLE_HIT,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_HIT"
	"		TASK_SET_SCHEDULE			SCHEDULE:SCHED_CONSCRIPT_BARNACLE_PULL"
	""
	"	Interrupts"
);

//=========================================================
// > SCHED_CONSCRIPT_BARNACLE_PULL
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_BARNACLE_PULL,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_PULL"
	""
	"	Interrupts"
);

//=========================================================
// > SCHED_CONSCRIPT_BARNACLE_CHOMP
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_BARNACLE_CHOMP,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_CHOMP"
	"		 TASK_SET_SCHEDULE			SCHEDULE:SCHED_CONSCRIPT_BARNACLE_CHEW"
	""
	"	Interrupts"
);

AI_DEFINE_SCHEDULE
(
    SCHED_CONSCRIPT_SUPPRESSINGFIRE,
 
    "   Tasks"
    "   TASK_STOP_MOVING    0"
    "   TASK_GET_PATH_TO_ENEMY_LKP_LOS  0"
	"	TASK_RUN_PATH				0"
	"	TASK_WAIT_FOR_MOVEMENT		0"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    ""
    "   Interrupts"
    "       COND_WEAPON_BLOCKED_BY_FRIEND"
     "      COND_ENEMY_DEAD"
    "       COND_LIGHT_DAMAGE"
    "       COND_HEAVY_DAMAGE"
    "       COND_NO_PRIMARY_AMMO"
     "      COND_HEAR_DANGER"
     "      COND_HEAR_MOVE_AWAY"
);

AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_CAUTIOUS_TAKECOVER,

	"   Tasks"
	"   TASK_STOP_MOVING    0"
    "   TASK_GET_PATH_TO_ENEMY_LOS  0"
	"	TASK_RUN_PATH				0"
	"	TASK_WAIT_FOR_MOVEMENT		0"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
    "   TASK_RANGE_ATTACK1  0"
    "   TASK_WAIT_RANDOM    0.15"
	"	TASK_FIND_FAR_NODE_COVER_FROM_ENEMY 0"
	""
    "   Interrupts"
    "       COND_WEAPON_BLOCKED_BY_FRIEND"
     "      COND_ENEMY_DEAD"
    "       COND_LIGHT_DAMAGE"
    "       COND_HEAVY_DAMAGE"
    "       COND_NO_PRIMARY_AMMO"
     "      COND_HEAR_DANGER"
     "      COND_HEAR_MOVE_AWAY"
	
			
			
);

//=========================================================
// > SCHED_CONSCRIPT_BARNACLE_CHEW
//=========================================================
AI_DEFINE_SCHEDULE
(
	SCHED_CONSCRIPT_BARNACLE_CHEW,

	"	Tasks"
	"		 TASK_PLAY_SEQUENCE			ACTIVITY:ACT_BARNACLE_CHEW"
);


//#endif