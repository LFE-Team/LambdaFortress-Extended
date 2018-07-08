//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_bonesaw.h"

#ifdef CLIENT_DLL
#include "tf_weapon_medigun.h"
#include "c_tf_player.h"
#include "tf_viewmodel.h"
#endif

//=============================================================================
//
// Weapon Bonesaw tables.
//

CREATE_SIMPLE_WEAPON_TABLE( TFBonesaw, tf_weapon_bonesaw )

void CTFBonesaw::SecondaryAttack( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;
/*
	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType != 0 )
	{
#ifdef GAME_DLL
		pOwner->Taunt();
#endif
	}
*/
}

#ifdef CLIENT_DLL
void CTFBonesaw::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	UpdateChargePoseParam();
}

bool CTFBonesaw::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		UpdateChargePoseParam();
		return true;
	}

	return false;
}

void CTFBonesaw::UpdateChargePoseParam( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType != 0 )
	{
		CWeaponMedigun *pMedigun = pOwner->GetMedigun();
		if ( pMedigun )
		{
			SetPoseParameter( "syringe_charge_level", pMedigun->GetChargeLevel() );

			CBaseViewModel *vm = pOwner->GetViewModel( m_nViewModelIndex );
			if ( vm )
				vm->SetPoseParameter( "syringe_charge_level", pMedigun->GetChargeLevel() );
		}
	}
}

#endif
