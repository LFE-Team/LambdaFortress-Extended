//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		The CrematorRed. Walks around with an immolator and disintegrates and burns things.
//				Don't fuck with him or he'll burn your face off
//				CURRENTLY USING CLASS_COMBINE FOR NOW UNTIL I GET CLASS_CREMATORRED IMPLEMENTED
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "player.h"
#include "soundent.h"
#include "te_particlesystem.h"
#include "ndebugoverlay.h"
#include "in_buttons.h"
#include "ai_basenpc.h"
#include "ai_memory.h"

#include "beam_shared.h"
#include "game.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_route.h"
#include "ai_squad.h"
#include "ai_task.h"
#include "ai_node.h"
#include "ai_hint.h"
#include "ai_squadslot.h"
#include "ai_motor.h"
#include "npcevent.h"
#include "gib.h"
#include "ai_interactions.h"
#include "ndebugoverlay.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "npc_crematorred.h"
#include "soundent.h"
#include "IEffects.h"
#include "basecombatweapon.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

extern ConVar	sk_cremator_health;
extern ConVar	sk_cremator_dmg_immo;
extern ConVar	sk_cremator_max_range;
extern ConVar	sk_cremator_immolator_color_r;
extern ConVar	sk_cremator_immolator_color_g;
extern ConVar	sk_cremator_immolator_color_b;
extern ConVar	sk_cremator_immolator_beamsprite;

LINK_ENTITY_TO_CLASS( monster_crematorred, CNPC_CrematorRed );
LINK_ENTITY_TO_CLASS( npc_crematorred, CNPC_CrematorRed );

//=========================================================
// Spawn
//=========================================================
void CNPC_CrematorRed::Spawn()
{
	if ( sv_hl2_beta.GetBool() )
	{
		Precache( );

		SetModel( "models/cremator_npc.mdl" );
	
		SetRenderColor( 255, 255, 255, 255 );

		SetHullType(HULL_HUMAN);
		SetHullSizeNormal();
	
		SetSolid( SOLID_BBOX );
		AddSolidFlags( FSOLID_NOT_STANDABLE );
		SetMoveType( MOVETYPE_STEP );
		m_bloodColor		= BLOOD_COLOR_YELLOW; //NOTE TO DL'ers: You really, really, really want to change this
		ClearEffects();
		m_iHealth			= sk_cremator_health.GetFloat();
		m_flFieldOfView		= 0.65;
		m_NPCState			= NPC_STATE_NONE;

		m_iVoicePitch		= random->RandomInt( 85, 110 );

		CapabilitiesClear(); //Still can't open doors on his own for some reason... WTF
		CapabilitiesAdd( bits_CAP_MOVE_GROUND | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE | bits_CAP_DOORS_GROUP);
		CapabilitiesAdd( bits_CAP_INNATE_RANGE_ATTACK1 | bits_CAP_MOVE_SHOOT | bits_CAP_TURN_HEAD | bits_CAP_ANIMATEDFACE );
		CapabilitiesAdd( bits_CAP_SQUAD | bits_CAP_NO_HIT_SQUADMATES | bits_CAP_FRIENDLY_DMG_IMMUNE  );

		m_iBravery = 0;

		NPCInit();

		BaseClass::Spawn();
	}
	else
	{
		UTIL_Remove( this );
	}
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CNPC_CrematorRed::Precache()
{
	BaseClass::Precache();

	PrecacheModel( "models/gibs/cremator_arm1.mdl" );
	PrecacheModel( "models/gibs/cremator_arm2.mdl" );
	PrecacheModel( "models/gibs/cremator_lfoot.mdl" );
	PrecacheModel( "models/gibs/cremator_rfoot.mdl" );
	PrecacheModel( "models/gibs/cremator_head.mdl" );
	PrecacheModel( "models/gibs/cremator_midsection.mdl" );

	PrecacheModel( "models/cremator_npc.mdl" );
	PrecacheModel( "sprites/lgtning.vmt" );

	PrecacheScriptSound( "Crem.Pain" );
	PrecacheScriptSound( "Crem.Die" );
	PrecacheScriptSound( "Crem.ImmoShoot" );
	PrecacheScriptSound( "Crem.Footstep" );
	PrecacheScriptSound( "Crem.Swish" );
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
Class_T	CNPC_CrematorRed::Classify ( void )
{
	return	CLASS_PLAYER_ALLY;
}
