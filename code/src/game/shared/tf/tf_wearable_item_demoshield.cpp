//=============================================================================
//
// Purpose: whatever this is gonna be if i ever want to add it.
//
//=============================================================================
#include "cbase.h"
#include "tf_wearable.h"

#ifdef GAME_DLL
#include "tf_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFWearableDemoShield, DT_TFWearableDemoShield );

BEGIN_NETWORK_TABLE( CTFWearableDemoShield, DT_TFWearableDemoShield )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_wearable, CTFWearableDemoShield );
PRECACHE_REGISTER( tf_wearable );

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWearableDemoShield::Equip( CBasePlayer *pPlayer )
{
	BaseClass::Equip( pPlayer );
	UpdateModelToClass();
}

//---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWearableDemoShield::UpdateModelToClass( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( pOwner )
	{
		const char *pszModel = m_Item.GetPlayerDisplayModel( pOwner->GetPlayerClass()->GetClassIndex() );

		if ( pszModel[0] != '\0' )
		{
			SetModel( pszModel );
		}
	}
}

#endif // GAME_DLL
