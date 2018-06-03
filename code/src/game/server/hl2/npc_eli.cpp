//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Dr. Eli Vance, earths last great hope, single-handedly fighting
//			off both an evil alien invasion, as well as trying to stop 
//			that idiot lab assistant from putting the moves on his daughter.
//=============================================================================//


//-----------------------------------------------------------------------------
// Generic NPC - purely for scripted sequence work.
//-----------------------------------------------------------------------------
#include	"cbase.h"
#include	"npcevent.h"
#include	"ai_basenpc.h"
#include	"ai_hull.h"
#include "ai_baseactor.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// NPC's Anim Events Go Here
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CNPC_Eli : public CAI_BaseActor
{
public:
	DECLARE_CLASS( CNPC_Eli, CAI_BaseActor );

	void	Spawn( void );
	void	Precache( void );
	Class_T Classify ( void );
	void	HandleAnimEvent( animevent_t *pEvent );
	int		GetSoundInterests( void );
	void	SetupWithoutParent( void );
	void	PrescheduleThink( void );
};

LINK_ENTITY_TO_CLASS( npc_eli, CNPC_Eli );

//-----------------------------------------------------------------------------
// Classify - indicates this NPC's place in the 
// relationship table.
//-----------------------------------------------------------------------------
Class_T	CNPC_Eli::Classify ( void )
{
	return	CLASS_PLAYER_ALLY_VITAL;
}



//-----------------------------------------------------------------------------
// HandleAnimEvent - catches the NPC-specific messages
// that occur when tagged animation frames are played.
//-----------------------------------------------------------------------------
void CNPC_Eli::HandleAnimEvent( animevent_t *pEvent )
{
	switch( pEvent->event )
	{
	case 1:
	default:
		BaseClass::HandleAnimEvent( pEvent );
		break;
	}
}

//-----------------------------------------------------------------------------
// GetSoundInterests - generic NPC can't hear.
//-----------------------------------------------------------------------------
int CNPC_Eli::GetSoundInterests ( void )
{
	return	NULL;
}

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CNPC_Eli::Spawn()
{
	// Eli is allowed to use multiple models, because he appears in the pod.
	// He defaults to his normal model.
	char *szModel = (char *)STRING( GetModelName() );
	if (!szModel || !*szModel)
	{
		szModel = "models/eli.mdl";
		SetModelName( AllocPooledString(szModel) );
	}

	Precache();
	SetModel( szModel );
	char szMapName[256];
	Q_strncpy(szMapName, STRING(gpGlobals->mapname), sizeof(szMapName) );
	Q_strlower(szMapName);

	if( !Q_strnicmp( szMapName, "d1", 2 ) )
	{
		// Streetwar scanners are claw scanners
		PrecacheModel("models/hl2/eli.mdl");		
		SetModel("models/hl2/eli.mdl");
	}
	else if( !Q_strnicmp( szMapName, "d2", 2 ) )
	{
		// Streetwar scanners are claw scanners
		PrecacheModel("models/hl2/eli.mdl");		
		SetModel("models/hl2/eli.mdl");
	}
	else if( !Q_strnicmp( szMapName, "d3", 2 ) )
	{
		// Streetwar scanners are claw scanners
		PrecacheModel("models/hl2/eli.mdl");
		SetModel("models/hl2/eli.mdl");
	}
	else if( !Q_strnicmp( szMapName, "ep1", 3 ) )
	{
		// Streetwar scanners are claw scanners
		PrecacheModel("models/ep1/eli.mdl");
		SetModel("models/ep1/eli.mdl");
	}
	else if( !Q_strnicmp( szMapName, "ep2", 3 ) )
	{
		// Streetwar scanners are claw scanners
		PrecacheModel("models/ep2/eli.mdl");
		SetModel("models/ep2/eli.mdl");
	}

	BaseClass::Spawn();

	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	// If Eli has a parent, he's currently inside a pod. Prevent him from moving.
	if ( GetMoveParent() )
	{
		SetSolid( SOLID_BBOX );
		AddSolidFlags( FSOLID_NOT_STANDABLE );
		SetMoveType( MOVETYPE_NONE );

		CapabilitiesAdd( bits_CAP_ANIMATEDFACE | bits_CAP_TURN_HEAD );
		CapabilitiesAdd( bits_CAP_FRIENDLY_DMG_IMMUNE );
	}
	else
	{
		SetupWithoutParent();
	}

	AddEFlags( EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL | EFL_NO_PHYSCANNON_INTERACTION );
	SetBloodColor( BLOOD_COLOR_RED );
	m_iHealth			= 8;
	m_flFieldOfView		= 0.5;// indicates the width of this NPC's forward view cone ( as a dotproduct result )
	m_NPCState			= NPC_STATE_NONE;

	NPCInit();
}

//-----------------------------------------------------------------------------
// Precache - precaches all resources this NPC needs
//-----------------------------------------------------------------------------
void CNPC_Eli::Precache()
{
	PrecacheModel( STRING( GetModelName() ) );
	PrecacheModel("models/hl2/eli.mdl");
	PrecacheModel("models/ep1/eli.mdl");
	PrecacheModel("models/ep2/eli.mdl");
	BaseClass::Precache();
}	

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Eli::SetupWithoutParent( void )
{
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );

	CapabilitiesAdd( bits_CAP_MOVE_GROUND | bits_CAP_OPEN_DOORS | bits_CAP_ANIMATEDFACE | bits_CAP_TURN_HEAD );
	CapabilitiesAdd( bits_CAP_FRIENDLY_DMG_IMMUNE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Eli::PrescheduleThink( void )
{
	BaseClass::PrescheduleThink();

	// Figure out if Eli has just been removed from his parent
	if ( GetMoveType() == MOVETYPE_NONE && !GetMoveParent() )
	{
		SetupWithoutParent();
		SetupVPhysicsHull();
	}
}

//-----------------------------------------------------------------------------
// AI Schedules Specific to this NPC
//-----------------------------------------------------------------------------
