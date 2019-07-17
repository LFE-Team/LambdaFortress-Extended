//=============================================================================//
//	BY ALISTAIR 'peachfuzz' LESZKIEWICZ
//	peachfux@gmail.com
//=============================================================================//

#include "cbase.h"
#include "vcollide_parse.h"
#include "ndebugoverlay.h"
#include "igamemovement.h"
#include "soundenvelope.h"
#include "npc_vehicledriver.h"
#include "physics_saverestore.h"
#include "func_break.h"
#include "physics_impact_damage.h"
#include "entityblocker.h"

#include "vehicle_passenger.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


BEGIN_DATADESC( CPropVehiclePassenger )
	// Outputs
	DEFINE_OUTPUT( m_passengerOn, "PassengerOn" ),
	DEFINE_OUTPUT( m_passengerOff, "PassengerOff" ),

	DEFINE_ARRAY(m_hPassenger, FIELD_EHANDLE, LAST_SHARED_VEHICLE_ROLE),
	/*DEFINE_FIELD( m_hPassenger1, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hPassenger2, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hPassenger3, FIELD_EHANDLE ),*/

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CPropVehiclePassenger, DT_PropVehiclePassenger )
	
	SendPropArray( SendPropEHandle( SENDINFO(m_hPassenger) ), m_hPassenger ),

END_SEND_TABLE();

LINK_ENTITY_TO_CLASS( vehicle_passenger, CPropVehiclePassenger );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPropVehiclePassenger::CPropVehiclePassenger( void )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::DriveVehicle( CBasePlayer *pPlayer, CUserCmd *ucmd )
{
	//Lose control when the player dies
	if ( pPlayer->IsAlive() == false )
		return;

	//player is not the driver
	if( pPlayer != m_hPlayer )
		return;

	DriveVehicle( TICK_INTERVAL, ucmd, pPlayer->m_afButtonPressed, pPlayer->m_afButtonReleased );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased )
{
	int iButtons = ucmd->buttons;

	m_VehiclePhysics.UpdateDriverControls( ucmd, flFrameTime );

	m_nSpeed = m_VehiclePhysics.GetSpeed();	//send speed to client
	m_nRPM = clamp( m_VehiclePhysics.GetRPM(), 0, 4095 );
	m_nBoostTimeLeft = m_VehiclePhysics.BoostTimeLeft();
	m_nHasBoost = m_VehiclePhysics.HasBoost();
	m_flThrottle = m_VehiclePhysics.GetThrottle();

	m_nScannerDisabledWeapons = false;		// off for now, change once we have scanners, was "0", coudlnt compile, comments indicate it should be true but 0 indicated false
	m_nScannerDisabledVehicle = false;		// off for now, change once we have scanners

	//
	// Fire the appropriate outputs based on button pressed events.
	//
	// BUGBUG: m_afButtonPressed is broken - check the player.cpp code!!!
	float attack = 0, attack2 = 0;

	if ( iButtonsDown & IN_ATTACK )
	{
		m_pressedAttack.FireOutput( this, this, 0 );
	}
	if ( iButtonsDown & IN_ATTACK2 )
	{
		m_pressedAttack2.FireOutput( this, this, 0 );
	}

	if ( iButtons & IN_ATTACK )
	{
		attack = 1;
	}
	if ( iButtons & IN_ATTACK2 )
	{
		attack2 = 1;
	}

	m_attackaxis.Set( attack, this, this );
	m_attack2axis.Set( attack2, this, this );
}



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::CreateServerVehicle( void )
{
	// Create our armed server vehicle
	m_pServerVehicle = new CPassengerServerVehicle();
	m_pServerVehicle->SetVehicle( this );
}

/*CBaseEntity* CPropVehiclePassenger::GetPassenger( int iRole )
{
	if( iRole == VEHICLE_DRIVER )
	{
		return GetDriver();
	}
	else
	{
		switch( iRole )
		{
		case VEHICLE_PASSENGER1:
			return m_hPassenger1;
		case VEHICLE_PASSENGER2:
			return m_hPassenger2;
		case VEHICLE_PASSENGER3:
			return m_hPassenger3;
		}
	}
	Assert( 0 );
	return NULL;
}*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPropVehiclePassenger::GetNumPassengers( void ) 
{ 
	
	int count = 0;
	for (int i = 0; i<LAST_SHARED_VEHICLE_ROLE; i++)
	{
		if( GetPassenger( i ) )
			count++;
	}

	return count;
}

void CPropVehiclePassenger::NullifyPassenger( int iRole )
{
	//FixMe, wtf?
	#ifndef DEBUG
	Assert( iRole != VEHICLE_DRIVER );
	#endif

	/*switch( iRole )
	{
		case VEHICLE_PASSENGER1:
			m_hPassenger1 = NULL;
			break;
		case VEHICLE_PASSENGER2:
			m_hPassenger2 = NULL;
			break;
		case VEHICLE_PASSENGER3:
			m_hPassenger3 = NULL;
			break;
	
		default:
			Assert( 0 );
	}*/

	m_hPassenger.Set( iRole, NULL );

}

void CPropVehiclePassenger::PassengerEnterVehicle( CBasePlayer *pPlayer, int iRole )
{
	if ( !pPlayer )
		return;

	if( iRole == VEHICLE_ROLE_DRIVER )
	{
		return EnterVehicle( pPlayer );
	}
	
	m_passengerOn.FireOutput( pPlayer, this, 0 );

	/*switch( iRole )
	{
		case VEHICLE_PASSENGER1:
			m_hPassenger1 = pPlayer;
			break;
		case VEHICLE_PASSENGER2:
			m_hPassenger2 = pPlayer;
			break;
		case VEHICLE_PASSENGER3:
			m_hPassenger3 = pPlayer;
			break;
		default:
			Assert( 0 );
	}*/

	m_hPassenger.Set( iRole, pPlayer );

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move )
{
	// If the engine's not active, prevent driving
	if ( !IsEngineOn() || m_bEngineLocked )
		return;

	// If the player's entering/exiting the vehicle, prevent movement
	if ( m_bEnterAnimOn || m_bExitAnimOn )
		return;

	DriveVehicle( player, ucmd );
}

//-----------------------------------------------------------------------------
// Purpose: Return true of the player's allowed to enter the vehicle
//-----------------------------------------------------------------------------
bool CPropVehiclePassenger::CanEnterVehicle( CBaseEntity *pEntity )
{
	// the max passengers
	if (GetNumPassengers() >= LAST_SHARED_VEHICLE_ROLE)
		return false;

	//one person at a time
	if( m_bEnterAnimOn || m_bExitAnimOn)
		return false;

	if ( IsOverturned() )
		return false;

	// Prevent entering if the vehicle's locked, or if it's moving too fast.
	return ( !m_bLocked && (m_nSpeed <= m_flMinimumSpeedToEnterExit) );
}

//-----------------------------------------------------------------------------
// Purpose: Return true of the player's allowed to exit the vehicle
//-----------------------------------------------------------------------------
bool CPropVehiclePassenger::CanExitVehicle( CBaseEntity *pEntity )
{
	// Prevent exiting if the vehicle's locked, or if it's moving too fast.
	return ( !m_bEnterAnimOn && !m_bExitAnimOn && !m_bLocked && (m_nSpeed <= m_flMinimumSpeedToEnterExit) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::EnterVehicle( CBasePlayer *pPlayer )
{
	return BaseClass::EnterVehicle( pPlayer );

	if ( !pPlayer )
		return;

	// Remove any player who may be in the vehicle at the moment
	//if ( m_hPlayer )
	//{
	//	ExitVehicle(VEHICLE_DRIVER);
	//}

	m_hPlayer = pPlayer;
	m_playerOn.FireOutput( pPlayer, this, 0 );

	// Don't start the engine if the player's using an entry animation,
	// because we want to start the engine once the animation is done.
	if ( !m_bEnterAnimOn )
	{
		StartEngine();
	}

	// Start Thinking
	SetNextThink( gpGlobals->curtime );

	m_VehiclePhysics.GetVehicle()->OnVehicleEnter();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropVehiclePassenger::ExitVehicle( int iRole )
{
	if( iRole == VEHICLE_ROLE_DRIVER )
		return BaseClass::ExitVehicle( iRole );

	CBasePlayer *pPlayer = ToBasePlayer( GetPassenger(iRole ) );
	
	if ( !pPlayer )
		return;

	NullifyPassenger( iRole );
	ResetUseKey( pPlayer );
	
	//m_passengerOff.FireOutput( pPlayer, this, 0 );

	// clear out the fire buttons
/*	m_attackaxis.Set( 0, pPlayer, this );
	m_attack2axis.Set( 0, pPlayer, this );
	
	m_nSpeed = 0;
	m_flThrottle = 0.0f;
	StopEngine();
	m_VehiclePhysics.GetVehicle()->OnVehicleExit();*/
}