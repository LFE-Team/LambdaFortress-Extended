//========= Copyright Valve Corporation, All rights reserved. =================
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_PHYSGUN_H
#define TF_WEAPON_PHYSGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "beamdraw.h"
	#include "fx_interpvalue.h"
#else

#endif

#include "soundenvelope.h"
#include "physics.h"
#include "physics_saverestore.h"
#include "tf_weaponbase.h"

#define PHYSGUN_BEAM_SPRITE1	"sprites/physbeam1.vmt"
#define PHYSGUN_BEAM_SPRITE		"sprites/physbeam.vmt"
#define PHYSGUN_BEAM_GLOW		"sprites/physglow.vmt"

#ifdef CLIENT_DLL
#define CWeaponGravityGun C_WeaponGravityGun
#define CWeaponGravityGun_Secondary C_WeaponGravityGun_Secondary
#endif


// derive from this so we can add save/load data to it
struct game_shadowcontrol_params_t : public hlshadowcontrol_params_t
{
	DECLARE_SIMPLE_DATADESC();
};

//-----------------------------------------------------------------------------
class CGrabController : public IMotionEvent
{
public:

	CGrabController( void );
	~CGrabController( void );
	void AttachEntity( CBasePlayer *pPlayer, CBaseEntity *pEntity, IPhysicsObject *pPhys, bool bIsMegaPhysCannon, const Vector &vGrabPosition, bool bUseGrabPosition );
	void DetachEntity( bool bClearVelocity );
	void OnRestore();

	bool UpdateObject( CBasePlayer *pPlayer, float flError );

	void SetTargetPosition( const Vector &target, const QAngle &targetOrientation );
	float ComputeError();
	float GetLoadWeight( void ) const { return m_flLoadWeight; }
	void SetAngleAlignment( float alignAngleCosine ) { m_angleAlignment = alignAngleCosine; }
	void SetIgnorePitch( bool bIgnore ) { m_bIgnoreRelativePitch = bIgnore; }
	QAngle TransformAnglesToPlayerSpace( const QAngle &anglesIn, CBasePlayer *pPlayer );
	QAngle TransformAnglesFromPlayerSpace( const QAngle &anglesIn, CBasePlayer *pPlayer );

	CBaseEntity *GetAttached() { return (CBaseEntity *)m_attachedEntity; }

	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );
	float GetSavedMass( IPhysicsObject *pObject );

	QAngle			m_attachedAnglesPlayerSpace;
	Vector			m_attachedPositionObjectSpace;

private:
	// Compute the max speed for an attached object
	void ComputeMaxSpeed( CBaseEntity *pEntity, IPhysicsObject *pPhysics );

	game_shadowcontrol_params_t	m_shadow;
	float			m_timeToArrive;
	float			m_errorTime;
	float			m_error;
	float			m_contactAmount;
	float			m_angleAlignment;
	bool			m_bCarriedEntityBlocksLOS;
	bool			m_bIgnoreRelativePitch;

	float			m_flLoadWeight;
	float			m_savedRotDamping[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	float			m_savedMass[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	EHANDLE			m_attachedEntity;
	QAngle			m_vecPreferredCarryAngles;
	bool			m_bHasPreferredCarryAngles;


	IPhysicsMotionController *m_controller;
	int				m_frameCount;
	friend class CWeaponGravityGun;
};

class CWeaponGravityGun : public CTFWeaponBase
{
public:
	DECLARE_CLASS( CWeaponGravityGun, CTFWeaponBase );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CWeaponGravityGun( void );

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_PHYSGUN; }
	virtual void	WeaponReset( void );
	virtual int		GetSlot( void ) const;
	virtual void	FallInit( void );
	void	Drop( const Vector &vecVelocity );
	void	Precache();
	virtual void	OnRestore();
	virtual void	StopLoopingSounds();
	virtual void	UpdateOnRemove(void);
	void	PrimaryAttack();
	void	SecondaryAttack();
	void	WeaponIdle();
	void	ItemPreFrame();
	void	ItemPostFrame();
	void	ItemHolsterFrame();

	void	ForceDrop( void );
	bool	DropIfEntityHeld( CBaseEntity *pTarget );	// Drops its held entity if it matches the entity passed in
	CGrabController &GetGrabController() { return m_grabController; }

	bool	CanHolster( void ) const;
	bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	bool	Deploy( void );

	bool	HasAnyAmmo( void ) { return true; }

	virtual void SetViewModel( void );
	virtual const char *GetShootSound( int iIndex ) const;
	
#ifndef CLIENT_DLL
	CNetworkQAngle	( m_attachedAnglesPlayerSpace );
#else
	QAngle m_attachedAnglesPlayerSpace;
#endif

	CNetworkVector	( m_attachedPositionObjectSpace );

	CNetworkHandle( CBaseEntity, m_hAttachedObject );

	EHANDLE m_hOldAttachedObject;

protected:
	enum FindObjectResult_t
	{
		OBJECT_FOUND = 0,
		OBJECT_NOT_FOUND,
		OBJECT_BEING_DETACHED,
	};

	void	DoEffect( int effectType, Vector *pos = NULL );

	void	OpenElements( void );
	void	CloseElements( void );

	// Pickup and throw objects.
	bool	CanPickupObject( CBaseEntity *pTarget );
	void	CheckForTarget( void );
	
#ifndef CLIENT_DLL
	bool	AttachObject( CBaseEntity *pObject, const Vector &vPosition );
	FindObjectResult_t		FindObject( void );
	CBaseEntity *FindObjectInCone( const Vector &vecOrigin, const Vector &vecDir, float flCone );
#endif	// !CLIENT_DLL

	void	UpdateObject( void );
	void	DetachObject( bool playSound = true, bool wasLaunched = false );
	void	LaunchObject( const Vector &vecDir, float flForce );
	void	StartEffects( void );	// Initialize all sprites and beams
	void	StopEffects( bool stopSound = true );	// Hide all effects temporarily
	void	DestroyEffects( void );	// Destroy all sprites and beams

	// Punt objects - this is pointing at an object in the world and applying a force to it.
	void	PuntNonVPhysics( CBaseEntity *pEntity, const Vector &forward, trace_t &tr );
	void	PuntVPhysics( CBaseEntity *pEntity, const Vector &forward, trace_t &tr );

	// Velocity-based throw common to punt and launch code.
	void	ApplyVelocityBasedForce( CBaseEntity *pEntity, const Vector &forward );

	// Physgun effects
	void	DoEffectClosed( void );
	void	DoEffectReady( void );
	void	DoEffectHolding( void );
	void	DoEffectLaunch( Vector *pos );
	void	DoEffectNone( void );
	void	DoEffectIdle( void );

	// Trace length
	float	TraceLength();

	// Sprite scale factor 
	float	SpriteScaleFactor();

	float			GetLoadPercentage();
	CSoundPatch		*GetMotorSound( void );

	void	DryFire( void );
	void	PrimaryFireEffect( void );

#ifndef CLIENT_DLL
	// What happens when the physgun picks up something 
	void	Physgun_OnPhysGunPickup( CBaseEntity *pEntity, CBasePlayer *pOwner, PhysGunPickup_t reason );
#endif	// !CLIENT_DLL

#ifdef CLIENT_DLL

	enum EffectType_t
	{
		PHYSCANNON_CORE = 0,
		
		PHYSCANNON_BLAST,

		PHYSCANNON_GLOW1,	// Must be in order!
		PHYSCANNON_GLOW2,
		PHYSCANNON_GLOW3,
		PHYSCANNON_GLOW4,
		PHYSCANNON_GLOW5,
		PHYSCANNON_GLOW6,

		PHYSCANNON_ENDCAP1,	// Must be in order!
		PHYSCANNON_ENDCAP2,
		PHYSCANNON_ENDCAP3,	// Only used in third-person!

		NUM_PHYSCANNON_PARAMETERS	// Must be last!
	};

#define	NUM_GLOW_SPRITES ((CWeaponGravityGun::PHYSCANNON_GLOW6-CWeaponGravityGun::PHYSCANNON_GLOW1)+1)
#define NUM_ENDCAP_SPRITES ((CWeaponGravityGun::PHYSCANNON_ENDCAP3-CWeaponGravityGun::PHYSCANNON_ENDCAP1)+1)

#define	NUM_PHYSCANNON_BEAMS	3

	virtual int		DrawModel( int flags );
	virtual void	ViewModelDrawn( C_BaseViewModel *pBaseViewModel );
	virtual bool	IsTransparent( void );
	virtual void	OnDataChanged( DataUpdateType_t type );
	virtual void	ClientThink( void );
	
	void			ManagePredictedObject( void );
	void			DrawEffects( void );
	void			GetEffectParameters( EffectType_t effectID, color32 &color, float &scale, IMaterial **pMaterial, Vector &vecAttachment );
	void			DrawEffectSprite( EffectType_t effectID );
	inline bool		IsEffectVisible( EffectType_t effectID );
	void			UpdateElementPosition( void );

	// We need to render opaque and translucent pieces
	RenderGroup_t	GetRenderGroup( void ) {	return RENDER_GROUP_TWOPASS;	}

	CInterpolatedValue		m_ElementParameter;							// Used to interpolate the position of the articulated elements
	//CPhysCannonEffect		m_Parameters[NUM_PHYSCANNON_PARAMETERS];	// Interpolated parameters for the effects
	//CPhysCannonEffectBeam	m_Beams[NUM_PHYSCANNON_BEAMS];				// Beams

	int				m_nOldEffectState;	// Used for parity checks
	bool			m_bOldOpen;			// Used for parity checks

	void			NotifyShouldTransmit( ShouldTransmitState_t state );

#endif	// CLIENT_DLL

	int		m_nChangeState;				// For delayed state change of elements
	float	m_flCheckSuppressTime;		// Amount of time to suppress the checking for targets
	bool	m_flLastDenySoundPlayed;	// Debounce for deny sound
	int		m_nAttack2Debounce;

	CNetworkVar( bool,	m_bActive );
	CNetworkVar( int,	m_EffectState );		// Current state of the effects on the gun
	CNetworkVar( bool,	m_bOpen );

	bool	m_bResetOwnerEntity;
	
	float	m_flElementDebounce;

	CSoundPatch			*m_sndMotor;		// Whirring sound for the gun
	
	CGrabController		m_grabController;

	float	m_flRepuntObjectTime;
	EHANDLE m_hLastPuntedObject;

private:
	CWeaponGravityGun( const CWeaponGravityGun & );

#ifndef CLIENT_DLL
#ifdef HL2MP
	DECLARE_ACTTABLE();
#endif
#endif
};

class CWeaponGravityGun_Secondary : public CWeaponGravityGun
{
public:
	DECLARE_CLASS( CWeaponGravityGun_Secondary, CWeaponGravityGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
};

#endif // TF_WEAPON_PHYSGUN_H