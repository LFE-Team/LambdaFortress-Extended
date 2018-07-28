//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_lunchbox.h"

#ifdef GAME_DLL
#include "tf_player.h"
#include "tf_powerup.h"
#include "entity_healthkit.h"
#else
#include "c_tf_player.h"
#include "c_tf_viewmodeladdon.h"
#endif

CREATE_SIMPLE_WEAPON_TABLE( TFLunchBox, tf_weapon_lunchbox )

#define SANDVICH_BODYGROUP_BITE 0
#define SANDVICH_STATE_BITTEN 1
#define SANDVICH_STATE_NORMAL 0
#define TF_SANDVICH_PLATE_MODEL "models/items/plate.mdl"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox::PrimaryAttack( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	if (pOwner->GetGroundEntity() != NULL)
	{
#ifdef GAME_DLL
		pOwner->Taunt();

		//ApplyBiteEffects();
		if (pOwner->GetHealth() < 300)
		{
			if (HasAmmo())
			{
				pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
			}
			pOwner->SwitchToNextBestWeapon(this);
			StartEffectBarRegen();
		}	
#else
		C_ViewmodelAttachmentModel *pAttach = GetViewmodelAddon();
		if ( pAttach )
		{
			pAttach->SetBodygroup( SANDVICH_BODYGROUP_BITE, SANDVICH_STATE_BITTEN );
		}
#endif
	}

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox::SecondaryAttack( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	if ( !CanAttack() )
		return;

#ifdef GAME_DLL
	// Remove the previous dropped lunch box.
	if ( m_hDroppedLunch.Get() )
	{
		UTIL_Remove( m_hDroppedLunch.Get() );
		m_hDroppedLunch = NULL;
	}

	// Throw a sandvich plate down on the ground.
	Vector vecSrc, vecThrow;
	QAngle angThrow;
	vecSrc = pOwner->EyePosition();

	// A bit below the eye position.
	vecSrc.z -= 8.0f;

	CHealthKitMedium *pPowerup = static_cast<CHealthKitMedium *>( CBaseEntity::Create( "item_healthkit_medium", vecSrc, vec3_angle, pOwner ) );
	if ( !pPowerup )
		return;

	pPowerup->SetModel( TF_SANDVICH_PLATE_MODEL );
	UTIL_SetSize( pPowerup, -Vector( 17, 17, 10 ), Vector( 17, 17, 10 ) );

	// Throw it down.
	angThrow = pOwner->EyeAngles();
	angThrow[PITCH] -= 10.0f;
	AngleVectors( angThrow, &vecThrow );
	vecThrow *= 500;

	pPowerup->DropSingleInstance( vecThrow, pOwner, 0.3f, 0.1f );

	m_hDroppedLunch = pPowerup;
#endif

	// Switch away from it immediately, don't want it to stick around.
	if (HasAmmo())
	{
		pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
	}
	pOwner->SwitchToNextBestWeapon( this );

	StartEffectBarRegen();

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
}

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox::Precache( void )
{
	UTIL_PrecacheOther( "item_healthkit_medium" );
	PrecacheModel( TF_SANDVICH_PLATE_MODEL );

	PrecacheParticleSystem( "sandwich_fx" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox::ApplyBiteEffects( void )
{
	// Heal 80 HP per second for a total 300 HP because this is sandvich not banana.
	CTFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner )
	{
		pOwner->TakeHealth( 80, DMG_GENERIC );
		//pOwner->TakeHealth( 120, DMG_GENERIC );
		//pOwner->SpeakConceptIfAllowed( MP_CONCEPT_ATE_FOOD );
	}
#ifdef CLIENT_DLL
	ParticleProp()->Create( "sandwich_fx", PATTACH_ABSORIGIN_FOLLOW );
#endif
}

#endif

//=============================================================================
//
// Weapon BONK! tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFLunchBox_Drink, tf_weapon_lunchbox_drink )

//-----------------------------------------------------------------------------
// Purpose: Show the drink splash when we deploy
//-----------------------------------------------------------------------------
bool CTFLunchBox_Drink::Deploy( void )
{
#ifdef CLIENT_DLL
	int iType = 0;
	CALL_ATTRIB_HOOK_INT( iType, set_weapon_mode );
	if ( iType == 0 ) // it's a bonk
	{
		ParticleProp()->Create( "energydrink_splash", PATTACH_ABSORIGIN_FOLLOW );
	}
	else if ( iType == 1 )
	{ // it's a bonk again
		ParticleProp()->Create( "energydrink_splash", PATTACH_ABSORIGIN_FOLLOW );
	} // it's a cola
	else if ( iType == 2 )
	{
		ParticleProp()->Create( "energydrink_cola_splash", PATTACH_ABSORIGIN_FOLLOW );
	}
#endif

	return BaseClass::BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox_Drink::PrimaryAttack( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
	{
		return;
	}

	if (pOwner->GetGroundEntity() != NULL)
	{
#ifdef GAME_DLL
		pOwner->Taunt();
#endif

		// Switch away from it immediately, don't want it to stick around.
		if (HasAmmo())
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		pOwner->SwitchToNextBestWeapon(this);

		StartEffectBarRegen();

		m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
	}
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFLunchBox_Drink::Precache( void )
{
	PrecacheParticleSystem( "energydrink_splash" );
	PrecacheParticleSystem( "energydrink_cola_splash" );
	BaseClass::BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*void CTFLunchBox_Drink::ApplyBiteEffects( bool bHurt )
{
}*/
#endif