//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Mannpower powerups 
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_rune.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFRune )
	DEFINE_KEYFIELD( m_strPickupSound, FIELD_SOUNDNAME, "PickupSound" ),
END_DATADESC()

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFRune::CTFRune()
{
}

CTFRune *CTFRune::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszClassname, float flDuration )
{
	CTFRune *pPowerup = dynamic_cast<CTFRune *>( CBaseEntity::CreateNoSpawn( pszClassname, vecOrigin, vecAngles, pOwner ) );

	if ( pPowerup )
	{
		pPowerup->SetEffectDuration( flDuration );
		pPowerup->AddSpawnFlags( SF_NORESPAWN );

		DispatchSpawn( pPowerup );

		pPowerup->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );

		pPowerup->SetThink( &CBaseEntity::SUB_Remove );
		pPowerup->SetNextThink( gpGlobals->curtime + 30.0f );
	}

	return pPowerup;
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFRune::Precache( void )
{
	UTIL_ValidateSoundName( m_strPickupSound, GetDefaultPickupSound() );
	if ( GetModelName() == NULL_STRING )
		SetModelName( AllocPooledString( GetDefaultPowerupModel() ) );

	PrecacheModel( STRING( GetModelName() ) );
	PrecacheScriptSound( STRING( m_strPickupSound ) );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFRune::Spawn( void )
{
	Precache();
	SetModel( STRING( GetModelName() ) );

	BaseClass::Spawn();

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	UTIL_DropToFloor( this, MASK_SOLID );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRune::Materialize( void )
{
	BaseClass::Materialize();

	if ( !IsDisabled() )
	{
		EmitSound( "Item.Materialize" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFRune::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( pTFPlayer && ValidTouch( pPlayer ) )
	{
		// Add the condition and duration from derived classes
		pTFPlayer->m_Shared.AddCond( GetCondition(), GetEffectDuration() );

		// Give full health
		int iHealthRestored = pTFPlayer->TakeHealth( pTFPlayer->GetMaxHealth(), DMG_GENERIC );

		if ( iHealthRestored )
		{
			IGameEvent *event_healonhit = gameeventmanager->CreateEvent( "player_healonhit" );
			if ( event_healonhit )
			{
				event_healonhit->SetInt( "amount", iHealthRestored );
				event_healonhit->SetInt( "entindex", pTFPlayer->entindex() );

				gameeventmanager->FireEvent( event_healonhit );
			}
		}

		CSingleUserRecipientFilter user( pTFPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
		MessageEnd();

		if ( m_strPickupSound != NULL_STRING )
			pTFPlayer->EmitSound( STRING( m_strPickupSound ) );

		bSuccess = true;
	}

	return bSuccess;
}

//=============================================================================

BEGIN_DATADESC( CTFRuneTemp )
	DEFINE_KEYFIELD( m_flRespawnTime, FIELD_FLOAT, "RespawnTime" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_rune_temp, CTFRuneTemp );

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFRuneTemp::CTFRuneTemp()
{	
	m_iPowerupCondition = TF_COND_RUNE_STRENGTH;
	m_flEffectDuration = 30.0f;
	m_flRespawnTime = 60.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFRuneTemp::Spawn( void )
{
	Precache();
	SetModel( STRING( GetModelName() ) );
	AddEffects( EF_ITEM_BLINK );

	BaseClass::BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:  Override to get rid of EF_NODRAW
//-----------------------------------------------------------------------------
CBaseEntity* CTFRuneTemp::Respawn( void )
{
	CBaseEntity *pRet = BaseClass::Respawn();

	RemoveEffects( EF_NODRAW );
	RemoveEffects( EF_ITEM_BLINK );
	SetRenderColorA( 80 );

	m_flRespawnAtTime = GetNextThink();

	return pRet;
}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
float CTFRuneTemp::GetRespawnDelay( void )
{
	return m_flRespawnTime;
}

//=============================================================================

BEGIN_DATADESC( CTFRuneTempCrit )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_crit, CTFRuneTempCrit );

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFRuneTempCrit::CTFRuneTempCrit()
{	
	m_flEffectDuration = 30.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFRuneTempCrit::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( pTFPlayer && ValidTouch( pPlayer ) )
	{
		// Add the condition and duration from derived classes
		pTFPlayer->m_Shared.AddCond( GetCondition(), GetEffectDuration() );
		// Also add this too
		pTFPlayer->m_Shared.AddCond( TF_COND_RUNE_IMBALANCE, GetEffectDuration() );

		// Give full health
		int iHealthRestored = pTFPlayer->TakeHealth( pTFPlayer->GetMaxHealth(), DMG_GENERIC );

		if ( iHealthRestored )
		{
			IGameEvent *event_healonhit = gameeventmanager->CreateEvent( "player_healonhit" );
			if ( event_healonhit )
			{
				event_healonhit->SetInt( "amount", iHealthRestored );
				event_healonhit->SetInt( "entindex", pTFPlayer->entindex() );

				gameeventmanager->FireEvent( event_healonhit );
			}
		}

		CSingleUserRecipientFilter user( pTFPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
		MessageEnd();

		if ( m_strPickupSound != NULL_STRING )
			pTFPlayer->EmitSound( STRING( m_strPickupSound ) );

		bSuccess = true;
	}

	return bSuccess;
}

//=============================================================================

BEGIN_DATADESC( CTFRuneTempUber )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_uber, CTFRuneTempUber );

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFRuneTempUber::CTFRuneTempUber()
{	
	m_flEffectDuration = 30.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFRuneTempUber::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( pTFPlayer && ValidTouch( pPlayer ) )
	{
		// Add the condition and duration from derived classes
		pTFPlayer->m_Shared.AddCond( GetCondition(), GetEffectDuration() );
		// Also add this too
		pTFPlayer->m_Shared.AddCond( TF_COND_RUNE_IMBALANCE, GetEffectDuration() );

		// Give full health
		int iHealthRestored = pTFPlayer->TakeHealth( pTFPlayer->GetMaxHealth(), DMG_GENERIC );

		if ( iHealthRestored )
		{
			IGameEvent *event_healonhit = gameeventmanager->CreateEvent( "player_healonhit" );
			if ( event_healonhit )
			{
				event_healonhit->SetInt( "amount", iHealthRestored );
				event_healonhit->SetInt( "entindex", pTFPlayer->entindex() );

				gameeventmanager->FireEvent( event_healonhit );
			}
		}

		CSingleUserRecipientFilter user( pTFPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
		MessageEnd();

		if ( m_strPickupSound != NULL_STRING )
			pTFPlayer->EmitSound( STRING( m_strPickupSound ) );

		bSuccess = true;
	}

	return bSuccess;
}

//=============================================================================

BEGIN_DATADESC( CTFRuneCustom )

	DEFINE_KEYFIELD( m_iPowerupCondition, FIELD_INTEGER, "condition" ),
	DEFINE_KEYFIELD( m_flEffectDuration, FIELD_FLOAT, "EffectDuration" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_rune_custom, CTFRuneCustom );



//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFRuneCustom::CTFRuneCustom()
{	
	m_iPowerupCondition = TF_COND_INVULNERABLE;
	m_flEffectDuration = 15.0f;
}