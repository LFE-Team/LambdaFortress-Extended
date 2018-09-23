//======= Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: CTF Flag Alert trigger.
//
//=============================================================================//
#ifndef FUNC_FLAG_ALERT_H
#define FUNC_FLAG_ALERT_H

#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "triggers.h"
#include "tf_player.h"

//=============================================================================

DECLARE_AUTO_LIST( IFlagAlertZoneAutoList );

class CFuncFlagAlertZone : public CBaseTrigger, public IFlagAlertZoneAutoList
{
public:
	DECLARE_CLASS( CFuncFlagAlertZone, CBaseTrigger );
	DECLARE_DATADESC();

	virtual void	Spawn( void );
	virtual void	StartTouch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

	virtual void	SetDisabled( bool bDisabled );

	bool	EntityIsFlagCarrier( CBaseEntity *pEntity );

	void	FlagCaptured( CBaseEntity *pPlayer );
	void	FlagPickedUp( CBaseEntity *pPlayer );

	bool	IsDisabled( void ) { return m_bDisabled; };

	// Input handlers
	virtual void	InputEnable( inputdata_t &inputdata );
	virtual void	InputDisable( inputdata_t &inputdata );

private:
	bool	m_bDisabled;
	bool	m_bPlaySound;
	float	m_nAlertDelay;

	COutputEvent m_OnTriggeredByTeam1; // Sent when a red flag carrier first touches the zone.
	COutputEvent m_OnTriggeredByTeam2; // Sent when a blue flag carrier first touches the zone.
};

void HandleFlagPickedUpInAlertZone( CBaseEntity *pPlayer );
void HandleFlagCapturedInAlertZone( CBaseEntity *pPlayer );

#endif // FUNC_FLAG_ALERT_H



