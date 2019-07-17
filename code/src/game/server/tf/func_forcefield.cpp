//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "func_forcefield.h"
#include "func_no_build.h"
#include "tf_team.h"
#include "ndebugoverlay.h"
#include "tf_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_AUTO_LIST(IFuncForceFieldAutoList);

//-----------------------------------------------------------------------------
// Purpose: Check whether the line between two vectors crosses an respawn room visualizer
//-----------------------------------------------------------------------------
bool PointsCrossForceField( const Vector &vecStart, const Vector &vecEnd, int iTeam )
{
	Ray_t ray;
	ray.Init(vecStart, vecEnd);

	for (int i = 0; i < IFuncForceFieldAutoList::AutoList().Count(); ++i)
	{
		CFuncForceField *pVisualizer = static_cast<CFuncForceField *>( IFuncForceFieldAutoList::AutoList()[i] );
		
		if (pVisualizer->GetTeamNumber() != iTeam || !iTeam)
		{
			trace_t tr;

			enginetrace->ClipRayToEntity(ray, MASK_ALL, pVisualizer, &tr);

			if (tr.fraction < 1.f)
				return true;
		}
	}

	return false;
}

//===========================================================================================================

LINK_ENTITY_TO_CLASS( func_forcefield, CFuncForceField );

BEGIN_DATADESC( CFuncForceField )
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CFuncForceField, DT_FuncForceField )
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncForceField::Spawn( void )
{
	BaseClass::Spawn();

	SetActive( true );

	SetCollisionGroup( TFCOLLISION_GROUP_RESPAWNROOMS );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFuncForceField::UpdateTransmitState()
{
	//return SetTransmitState( FL_EDICT_FULLCHECK );

	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose: Only transmit this entity to clients that aren't in our team
//-----------------------------------------------------------------------------
int CFuncForceField::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	/*
	// Respawn rooms are open in win state
	if ( TFGameRules()->State_Get() != GR_STATE_TEAM_WIN && GetTeamNumber() != TEAM_UNASSIGNED )
	{
		// Only transmit to enemy players
		CBaseEntity *pRecipientEntity = CBaseEntity::Instance( pInfo->m_pClientEnt );
		if ( pRecipientEntity->GetTeamNumber() > LAST_SHARED_TEAM && !InSameTeam(pRecipientEntity) )
			return FL_EDICT_ALWAYS;
	}
	*/
	return FL_EDICT_DONTSEND;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFuncForceField::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	// Respawn rooms are open in win state
	//if ( TFGameRules()->State_Get() == GR_STATE_TEAM_WIN )
	//	return false;

	if ( GetTeamNumber() == TEAM_UNASSIGNED )
		return false;

	if ( collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		switch( GetTeamNumber() )
		{
		case TF_TEAM_BLUE:
			if ( !(contentsMask & CONTENTS_BLUETEAM) )
				return false;
			break;

		case TF_TEAM_RED:
			if ( !(contentsMask & CONTENTS_REDTEAM) )
				return false;
			break;

		case TF_TEAM_GREEN:
			if ( !(contentsMask & CONTENTS_GREENTEAM ) )
				return false;
			break;

		case TF_TEAM_YELLOW:
			if ( !(contentsMask & CONTENTS_YELLOWTEAM ) )
				return false;
			break;
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncForceField::SetActive( bool bActive )
{
	if ( bActive )
	{
		// We're a trigger, but we want to be solid. Out ShouldCollide() will make
		// us non-solid to members of the team that spawns here.
		RemoveSolidFlags( FSOLID_TRIGGER );
		RemoveSolidFlags( FSOLID_NOT_SOLID );	
	}
	else
	{
		AddSolidFlags( FSOLID_NOT_SOLID );
		AddSolidFlags( FSOLID_TRIGGER );	
	}
}
