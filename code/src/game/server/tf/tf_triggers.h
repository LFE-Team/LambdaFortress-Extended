//===== NOT Copyright © 1996-2005, Valve Corporation, All rights reserved. =====//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TE_TFTRIGGER_H
#define TE_TFTRIGGER_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "triggers.h"

class CTriggerAddTFPlayerCondition : public CBaseTrigger
{
	DECLARE_CLASS( CTriggerAddTFPlayerCondition, CBaseTrigger );
public:
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *pOther );
	
	// Ignore non-living entities
	virtual bool PassesTriggerFilters(CBaseEntity *pOther)
	{
		if ( !BaseClass::PassesTriggerFilters(pOther) )
			return false;

		return (pOther->m_takedamage == DAMAGE_YES);
	}

	virtual void StartTouch(CBaseEntity *pOther);
	virtual void EndTouch(CBaseEntity *pOther);

private:

	float			m_flDuration;
	int				m_nCondition;
};

class CTriggerRemoveTFPlayerCondition : public CBaseTrigger
{
	DECLARE_CLASS( CTriggerRemoveTFPlayerCondition, CBaseTrigger );
public:
	DECLARE_DATADESC();

	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *pOther );
	
	// Ignore non-living entities
	virtual bool PassesTriggerFilters(CBaseEntity *pOther)
	{
		if ( !BaseClass::PassesTriggerFilters(pOther) )
			return false;

		return (pOther->m_takedamage == DAMAGE_YES);
	}

	virtual void StartTouch(CBaseEntity *pOther);
	virtual void EndTouch(CBaseEntity *pOther);

private:

	int				m_nCondition;
};

#endif