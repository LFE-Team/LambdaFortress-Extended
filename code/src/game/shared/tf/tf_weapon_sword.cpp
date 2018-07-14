//====== Copyright © 1996-2018, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Sword(s) from live TF2
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_sword.h"
#include "decals.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Eyelander tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFSword, tf_weapon_sword )

//=============================================================================
//
// Weapon Eyelander functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFSword::CTFSword()
{
}
