//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose:		This is the base version of the combine (not instanced only subclassed)
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#ifndef	NPC_CONSCRIPT_H
#define	NPC_CONSCRIPT_H
#pragma once

//#if 0
#include "npc_talker.h"
#include "ai_basenpc.h"
#include "ai_basehumanoid.h"
#include "ai_behavior.h"
#include "ai_behavior_assault.h"
#include "ai_behavior_standoff.h"
#include "ai_behavior_follow.h"
#include "ai_behavior_functank.h"
#include "ai_behavior_rappel.h"
#include "ai_behavior_actbusy.h"
#include "ai_sentence.h"
#include "ai_baseactor.h"

//=========================================================
//	>> CNPC_Conscript
//=========================================================
//class CNPC_Conscript : public CAI_PlayerAlly
class CNPC_Conscript : public CNPCSimpleTalker
{
//	DECLARE_CLASS( CNPC_Conscript, CAI_PlayerAlly );
	DECLARE_CLASS( CNPC_Conscript, CNPCSimpleTalker );
public:
	void			Spawn( void );
	void			Precache( void );
	float			MaxYawSpeed( void );
	int				GetSoundInterests( void );
	void			ConscriptFirePistol( void );
	void			AlertSound( void );
	Class_T			Classify ( void );
	void			HandleAnimEvent( animevent_t *pEvent );
	bool			HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt);

	void			RunTask( const Task_t *pTask );
	int				ObjectCaps( void ) { return UsableNPCObjectCaps( BaseClass::ObjectCaps() ); }
	int				OnTakeDamage_Alive( const CTakeDamageInfo &info );
	Vector			BodyTarget( const Vector &posSrc, bool bNoisy = true );

	Activity		GetFollowActivity( float flDistance ) { return ACT_RUN; }

	void			DeclineFollowing( void );

	Activity		NPC_TranslateActivity( Activity eNewActivity );
	WeaponProficiency_t CalcWeaponProficiency( CBaseCombatWeapon *pWeapon );

	EHANDLE m_hPhysicsEnt;

	// Override these to set behavior
	virtual int		TranslateSchedule( int scheduleType );
	virtual int		SelectSchedule( void );

	void			DeathSound( void );
	void			PainSound( void );
	
	void			TalkInit( void );

	bool			CreateBehaviors();

	void			TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr );

	bool			m_fGunDrawn;
	float			m_painTime;
	float			m_checkAttackTime;
	float			m_nextLineFireTime;
	bool			m_lastAttackCheck;
	bool			m_bInBarnacleMouth;
	bool			m_bIsFemale;
	int				m_iPersonality;

	CAI_ActBusyBehavior			m_ActBusyBehavior;

	bool FindNearestPhysicsObject( int iMaxMass );
	virtual bool CanSwatPhysicsObjects( void ) { return true; }
	float DistToPhysicsEnt( void );
	int GetSwatActivity( void );

	//=========================================================
	// Conscript Tasks
	//=========================================================
	enum 
	{
		TASK_CONSCRIPT_CROUCH = BaseClass::NEXT_TASK,
	};

	//=========================================================
	// Conscript schedules
	//=========================================================
	enum
	{
		SCHED_CONSCRIPT_FOLLOW = BaseClass::NEXT_SCHEDULE,
		SCHED_CONSCRIPT_DRAW,
		SCHED_CONSCRIPT_FACE_TARGET,
		SCHED_CONSCRIPT_STAND,
		SCHED_CONSCRIPT_AIM,
		SCHED_CONSCRIPT_FIRE_RPG,
		SCHED_CONSCRIPT_ESTABLISH_RPG_LINE_OF_FIRE,
		SCHED_CONSCRIPT_SUPPRESSINGFIRE,
		SCHED_CONSCRIPT_BARNACLE_HIT,
		SCHED_CONSCRIPT_BARNACLE_PULL,
		SCHED_CONSCRIPT_BARNACLE_CHOMP,
		SCHED_CONSCRIPT_BARNACLE_CHEW,
		SCHED_CONSCRIPT_CAUTIOUS_TAKECOVER,
	};


public:
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;
};

//#endif

#endif	//NPC_CONSCRIPT_H