//=========== Copyright © 2018, LFE-Team, Not All rights reserved. ============
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_buff_item.h"
#include "tf_fx_shared.h"
#include "in_buttons.h"
#include "datacache/imdlcache.h"
#include "effect_dispatch_data.h"
#include "engine/IEngineSound.h"
#include "tf_gamerules.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "particles_new.h"
// Server specific.
#else
#include "tf_player.h"
#include "ai_basenpc.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "tf_fx.h"
#include "tf_gamestats.h"
#endif

#define TF_BUFF_RADIUS 450.0f

//=============================================================================
//
// Weapon BUFF item tables.
//


IMPLEMENT_NETWORKCLASS_ALIASED( TFBuffItem, DT_WeaponBuffItem )

BEGIN_NETWORK_TABLE( CTFBuffItem, DT_WeaponBuffItem )
#ifdef CLIENT_DLL
#else
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFBuffItem )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_buff_item, CTFBuffItem );
PRECACHE_WEAPON_REGISTER( tf_weapon_buff_item );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFBuffItem )
END_DATADESC()
#endif

CTFBuffItem::CTFBuffItem()
{
	m_flEffectBarRegenTime = 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBuffItem::Precache( void )
{
	BaseClass::Precache();
	PrecacheTeamParticles( "soldierbuff_%s_buffed" );
	PrecacheParticleSystem( "soldierbuff_mvm" );
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBuffItem::PrimaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	if ( !CanAttack() )
		return;

	//if ( m_flEffectBarRegenTime != InternalGetEffectBarRechargeTime() )
	//	return;

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pOwner->SetAnimation( PLAYER_ATTACK1 );
	pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	if ( GetBuffType() == 1 )
	{
		if ( pOwner->GetTeamNumber() == TF_TEAM_RED )
			pOwner->EmitSound( "Weapon_BuffBanner.HornRed" );
		else if ( pOwner->GetTeamNumber() == TF_TEAM_BLUE )
			pOwner->EmitSound( "Weapon_BuffBanner.HornBlue" );
	}
	else if ( GetBuffType() == 2 )
	{
		if ( pOwner->GetTeamNumber() == TF_TEAM_RED )
			pOwner->EmitSound( "Weapon_BattalionsBackup.HornRed" );
		else if ( pOwner->GetTeamNumber() == TF_TEAM_BLUE )
			pOwner->EmitSound( "Weapon_BattalionsBackup.HornBlue" );
	}
	else if ( GetBuffType() == 3 )
	{
		pOwner->EmitSound( "Samurai.Conch" );
	}

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
	if ( m_flTimeWeaponIdle > gpGlobals->curtime )
	{
#ifdef GAME_DLL 
		pOwner->SpeakWeaponFire( MP_CONCEPT_PLAYER_BATTLECRY );
		CTF_GameStats.Event_PlayerFiredWeapon( pOwner, false );
#endif
		pOwner->SwitchToNextBestWeapon( this );
		pOwner->EmitSound( "Weapon_BuffBanner.Flag" );

		IGameEvent *event_buff = gameeventmanager->CreateEvent( "deploy_buff_banner" );
		if ( event_buff )
		{
			event_buff->SetInt( "buff_type", GetBuffType() );
			event_buff->SetInt( "buff_owner", pOwner->entindex() );
 			gameeventmanager->FireEvent( event_buff );
		}
#ifdef GAME_DLL 
		CBaseEntity *pEntity = NULL;
		Vector vecOrigin = pOwner->GetAbsOrigin();

		for ( CEntitySphereQuery sphere( vecOrigin, TF_BUFF_RADIUS ); ( pEntity = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
		{
			if ( !pEntity )
				continue;

			Vector vecHitPoint;
			pEntity->CollisionProp()->CalcNearestPoint( vecOrigin, &vecHitPoint );
			Vector vecDir = vecHitPoint - vecOrigin;

			CTFPlayer *pPlayer = ToTFPlayer( pEntity );
			CAI_BaseNPC *pNPC = pEntity->MyNPCPointer();

			if ( vecDir.LengthSqr() < ( TF_BUFF_RADIUS * TF_BUFF_RADIUS ) )
			{
				if ( pPlayer )
				{
					if ( pPlayer->InSameTeam( pOwner ) )
					{
						if ( GetBuffType() == 1 )
							pPlayer->m_Shared.AddCond( TF_COND_OFFENSEBUFF );
						else if ( GetBuffType() == 2 )
							pPlayer->m_Shared.AddCond( TF_COND_DEFENSEBUFF );
						else if ( GetBuffType() == 3 )
							pPlayer->m_Shared.AddCond( TF_COND_REGENONDAMAGEBUFF );
					}
				}

				if ( pNPC )
				{
					if ( pNPC->InSameTeam( pOwner ) )
					{
						if ( GetBuffType() == 1 )
							pNPC->AddCond( TF_COND_OFFENSEBUFF );
						else if ( GetBuffType() == 2 )
							pNPC->AddCond( TF_COND_DEFENSEBUFF );
						else if ( GetBuffType() == 3 )
							pNPC->AddCond( TF_COND_REGENONDAMAGEBUFF );
					}
				}
			}
			else
			{
				if ( pPlayer )
				{
					if ( pPlayer->InSameTeam( pOwner ) )
					{
						if ( GetBuffType() == 1 )
							pPlayer->m_Shared.RemoveCond( TF_COND_OFFENSEBUFF );
						else if ( GetBuffType() == 2 )
							pPlayer->m_Shared.RemoveCond( TF_COND_DEFENSEBUFF );
						else if ( GetBuffType() == 3 )
							pPlayer->m_Shared.RemoveCond( TF_COND_REGENONDAMAGEBUFF );
					}
				}

				if ( pNPC )
				{
					if ( pNPC->InSameTeam( pOwner ) )
					{
						if ( GetBuffType() == 1 )
							pNPC->RemoveCond( TF_COND_OFFENSEBUFF );
						else if ( GetBuffType() == 2 )
							pNPC->RemoveCond( TF_COND_DEFENSEBUFF );
						else if ( GetBuffType() == 3 )
							pNPC->RemoveCond( TF_COND_REGENONDAMAGEBUFF );
					}
				}
			}
		}
#endif
	}

	//m_flEffectBarRegenTime = gpGlobals->curtime + InternalGetEffectBarRechargeTime();
}

void CTFBuffItem::WeaponReset( void )
{
	m_flEffectBarRegenTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBuffItem::CheckEffectBarRegen( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	if ( m_flEffectBarRegenTime != 0.0f )
	{
		if ( gpGlobals->curtime >= m_flEffectBarRegenTime )
		{
			m_flEffectBarRegenTime = 0.0f;
			EffectBarRegenFinished();
		}
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
int CTFBuffItem::GetBuffType( void )
{
	int iBuffType = 0;
	CALL_ATTRIB_HOOK_INT( iBuffType, set_buff_type );
	return iBuffType;
}