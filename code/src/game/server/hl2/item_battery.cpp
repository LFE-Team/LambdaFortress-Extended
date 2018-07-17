//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Handling for the suit batteries.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hl2_player.h"
#include "basecombatweapon.h"
#include "gamerules.h"
#include "items.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef TF_CLASSIC
ConVar sk_battery( "sk_battery","15" );
#include "entity_ammopack.h"
#include "entity_healthkit.h"
#include "tf_player.h"
#include "tf_gamerules.h"

#define TF_AMMOPACK_PICKUP_SOUND	"AmmoPack.Touch"

bool ITEM_GiveTFAmmoHealth( CBasePlayer *pPlayer, float flCount, bool bSuppressSound = true)
{
	bool bSuccess = false;
	int iHealthRestored = 0;
	int iHealthToAdd = pPlayer->GetMaxHealth() * PackRatios[POWERUP_TINY];

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( !pTFPlayer )
		return false;

	int iMaxPrimary = pTFPlayer->GetMaxAmmo( TF_AMMO_PRIMARY );
	if ( pPlayer->GiveAmmo( ceil( iMaxPrimary * PackRatios[POWERUP_SMALL] ), TF_AMMO_PRIMARY, true ) )
	{
		bSuccess = true;
	}

	int iMaxSecondary = pTFPlayer->GetMaxAmmo( TF_AMMO_SECONDARY );
	if ( pPlayer->GiveAmmo( ceil( iMaxSecondary * PackRatios[POWERUP_SMALL] ), TF_AMMO_SECONDARY, true ) )
	{
		bSuccess = true;
	}

	int iMaxMetal = pTFPlayer->GetMaxAmmo( TF_AMMO_METAL );
	if ( pPlayer->GiveAmmo( ceil( iMaxMetal * PackRatios[POWERUP_SMALL] ), TF_AMMO_METAL, true ) )
	{
		bSuccess = true;
	}

	float flCloak = pTFPlayer->m_Shared.GetSpyCloakMeter();
	if ( flCloak < 100.0f )
	{
		pTFPlayer->m_Shared.SetSpyCloakMeter( min( 100.0f, flCloak + 100.0f * PackRatios[POWERUP_SMALL] ) );
		bSuccess = true;
	}

	iHealthToAdd = clamp( iHealthToAdd, 0, pTFPlayer->m_Shared.GetMaxBuffedHealth() - pTFPlayer->GetHealth() );
	iHealthRestored = pPlayer->TakeHealth( iHealthToAdd, DMG_IGNORE_MAXHEALTH );

	if ( iHealthRestored )
		bSuccess = true;

	return bSuccess;
}
#endif

class CItemBattery : public CItem
{
public:
	DECLARE_CLASS( CItemBattery, CItem );

	void Spawn( void )
	{ 
		Precache( );
		#ifdef TF_CLASSIC
		SetModel( STRING( GetModelName() ) );
		#else
		SetModel( "models/items/battery.mdl" );
		#endif
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		#ifdef TF_CLASSIC
		if (TFGameRules()->IsInHL1Map())
			SetModelName( AllocPooledString( "models/w_battery.mdl") );	//If we're in HL1
		else if ( CBaseEntity::GetModelName() == NULL_STRING )
			SetModelName( AllocPooledString( "models/items/battery.mdl") );
		else
			SetModelName( CBaseEntity::GetModelName() );

		PrecacheModel( STRING( GetModelName() ) );

		#else
		PrecacheModel ( "models/items/battery.mdl" );
		#endif
		PrecacheScriptSound( "ItemBattery.Touch" );

	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
#ifdef TF_CLASSIC
		if ( ITEM_GiveTFAmmoHealth( pPlayer, PackRatios[POWERUP_TINY] ) )
		{
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), TF_AMMOPACK_PICKUP_SOUND );
		}
		return true;
#else
		CHL2_Player *pHL2Player = dynamic_cast<CHL2_Player *>( pPlayer );
		return ( pHL2Player && pHL2Player->ApplyBattery() );
#endif
	}
};

LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);
PRECACHE_REGISTER(item_battery);

