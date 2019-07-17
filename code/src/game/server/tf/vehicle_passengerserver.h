//=============================================================================//
//	BY ALISTAIR 'peachfuzz' LESZKIEWICZ
//	peachfux@gmail.com
//
//=============================================================================//


#ifndef PASSENGER_SERVER_VEHICLE
#define PASSENGER_SERVER_VEHICLE
#ifdef _WIN32
#pragma once
#endif

#include "vehicle_passenger.h"


class CPropVehiclePassenger;
class CPassengerServerVehicle : public CFourWheelServerVehicle
{

	DECLARE_DATADESC();
	typedef CFourWheelServerVehicle BaseClass;

// IServerVehicle
public:
	CPassengerServerVehicle( void )
	{
	}
	~CPassengerServerVehicle( void ){}

	IPassengerVehicle	*GetPassengerVehicle( void );

	// IVehicle
public:
	virtual CBasePlayer*	GetPassenger( int nRole = VEHICLE_ROLE_DRIVER );

	virtual int				GetPassengerRole( CBasePlayer *pPassenger );
	virtual void			GetVehicleViewPosition( int nRole, Vector *pOrigin, QAngle *pAngles );
	virtual bool			IsPassengerUsingStandardWeapons( int nRole = VEHICLE_ROLE_DRIVER ) { return false; }
	virtual void			SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void			ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData );
	virtual void			FinishMove( CBasePlayer *player, CUserCmd *ucmd, CMoveData *move );
	virtual void			ItemPostFrame( CBasePlayer *pPlayer );

	// IServerVehicle
public:
	virtual void			SetPassenger( int nRole, CBasePlayer *pPassenger );
	virtual bool			IsPassengerVisible( int nRole = VEHICLE_ROLE_DRIVER ) { return false; }
	virtual bool			IsPassengerDamagable( int nRole  = VEHICLE_ROLE_DRIVER ) { return true; }

	virtual void			HandlePassengerEntry( CBasePlayer *pPlayer, bool bAllowEntryOutsideZone = false );
	virtual bool			HandlePassengerExit( CBasePlayer *pPlayer );

	virtual void			GetPassengerStartPoint( int nRole, Vector *pPoint, QAngle *pAngles );
	virtual bool			GetPassengerExitPoint( int nRole, Vector *pPoint, QAngle *pAngles );

	virtual Class_T			ClassifyPassenger( CBasePlayer *pPassenger, Class_T defaultClassification ) { return defaultClassification; }
	virtual float			DamageModifier ( CTakeDamageInfo &info ) { return 1.0; }


public:
	virtual bool			CheckExitPoint( float yaw, int distance, Vector *pEndPoint );
	
	virtual int				GetEntryAnimForPoint( const Vector &vecPoint );
	virtual int				GetEntryRoleForPoint( const Vector &vecPoint );

	virtual int				GetExitAnimToUse( int nRole, Vector &vecEyeExitEndpoint, bool &bAllPointsBlocked );
	virtual void			HandleEntryExitFinish( int nRole, bool bExitAnimOn, bool bResetAnim );

	//kept for back compatability
	virtual int				GetExitAnimToUse( Vector &vecEyeExitEndpoint, bool &bAllPointsBlocked )
	{ return GetExitAnimToUse( m_CurrentEntryExitRole, vecEyeExitEndpoint, bAllPointsBlocked ); }

	virtual void			HandleEntryExitFinish( bool bExitAnimOn, bool bResetAnim )
	{ HandleEntryExitFinish( m_CurrentEntryExitRole, bExitAnimOn, bResetAnim ); }

	

	Vector m_savedPassengerViewOffset[LAST_SHARED_VEHICLE_ROLE];
	int m_CurrentEntryExitRole;
};
#endif