//=========================                         ==========================//
//
// Purpose: Custom Weapon with Custom Attributes.
//
//=============================================================================//
#include "cbase.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_weapon_custom.h"
#include "tf_weaponbase.h"
#include "basecombatcharacter.h"
#include "in_buttons.h"
#include "tf_fx.h"
#include "tf_dropped_weapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CTFWeaponCustom )
	DEFINE_KEYFIELD( m_nItemID, 			FIELD_INTEGER, "itemid" ),
	DEFINE_KEYFIELD( m_bSkipBaseAttributes,	FIELD_BOOLEAN, "skipbaseattribute"),

	DEFINE_KEYFIELD( m_nAttributes[0], FIELD_INTEGER, "attribute01"),
	DEFINE_KEYFIELD( m_nAttributes[1], FIELD_INTEGER, "attribute02"),
	DEFINE_KEYFIELD( m_nAttributes[2], FIELD_INTEGER, "attribute03"),
	DEFINE_KEYFIELD( m_nAttributes[3], FIELD_INTEGER, "attribute04"),
	DEFINE_KEYFIELD( m_nAttributes[4], FIELD_INTEGER, "attribute05"),
	DEFINE_KEYFIELD( m_nAttributes[5], FIELD_INTEGER, "attribute06"),
	DEFINE_KEYFIELD( m_nAttributes[6], FIELD_INTEGER, "attribute07"),
	DEFINE_KEYFIELD( m_nAttributes[7], FIELD_INTEGER, "attribute08"),
	DEFINE_KEYFIELD( m_nAttributes[8], FIELD_INTEGER, "attribute09"),
	DEFINE_KEYFIELD( m_nAttributes[9], FIELD_INTEGER, "attribute10"),
	DEFINE_KEYFIELD( m_nAttributes[10], FIELD_INTEGER, "attribute11"),
	DEFINE_KEYFIELD( m_nAttributes[11], FIELD_INTEGER, "attribute12"),
	DEFINE_KEYFIELD( m_nAttributes[12], FIELD_INTEGER, "attribute13"),
	DEFINE_KEYFIELD( m_nAttributes[13], FIELD_INTEGER, "attribute14"),
	DEFINE_KEYFIELD( m_nAttributes[14], FIELD_INTEGER, "attribute15"),
	DEFINE_KEYFIELD( m_nAttributes[15], FIELD_INTEGER, "attribute16"),

	DEFINE_KEYFIELD( m_nAttributesValue[0], FIELD_FLOAT, "attributevalue01"),
	DEFINE_KEYFIELD( m_nAttributesValue[1], FIELD_FLOAT, "attributevalue02"),
	DEFINE_KEYFIELD( m_nAttributesValue[2], FIELD_FLOAT, "attributevalue03"),
	DEFINE_KEYFIELD( m_nAttributesValue[3], FIELD_FLOAT, "attributevalue04"),
	DEFINE_KEYFIELD( m_nAttributesValue[4], FIELD_FLOAT, "attributevalue05"),
	DEFINE_KEYFIELD( m_nAttributesValue[5], FIELD_FLOAT, "attributevalue06"),
	DEFINE_KEYFIELD( m_nAttributesValue[6], FIELD_FLOAT, "attributevalue07"),
	DEFINE_KEYFIELD( m_nAttributesValue[7], FIELD_FLOAT, "attributevalue08"),
	DEFINE_KEYFIELD( m_nAttributesValue[8], FIELD_FLOAT, "attributevalue09"),
	DEFINE_KEYFIELD( m_nAttributesValue[9], FIELD_FLOAT, "attributevalue10"),
	DEFINE_KEYFIELD( m_nAttributesValue[10], FIELD_FLOAT, "attributevalue11"),
	DEFINE_KEYFIELD( m_nAttributesValue[11], FIELD_FLOAT, "attributevalue12"),
	DEFINE_KEYFIELD( m_nAttributesValue[12], FIELD_FLOAT, "attributevalue13"),
	DEFINE_KEYFIELD( m_nAttributesValue[13], FIELD_FLOAT, "attributevalue14"),
	DEFINE_KEYFIELD( m_nAttributesValue[14], FIELD_FLOAT, "attributevalue15"),
	DEFINE_KEYFIELD( m_nAttributesValue[15], FIELD_FLOAT, "attributevalue16"),
END_DATADESC()

LINK_ENTITY_TO_CLASS( lfe_weapon_custom, CTFWeaponCustom );


CTFWeaponCustom::CTFWeaponCustom()
{
	m_nItemID = -1;
	m_bSkipBaseAttributes = false;

	for ( int i = 0; i < MAX_NUM_CUSTOM_ATTRIBUTES; i++ )
	{
		m_nAttributes[i] = 0;
		m_nAttributesValue[i] = 0;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFWeaponCustom::Spawn( void )
{
	CEconItemDefinition *pItemDef = GetItemSchema()->GetItemDefinition( m_nItemID );
	if ( !pItemDef )
	{
		Warning( "lfe_weapon_custom has incorrect item ID %d. DELETED\n", m_nItemID );
		UTIL_Remove( this );
		return;
	}

	m_Item.SetItemDefIndex( m_nItemID );

	Precache();

	SetModel( m_Item.GetWorldDisplayModel() );

	BaseClass::Spawn();

	SetSolid( SOLID_BBOX );
	SetCollisionBounds( -Vector( 22, 22, 15 ), Vector( 22, 22, 15 ) );

	//AddEffects( EF_ITEM_BLINK );
}

//-----------------------------------------------------------------------------
// Purpose: Precache function 
//-----------------------------------------------------------------------------
void CTFWeaponCustom::Precache( void )
{
	PrecacheModel( m_Item.GetWorldDisplayModel() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponCustom::KeyValue( const char *szKeyName, const char *szValue )
{
	return BaseClass::KeyValue( szKeyName, szValue );
}

//-----------------------------------------------------------------------------
// Purpose:  Override to get rid of EF_NODRAW
//-----------------------------------------------------------------------------
CBaseEntity* CTFWeaponCustom::Respawn( void )
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponCustom::Materialize( void )
{

}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
void CTFWeaponCustom::EndTouch( CBaseEntity *pOther )
{
	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pOther );

	if ( ValidTouch( pTFPlayer ) )
	{
		int iCurrentWeaponID = pTFPlayer->m_Shared.GetDesiredWeaponIndex();
		if ( iCurrentWeaponID == m_nItemID )
		{
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
		}
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponCustom::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );

	if ( ValidTouch( pTFPlayer ))
	{
		int iSlot = m_Item.GetStaticData()->GetLoadoutSlot( TF_CLASS_COUNT_ALL );
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->GetEntityForLoadoutSlot( iSlot );

		CTFWeaponBase *pActiveWeapon = (CTFWeaponBase *)pTFPlayer->GetActiveTFWeapon();
		if ( pActiveWeapon )
		{
			// Drop a usable weapon
			pTFPlayer->DropWeapon( pActiveWeapon );

			pActiveWeapon->UnEquip( pTFPlayer );
			pActiveWeapon = NULL;
		}


		if ( !pWeapon )
		{
			for ( int i = 0; i < MAX_NUM_CUSTOM_ATTRIBUTES; i++ )
			{
				CEconItemAttribute econAttribute( m_nAttributes[i], m_nAttributesValue[i] );
				econAttribute.m_strAttributeClass = AllocPooledString( econAttribute.attribute_class );
				m_bSkipBaseAttributes = m_Item.AddAttribute( &econAttribute );
			}

			m_Item.SkipBaseAttributes( m_bSkipBaseAttributes );

			const char *pszWeaponName = m_Item.GetEntityName();
			CTFWeaponBase *pNewWeapon = (CTFWeaponBase *)pTFPlayer->GiveNamedItem( pszWeaponName, 0, &m_Item );

			if ( pNewWeapon )
			{
				int iAmmoType = pNewWeapon->GetPrimaryAmmoType();
				pTFPlayer->SetAmmoCount( pTFPlayer->GetMaxAmmo( iAmmoType ), iAmmoType );
				pNewWeapon->GiveTo( pTFPlayer );
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
				bSuccess = true;
			}
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user( pPlayer );
			user.MakeReliable();

			UserMessageBegin( user, "ItemPickup" );
				WRITE_STRING( GetClassname() );
			MessageEnd();

			pPlayer->EmitSound( "BaseCombatCharacter.AmmoPickup" );

			UTIL_Remove( this );
		}
	}

	return bSuccess;
}
