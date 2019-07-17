//=========================                         ==========================//
//
// Purpose: Custom Weapon with Custom Attributes.
//
//=============================================================================//
#ifndef ENTITY_WEAPON_CUSTOM_H
#define ENTITY_WEAPON_CUSTOM_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

#define MAX_NUM_CUSTOM_ATTRIBUTES		16

//=============================================================================

class CTFWeaponCustom : public CTFPowerup
{
public:
	DECLARE_CLASS( CTFWeaponCustom, CTFPowerup );
	DECLARE_DATADESC();

	CTFWeaponCustom();

	void	Spawn( void );
	void	Precache( void );
	virtual bool KeyValue( const char *szKeyName, const char *szValue );
	virtual CBaseEntity* Respawn( void );
	virtual void	Materialize( void );
	bool	MyTouch( CBasePlayer *pPlayer );
	void	EndTouch( CBaseEntity *pOther );

private:
	CEconItemView m_Item;

	int						m_nAttributes[16];
	float					m_nAttributesValue[16];
	bool					m_bSkipBaseAttributes;

	int		m_nItemID;
};

#endif // ENTITY_WEAPON_CUSTOM_H


