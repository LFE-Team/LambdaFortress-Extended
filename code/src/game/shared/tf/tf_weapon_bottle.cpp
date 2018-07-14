//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_bottle.h"
#include "decals.h"

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

#define TF_BOTTLE_SWITCHGROUP 1
#define TF_BOTTLE_NOTBROKEN 0
#define TF_BOTTLE_BROKEN 1

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

void CTFBottle::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_bBroken = false;
}

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

	CTFPlayer *pTFPlayer = ToTFPlayer( GetOwner() );

	if ( pTFPlayer && pTFPlayer->GetActiveWeapon() == this )
	{
		if ( pTFPlayer->GetViewModel() )
		{
			pTFPlayer->GetViewModel()->SetBodygroup( TF_BOTTLE_SWITCHGROUP, iState );
		}
	}
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

//=============================================================================
//
// Weapon Caber tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFStickBomb, DT_TFWeaponStickBomb );

BEGIN_NETWORK_TABLE( CTFStickBomb, DT_TFWeaponStickBomb )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iDetonated ) )
#else
	SendPropInt( SENDINFO( m_iDetonated ) )
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFStickBomb )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_stickbomb, CTFStickBomb );
PRECACHE_WEAPON_REGISTER( tf_weapon_stickbomb );


#define MODEL_NORMAL   "models/weapons/c_models/c_caber/c_caber.mdl"
#define MODEL_EXPLODED "models/weapons/c_models/c_caber/c_caber_exploded.mdl"

#define TF_STICKBOMB_NORMAL    0
#define TF_STICKBOMB_DETONATED 1


CTFStickBomb::CTFStickBomb()
{
	m_iDetonated = TF_STICKBOMB_NORMAL;
}


const char *CTFStickBomb::GetWorldModel() const
{
	if ( m_iDetonated == TF_STICKBOMB_DETONATED )
	{
		return MODEL_EXPLODED;
	}
	
	return BaseClass::GetWorldModel();
}

void CTFStickBomb::Precache()
{
	BaseClass::Precache();
	
	PrecacheModel( MODEL_NORMAL );
	PrecacheModel( MODEL_EXPLODED );
}

void CTFStickBomb::Smack()
{
	BaseClass::Smack();
	
	if (m_iDetonated == TF_STICKBOMB_NORMAL && ConnectedHit()) 
	{
		m_iDetonated = TF_STICKBOMB_DETONATED;
		
		SwitchBodyGroups();
		
#ifdef GAME_DLL
		CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
		if ( pOwner )
		{
			// TF2 does these things and doesn't use the results:
			// calls pOwner->EyeAngles() and then AngleVectors()
			// calls this->GetSwingRange()
			
			// my bet: they meant to multiply the fwd vector by the swing range
			// and then use that for the damage force, but they typo'd it and
			// just reused the shoot position instead
			
			Vector where = pOwner->Weapon_ShootPosition();
			
			CPVSFilter filter(where);
			TE_TFExplosion(filter, 0.0f, where, Vector(0.0f, 0.0f, 1.0f),
				TF_WEAPON_GRENADELAUNCHER, ENTINDEX(pOwner));

			CSoundEnt::InsertSound ( SOUND_DANGER, GetAbsOrigin(), BASEGRENADE_EXPLOSION_VOLUME, 3.0 );

			/* why is the damage force vector set to Weapon_ShootPosition()?
			 * I dunno! */
			CTakeDamageInfo dmginfo(pOwner, pOwner, this, where, where, 75.0f,
				DMG_BLAST | DMG_CRITICAL | (this->IsCurrentAttackACrit() ? DMG_USEDISTANCEMOD : 0),
				TF_DMG_CUSTOM_STICKBOMB_EXPLOSION, &where);
			
			CTFRadiusDamageInfo radius;
			radius.info       = dmginfo;
			radius.m_vecSrc   = where;
			radius.m_flRadius = 100.0f;
			TFGameRules()->RadiusDamage(radius);
		}
#endif
	}
}

void CTFStickBomb::SwitchBodyGroups()
{
#ifdef CLIENT_DLL
	C_ViewmodelAttachmentModel *pAttach = GetViewmodelAddon();
	if ( pAttach )
	{
		int bodygroup = pAttach->FindBodygroupByName( "broken" );
		pAttach->SetBodygroup( bodygroup, m_iDetonated );
	}
#endif
}

void CTFStickBomb::WeaponRegenerate()
{
	BaseClass::WeaponRegenerate();
	
	m_iDetonated = TF_STICKBOMB_NORMAL;
	
#ifdef CLIENT_DLL
	SetNextClientThink( gpGlobals->curtime + 0.1f );
#endif
}

void CTFStickBomb::WeaponReset()
{
	BaseClass::WeaponReset();
	
	m_iDetonated = TF_STICKBOMB_NORMAL;
	
#ifdef CLIENT_DLL
	SetNextClientThink( gpGlobals->curtime + 0.1f );
#endif
}


#ifdef CLIENT_DLL
int C_TFStickBomb::GetWorldModelIndex()
{
	if ( modelinfo ) 
	{
		return BaseClass::GetWorldModelIndex();
	}
	
	int index = modelinfo->GetModelIndex( m_iDetonated == TF_STICKBOMB_DETONATED ? MODEL_EXPLODED : MODEL_NORMAL );
	m_iWorldModelIndex = index;
	return index;
}

void C_TFStickBomb::ClientThink( void )
{
	if ( WeaponState() == WEAPON_IS_ACTIVE )
	{
		SwitchBodyGroups();
		SetNextClientThink( CLIENT_THINK_NEVER );
		return;
	}

	// Keep checking until the weapon is deployed
	SetNextClientThink( gpGlobals->curtime + 0.1f );
}
#endif