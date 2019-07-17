//=============================================================================//
//	BY ALISTAIR 'peachfuzz' LESZKIEWICZ
//	peachfux@gmail.com
//=============================================================================//


#ifndef C_PROP_VEHICLEPASSENGER_H
#define C_PROP_VEHICLEPASSENGER_H
#pragma once

#include "c_prop_vehicle.h"

class C_PropVehiclePassenger : public C_PropVehicleDriveable
{

	DECLARE_CLASS( C_PropVehiclePassenger, C_PropVehicleDriveable );

public:

	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();
	DECLARE_DATADESC();

	C_PropVehiclePassenger();
	~C_PropVehiclePassenger();


	// IVehicle overrides.
public:

	virtual C_BaseCombatCharacter* GetPassenger( int nRole )
	{ 
		if( nRole == VEHICLE_ROLE_DRIVER )
			return BaseClass::GetPassenger(nRole);

		return m_hPassenger[nRole].Get(); 
	}
	virtual int	GetPassengerRole(CBaseCombatCharacter *pEnt);
	C_BasePlayer* 	GetPrevPassenger( int nRole ){ return m_hPrevPassenger[nRole].Get(); }
	virtual bool IsPassengerUsingStandardWeapons( int nRole = VEHICLE_ROLE_DRIVER ) { return false; }
	virtual void GetVehicleViewPosition( int nRole, Vector *pOrigin, QAngle *pAngles );

	virtual void SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move ) {}
	virtual void ProcessMovement( C_BasePlayer *pPlayer, CMoveData *pMoveData ){}
	virtual void FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move ) {}

	virtual void ItemPostFrame( C_BasePlayer *pPlayer );

	virtual bool IsPredicted() const { return true; }

// IClientVehicle overrides.
public:

	virtual void GetVehicleFOV( float &flFOV ) { flFOV = m_flFOV; }
	virtual void DrawHudElements();
	virtual void UpdateViewAngles( C_BasePlayer *pLocalPlayer, CUserCmd *pCmd );
	virtual void DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles );
	virtual void GetVehicleClipPlanes( float &flZNear, float &flZFar ) const;

#ifdef HL2_CLIENT_DLL
	virtual int GetPrimaryAmmoType() const { return -1; }
	virtual int GetPrimaryAmmoCount() const { return -1; }
	virtual int GetPrimaryAmmoClip() const  { return -1; }
	virtual bool PrimaryAmmoUsesClips() const { return false; }
#endif

// C_BaseEntity overrides.
public:

	virtual IClientVehicle*	GetClientVehicle() { return this; }
	virtual C_BaseEntity	*GetVehicleEnt() { return this; }
	virtual bool IsSelfAnimating() { return false; };

	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	// Should this object cast render-to-texture shadows?
	virtual ShadowType_t ShadowCastType();

	// Mark the shadow as dirty while the vehicle is being driven
	virtual void ClientThink( void );

private:
	virtual void OnEnteredVehicle( C_BasePlayer *pPlayer );
	virtual void RestrictView( float *pYawBounds, float *pPitchBounds, float *pRollBounds, QAngle &vecViewAngles );
	virtual void SetVehicleFOV( float flFOV ) { m_flFOV = flFOV; }

	CHandle< C_BasePlayer >	m_hPassenger[LAST_SHARED_VEHICLE_ROLE];

	CHandle< C_BasePlayer >	m_hPrevPassenger[LAST_SHARED_VEHICLE_ROLE];

};

#endif // C_PROP_VEHICLE_H