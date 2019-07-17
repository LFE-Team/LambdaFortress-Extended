//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_invis.h"
#include "in_buttons.h"

#if !defined( CLIENT_DLL )
	#include "vguiscreen.h"
	#include "tf_player.h"
#else
	#include "c_tf_player.h"
#endif

extern ConVar tf_spy_invis_unstealth_time;

//=============================================================================
//
// TFWeaponBase Melee tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFWeaponInvis, tf_weapon_invis )

//-----------------------------------------------------------------------------
// Purpose: Use the offhand view model
//-----------------------------------------------------------------------------
void CTFWeaponInvis::Spawn( void )
{
	BaseClass::Spawn();

	SetViewModelIndex( 1 );
}

//-----------------------------------------------------------------------------
// Purpose: Clear out the view model when we hide
//-----------------------------------------------------------------------------
void CTFWeaponInvis::HideThink( void )
{ 
	SetWeaponVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Show/hide weapon and corresponding view model if any
// Input  : visible - 
//-----------------------------------------------------------------------------
void CTFWeaponInvis::SetWeaponVisible( bool visible )
{
	CBaseViewModel *vm = NULL;

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner )
	{
		vm = pOwner->GetViewModel( m_nViewModelIndex );
	}

	if ( visible )
	{
		RemoveEffects( EF_NODRAW );
		if ( vm )
		{
			vm->RemoveEffects( EF_NODRAW );
		}
	}
	else
	{
		AddEffects( EF_NODRAW );
		if ( vm )
		{
			vm->AddEffects( EF_NODRAW );
		}
	}
}

bool CTFWeaponInvis::Deploy( void )
{
	bool b = BaseClass::Deploy();

	SetWeaponIdleTime( gpGlobals->curtime + 1.5 );

	return b;
}

bool CTFWeaponInvis::HasFeignDeath( void )
{
	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType == 1 )
		return true;

	return false;
}

bool CTFWeaponInvis::HasMotionCloak( void )
{
	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType == 2 )
		return true;

	return false;
}

bool CTFWeaponInvis::Holster( CBaseCombatWeapon *pSwitchingTo )
{ 
	bool bHolster = BaseClass::Holster( pSwitchingTo );

	// far in the future
	if ( HasFeignDeath() )
	{
		SetWeaponIdleTime( gpGlobals->curtime + 99999 );
	}
	else
	{
		SetWeaponIdleTime( gpGlobals->curtime + 10 );
	}

	return bHolster;
}

void CTFWeaponInvis::SecondaryAttack( void )
{
	// do nothing
}

void CTFWeaponInvis::ItemBusyFrame( void )
{
	// do nothing
}

const char* CTFWeaponInvis::GetEffectLabelText( void ) 
{
	if ( HasFeignDeath() )
		return "#TF_Feign"; 
	else if ( HasMotionCloak() )
		return "#TF_CloakDagger"; 
	else
		return "#TF_Cloak"; 
}

float CTFWeaponInvis::GetEffectBarProgress( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner )
	{
		return ( pOwner->m_Shared.GetSpyCloakMeter() / 100.0f );
	}

	return 1.0f;
}

#ifndef CLIENT_DLL

void CTFWeaponInvis::GetControlPanelInfo( int nPanelIndex, const char *&pPanelName )
{
	pPanelName = "pda_panel_spy_invis";
}

#endif