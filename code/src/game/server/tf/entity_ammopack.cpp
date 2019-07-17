//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_ammopack.h"
#include "tf_weapon_invis.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

LINK_ENTITY_TO_CLASS( item_ammopack_full, CAmmoPack );
LINK_ENTITY_TO_CLASS( item_ammopack_small, CAmmoPackSmall );
LINK_ENTITY_TO_CLASS( item_ammopack_medium, CAmmoPackMedium );

//=============================================================================
//
// CTF AmmoPack functions.
//

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the ammopack
//-----------------------------------------------------------------------------
void CAmmoPack::Spawn( void )
{
	Precache();
	SetModel( GetPowerupModel() );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the ammopack
//-----------------------------------------------------------------------------
void CAmmoPack::Precache( void )
{
	PrecacheModel( GetPowerupModel() );
	PrecacheScriptSound( TF_AMMOPACK_PICKUP_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the ammopack
//-----------------------------------------------------------------------------
bool CAmmoPack::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	if ( ValidTouch( pPlayer ) )
	{
		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		if ( !pTFPlayer )
			return false;

		int iMaxPrimary = pTFPlayer->GetMaxAmmo( TF_AMMO_PRIMARY );
		if ( pPlayer->GiveAmmo( ceil( iMaxPrimary * PackRatios[GetPowerupSize()] ), TF_AMMO_PRIMARY, true ) )
		{
			bSuccess = true;
		}

		int iMaxSecondary = pTFPlayer->GetMaxAmmo( TF_AMMO_SECONDARY );
		if ( pPlayer->GiveAmmo( ceil( iMaxSecondary * PackRatios[GetPowerupSize()] ), TF_AMMO_SECONDARY, true ) )
		{
			bSuccess = true;
		}

		int iMaxMetal = pTFPlayer->GetMaxAmmo( TF_AMMO_METAL );
		if ( pPlayer->GiveAmmo( ceil( iMaxMetal * PackRatios[GetPowerupSize()] ), TF_AMMO_METAL, true ) )
		{
			bSuccess = true;
		}

		int iNoItems = 0;
		int iCloakWhenCloaked = 0;
		float flReducedCloakFromAmmo = 0;
		CTFWeaponInvis *pInvis = static_cast<CTFWeaponInvis*>( pTFPlayer->Weapon_OwnsThisID( TF_WEAPON_INVIS ) );
		CALL_ATTRIB_HOOK_INT_ON_OTHER( pInvis, iNoItems, mod_cloak_no_regen_from_items );
		CALL_ATTRIB_HOOK_INT_ON_OTHER( pInvis, iCloakWhenCloaked, NoCloakWhenCloaked );
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pInvis, flReducedCloakFromAmmo, ReducedCloakFromAmmo );

		float flCloak = pTFPlayer->m_Shared.GetSpyCloakMeter();
		if ( flCloak < 100.0f )
		{
			if ( !pTFPlayer->m_Shared.InCond( TF_COND_STEALTHED ) && iCloakWhenCloaked != 1 && iNoItems != 1 )
			{
				pTFPlayer->m_Shared.SetSpyCloakMeter( min( 100.0f, flCloak + min( 100.0f, flReducedCloakFromAmmo ) * PackRatios[GetPowerupSize()] ) );
				bSuccess = true;
			}
		}

		// did we give them anything?
		if ( bSuccess )
		{
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), TF_AMMOPACK_PICKUP_SOUND );
		}
	}

	return bSuccess;
}
