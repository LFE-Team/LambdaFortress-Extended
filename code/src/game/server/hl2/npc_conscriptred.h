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

#ifndef	NPC_CONSCRIPTRED_H
#define	NPC_CONSCRIPTRED_H
#pragma once

//#if 0
#include "npc_talker.h"

//=========================================================
//	>> CNPC_ConscriptRed
//=========================================================
//class CNPC_ConscriptRed : public CAI_PlayerAlly
class CNPC_ConscriptRed : public CNPCSimpleTalker
{
//	DECLARE_CLASS( CNPC_ConscriptRed, CAI_PlayerAlly );
	DECLARE_CLASS( CNPC_ConscriptRed, CNPCSimpleTalker );
public:
	void			Spawn( void );
	void			Precache( void );
	float			MaxYawSpeed( void );
	int				GetSoundInterests( void );
	void			ConscriptRedFirePistol( void );
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

	// Override these to set behavior
	virtual int		TranslateSchedule( int scheduleType );
	virtual int		SelectSchedule( void );

	void			DeathSound( void );
	void			PainSound( void );
	
	void			TalkInit( void );

	void			TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr );

	bool			m_fGunDrawn;
	float			m_painTime;
	float			m_checkAttackTime;
	float			m_nextLineFireTime;
	bool			m_lastAttackCheck;
	bool			m_bInBarnacleMouth;


	//=========================================================
	// ConscriptRed Tasks
	//=========================================================
	enum 
	{
		TASK_CONSCRIPTRED_CROUCH = BaseClass::NEXT_TASK,
	};

	//=========================================================
	// ConscriptRed schedules
	//=========================================================
	enum
	{
		SCHED_CONSCRIPTRED_FOLLOW = BaseClass::NEXT_SCHEDULE,
		SCHED_CONSCRIPTRED_DRAW,
		SCHED_CONSCRIPTRED_FACE_TARGET,
		SCHED_CONSCRIPTRED_STAND,
		SCHED_CONSCRIPTRED_AIM,
		SCHED_CONSCRIPTRED_BARNACLE_HIT,
		SCHED_CONSCRIPTRED_BARNACLE_PULL,
		SCHED_CONSCRIPTRED_BARNACLE_CHOMP,
		SCHED_CONSCRIPTRED_BARNACLE_CHEW,
	};


public:
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;
};

//#endif

#endif	//NPC_CONSCRIPTRED_H