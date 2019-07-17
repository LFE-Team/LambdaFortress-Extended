//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"
#include "hl1_items.h"
#include "tf_player.h"


class CItemLongJump : public CHL1Item
{
public:
	DECLARE_CLASS( CItemLongJump, CHL1Item );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/w_longjump.mdl" );
		BaseClass::Spawn( );

		CollisionProp()->UseTriggerBounds( true, 16.0f );
	}
	void Precache( void )
	{
		PrecacheModel ("models/w_longjump.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		CTFPlayer *pTFPlayer = (CTFPlayer*)pPlayer;

		pTFPlayer->m_bHasLongJump = true;// player now has longjump module
		CreateEntityByName("lfe_logic_longjump", -1);

		CSingleUserRecipientFilter user( pTFPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( STRING(m_iClassname) );
		MessageEnd();

		UTIL_EmitSoundSuit( pTFPlayer->edict(), "!HEV_A1" );	// Play the longjump sound UNDONE: Kelly? correct sound?
		return true;
	}
};

LINK_ENTITY_TO_CLASS( item_longjump, CItemLongJump );
PRECACHE_REGISTER(item_longjump);
