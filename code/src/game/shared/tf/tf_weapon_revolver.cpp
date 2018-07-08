//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_revolver.h"
#include "tf_fx_shared.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Revolver tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFRevolver, tf_weapon_revolver )
CREATE_SIMPLE_WEAPON_TABLE( TFRevolver_Secondary, tf_weapon_revolver_secondary ) // engineer, anyone?

//=============================================================================
//
// Weapon Revolver functions.
//
/*
acttable_t CTFRevolver::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_SECONDARY2, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_SECONDARY2, false },
	{ ACT_MP_RUN, ACT_MP_RUN_SECONDARY2, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_SECONDARY2, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_SECONDARY2, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_SECONDARY2, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_SECONDARY2, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_SECONDARY2, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_SECONDARY2, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_SECONDARY2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_SECONDARY2, false },
};

IMPLEMENT_ACTTABLE( CTFRevolver );
*/

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFRevolver::CanFireCriticalShot( bool bIsHeadshot )
{
	// can only fire a crit shot if this is a headshot
	if ( !bIsHeadshot )
		return false;

	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType == 1 )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFRevolver::GetDamageType( void ) const
{
	int iDmgType = BaseClass::GetDamageType();

	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );

	if ( iType == 1 )
	{
		iDmgType |= DMG_USE_HITLOCATIONS;
	}
	else
	{
		iDmgType |= DMG_BULLET;	
	}

	return iDmgType;
}