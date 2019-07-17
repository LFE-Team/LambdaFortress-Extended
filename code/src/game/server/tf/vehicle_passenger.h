//=============================================================================//
//	BY ALISTAIR 'peachfuzz' LESZKIEWICZ
//	peachfux@gmail.com
//=============================================================================//


#ifndef PASSENGER_VEHICLE_H
#define PASSENGER_VEHICLE_H

#ifdef _WIN32
#pragma once
#endif

#include "vehicle_base.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "IEffects.h"
#include "soundenvelope.h"
#include "decals.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "movevars_shared.h"
#include "bone_setup.h"
#include "ai_hint.h"
#include "globalstate.h"
#include "vehicle_passengerserver.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CPassengerServerVehicle;

class CPropVehiclePassenger : public CPropVehicleDriveable
{
	DECLARE_CLASS( CPropVehiclePassenger, CPropVehicleDriveable );

public:

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPropVehiclePassenger( void );


	// Driving
	void	DriveVehicle( CBasePlayer *pPlayer, CUserCmd *ucmd );	// Player driving entrypoint
	virtual void DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased ); // Driving Button handling
	virtual bool IsVehicleBodyInWater( void ) { return false; }

// IPassengerVehicle
public:
	//to make non passenger vehicles back compatible
	virtual CBaseEntity	*GetPassenger( int iRole ) 
	{ 
		if( iRole == VEHICLE_ROLE_DRIVER )
			return BaseClass::GetPassenger(iRole);

		return m_hPassenger[iRole].Get(); 
	}
	virtual int			GetNumPassengers( void );
	virtual void		NullifyPassenger( int iRole );
	virtual void		PassengerEnterVehicle( CBasePlayer *pPlayer, int nRole );
	
	virtual void		CreateServerVehicle( void );

	virtual void		SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void		ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData ) { return; }
	virtual void		FinishMove( CBasePlayer *player, CUserCmd *ucmd, CMoveData *move ) { return; }
	virtual bool		CanEnterVehicle( CBaseEntity *pEntity );
	virtual bool		CanExitVehicle( CBaseEntity *pEntity );
	virtual void		SetVehicleEntryAnim( bool bOn ) { m_bEnterAnimOn = bOn; }
	virtual void		SetVehicleExitAnim( bool bOn, Vector vecEyeExitEndpoint ) { m_bExitAnimOn = bOn; if ( bOn ) m_vecEyeExitEndpoint = vecEyeExitEndpoint; }
	virtual void		EnterVehicle( CBasePlayer *pPlayer );

	virtual bool		AllowBlockedExit( CBasePlayer *pPlayer, int iRole ) { return true; }
	virtual bool		AllowMidairExit( CBasePlayer *pPlayer, int iRole ) { return false; }
	virtual void		PreExitVehicle( CBasePlayer *pPlayer, int iRole ) {}
	virtual void		ExitVehicle( int iRole );

private:


	CNetworkArray(EHANDLE, m_hPassenger, LAST_SHARED_VEHICLE_ROLE);



	COutputEvent		m_passengerOn;
	COutputEvent		m_passengerOff;
};

#endif