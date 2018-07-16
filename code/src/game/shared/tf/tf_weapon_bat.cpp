//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_bat.h"
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
// Weapon Bat tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFBat, tf_weapon_bat )

//=============================================================================
//
// Weapon Bat functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFBat::CTFBat()
{
}

//=============================================================================
//
// Weapon Bat Wood tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFBat_Wood, tf_weapon_bat_wood )
/*
//=============================================================================
//
// Weapon Bat Giftwarp tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFBat_Giftwarp, tf_weapon_bat_giftwarp )
*/
//=============================================================================
//
// Weapon Bat Fish tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFBat_Fish, tf_weapon_bat_fish )