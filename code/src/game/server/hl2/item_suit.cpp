//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== item_suit.cpp ========================================================

  handling for the player's suit.
*/

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_SUIT_SHORTLOGON		0x0001

class CItemSuit : public CItem
{
public:
	DECLARE_CLASS( CItemSuit, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/hevsuit.mdl" );
		char szMapName[256];
		Q_strncpy(szMapName, STRING(gpGlobals->mapname), sizeof(szMapName) );
		Q_strlower(szMapName);

		if( !Q_strnicmp( szMapName, "c1a0d", 5 ) )
		{
			PrecacheModel( "models/w_suit_hl1.mdl" );
			SetModel( "models/w_suit_hl1.mdl" );
		}
		else if( !Q_strnicmp( szMapName, "t0a0", 4 ) )
		{
			PrecacheModel( "models/w_suit_hl1.mdl" );
			SetModel( "models/w_suit_hl1.mdl" );
		}
		BaseClass::Spawn( );
		
		CollisionProp()->UseTriggerBounds( false, 0 );
	}
	void Precache( void )
	{
		PrecacheModel ("models/items/hevsuit.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
#ifdef TF_CLASSIC
		// Can be picked up but doesn't actually do anything.
		return true;
#else
		if ( pPlayer->IsSuitEquipped() )
			return FALSE;

		if ( m_spawnflags & SF_SUIT_SHORTLOGON )
			UTIL_EmitSoundSuit(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
		else
			UTIL_EmitSoundSuit(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon

		pPlayer->EquipSuit();
				
		return true;
#endif
	}
};

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit);
