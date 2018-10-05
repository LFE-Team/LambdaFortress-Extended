//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_bottle.h"
#include "decals.h"
#include "tf_viewmodel.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_fx.h"
#include "takedamageinfo.h"
#include "tf_gamerules.h"
#include "soundent.h"
#endif

//=============================================================================
//
// Weapon Bottle tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFBottle, DT_TFWeaponBottle )

BEGIN_NETWORK_TABLE( CTFBottle, DT_TFWeaponBottle )
#if defined( CLIENT_DLL )
	RecvPropBool( RECVINFO( m_bBroken ) )
#else
	SendPropBool( SENDINFO( m_bBroken ) )
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFBottle )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_bottle, CTFBottle );
PRECACHE_WEAPON_REGISTER( tf_weapon_bottle );

#define TF_BOTTLE_SWITCHGROUP	1
#define TF_BOTTLE_NOTBROKEN		0
#define TF_BOTTLE_BROKEN		1

#define TF_BOTTLE_MODEL			"models/weapons/c_models/c_bottle/c_bottle.mdl"
#define TF_BOTTLE_MODEL_BROKEN	"models/weapons/c_models/c_bottle/c_bottle_broken.mdl"

//=============================================================================
//
// Weapon Bottle functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFBottle::CTFBottle()
{
}

const char *CTFBottle::GetWorldModel() const
{
	if ( m_bBroken == true )
	{
		return TF_BOTTLE_MODEL_BROKEN;
	}
	
	return BaseClass::GetWorldModel();
}

void CTFBottle::Precache()
{
	BaseClass::Precache();
	
	PrecacheModel( TF_BOTTLE_MODEL );
	PrecacheModel( TF_BOTTLE_MODEL_BROKEN );
}

void CTFBottle::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_bBroken = false;
}

#ifdef CLIENT_DLL
int CTFBottle::GetWorldModelIndex()
{
	if ( !modelinfo ) {
		return BaseClass::GetWorldModelIndex();
	}

	int index = modelinfo->GetModelIndex( m_bBroken == true ? TF_BOTTLE_MODEL_BROKEN : TF_BOTTLE_MODEL );
	m_iWorldModelIndex = index;
	return index;
}
#endif

bool CTFBottle::DefaultDeploy( char *szViewModel, char *szWeaponModel, int iActivity, char *szAnimExt )
{
	bool bRet = BaseClass::DefaultDeploy( szViewModel, szWeaponModel, iActivity, szAnimExt );

	if ( bRet )
	{
		SwitchBodyGroups();
	}

	return bRet;
}

void CTFBottle::SwitchBodyGroups( void )
{
	int iState = 0;

	if ( m_bBroken == true )
	{
		iState = 1;
	}

	SetBodygroup( TF_BOTTLE_SWITCHGROUP, iState );

#ifdef CLIENT_DLL
	C_ViewmodelAttachmentModel *pAttach = GetViewmodelAddon();
	if ( pAttach )
	{
		pAttach->SetModel( iState == 1 ? TF_BOTTLE_MODEL_BROKEN : TF_BOTTLE_MODEL );
	}
#endif
}

void CTFBottle::Smack( void )
{
	BaseClass::Smack();

	if ( ConnectedHit() && IsCurrentAttackACritical() )
	{
		m_bBroken = true;
		SwitchBodyGroups();
	}
}
