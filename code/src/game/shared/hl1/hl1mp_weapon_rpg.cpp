//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		RPG
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "hl1mp_basecombatweapon_shared.h"
#include "hl1mp_weapon_rpg.h"

#ifdef CLIENT_DLL
#include "hl1/c_hl1mp_player.h"
#include "model_types.h"
#include "beamdraw.h"
#include "fx_line.h"
#include "view.h"
#else
#include "basecombatcharacter.h"
#include "movie_explosion.h"
#include "hl1mp_player.h"
#include "rope.h"
#include "soundent.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "explode.h"
#include "util.h"
#include "te_effect_dispatch.h"
#include "shake.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_hl1_rpg;


void TE_BeamFollow(IRecipientFilter& filter, float delay,
	int iEntIndex, int modelIndex, int haloIndex, float life, float width, float endWidth,
	float fadeLength, float r, float g, float b, float a);

#define	RPG_LASER_SPRITE	"sprites/redglow_mp1"

class CLaserHL1Dot : public CBaseEntity
{
	DECLARE_CLASS(CLaserHL1Dot, CBaseEntity);
public:

	CLaserHL1Dot(void);
	~CLaserHL1Dot(void);

	static CLaserHL1Dot *Create(const Vector &origin, CBaseEntity *pOwner = NULL, bool bVisibleDot = true);

	void	SetTargetEntity(CBaseEntity *pTarget) { m_hTargetEnt = pTarget; }
	CBaseEntity *GetTargetEntity(void) { return m_hTargetEnt; }

	void	SetLaserPosition(const Vector &origin, const Vector &normal);
	Vector	GetChasePosition();
	void	TurnOn(void);
	void	TurnOff(void);
	bool	IsOn() const { return m_bIsOn; }

	void	Toggle(void);

	int		ObjectCaps() { return (BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }

	void	MakeInvisible(void);

#ifdef CLIENT_DLL

	virtual bool			IsTransparent(void) { return true; }
	virtual RenderGroup_t	GetRenderGroup(void) { return RENDER_GROUP_TRANSLUCENT_ENTITY; }
	virtual int				DrawModel(int flags);
	virtual void			OnDataChanged(DataUpdateType_t updateType);
	virtual bool			ShouldDraw(void) { return (IsEffectActive(EF_NODRAW) == false); }

	CMaterialReference	m_hSpriteMaterial;
#endif

protected:
	Vector				m_vecSurfaceNormal;
	EHANDLE				m_hTargetEnt;
	bool				m_bVisibleLaserHL1Dot;
	//	bool				m_bIsOn;
	CNetworkVar(bool, m_bIsOn);

	DECLARE_NETWORKCLASS();
	DECLARE_DATADESC();
public:
	CLaserHL1Dot			*m_pNext;
};


#ifndef CLIENT_DLL

//=============================================================================
// RPG Rocket
//=============================================================================


BEGIN_DATADESC(CRpgRocket)
DEFINE_FIELD(m_hOwner, FIELD_EHANDLE),
DEFINE_FIELD(m_vecAbsVelocity, FIELD_VECTOR),
DEFINE_FIELD(m_flIgniteTime, FIELD_TIME),
//DEFINE_FIELD( m_iTrail,			FIELD_INTEGER ),

// Function Pointers
DEFINE_ENTITYFUNC(RocketTouch),
DEFINE_THINKFUNC(IgniteThink),
DEFINE_THINKFUNC(SeekThink),
END_DATADESC()

LINK_ENTITY_TO_CLASS(rpg_rocket, CRpgRocket);

IMPLEMENT_SERVERCLASS_ST(CRpgRocket, DT_RpgRocket)
END_SEND_TABLE()

CRpgRocket::CRpgRocket()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CRpgRocket::Precache(void)
{
	PrecacheModel("models/rpgrocket.mdl");
	PrecacheModel("sprites/animglow01.vmt");

	PrecacheScriptSound("Weapon_RPG.RocketIgnite");

	m_iTrail = PrecacheModel("sprites/smoke.vmt");
}

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CRpgRocket::Spawn(void)
{
	Precache();

	SetSolid(SOLID_BBOX);
	SetModel("models/rpgrocket.mdl");
	UTIL_SetSize(this, -Vector(0, 0, 0), Vector(0, 0, 0));

	SetTouch(&CRpgRocket::RocketTouch);

	SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
	SetThink(&CRpgRocket::IgniteThink);

	SetNextThink(gpGlobals->curtime + 0.4f);

	QAngle angAngs;
	Vector vecFwd;

	angAngs = GetAbsAngles();
	angAngs.x -= 30;
	AngleVectors(angAngs, &vecFwd);
	SetAbsVelocity(vecFwd * 250);

	SetGravity(UTIL_ScaleForGravity(400));	// use a lower gravity for rockets

	m_flDamage = sk_plr_dmg_hl1_rpg.GetFloat();
	m_DmgRadius = m_flDamage * 2.5;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CRpgRocket::RocketTouch(CBaseEntity *pOther)
{
	if (!pOther->IsSolid() || pOther->IsSolidFlagSet(FSOLID_VOLUME_CONTENTS))
		return;

	if (m_hOwner != NULL)
	{
		m_hOwner->NotifyRocketDied();
	}

	StopSound("Weapon_RPG.RocketIgnite");
	ExplodeTouch(pOther);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRpgRocket::IgniteThink(void)
{
	SetMoveType(MOVETYPE_FLY);

	AddEffects(EF_DIMLIGHT);

	EmitSound("Weapon_RPG.RocketIgnite");

	SetThink(&CRpgRocket::SeekThink);
	SetNextThink(gpGlobals->curtime + 0.1f);

	CBroadcastRecipientFilter filter;
	TE_BeamFollow(filter, 0.0,
		entindex(),
		m_iTrail,
		0,
		4,
		5,
		5,
		0,
		224,
		224,
		255,
		255);

	m_flIgniteTime = gpGlobals->curtime;
}

#define	RPG_HOMING_SPEED	0.125f

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRpgRocket::SeekThink(void)
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget;
	Vector vecFwd;
	Vector vecDir;
	float flDist, flMax, flDot;
	trace_t tr;

	AngleVectors(GetAbsAngles(), &vecFwd);

	vecTarget = vecFwd;
	flMax = 4096;

	// Examine all entities within a reasonable radius
	while ((pOther = gEntList.FindEntityByClassname(pOther, "laser_spot")) != NULL)
	{
		CLaserHL1Dot *pDot = dynamic_cast<CLaserHL1Dot*>(pOther);

		//		if ( pDot->IsActive() )
		if (pDot->IsOn())
		{
			UTIL_TraceLine(GetAbsOrigin(), pDot->GetAbsOrigin(), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);
			if (tr.fraction >= 0.90)
			{
				vecDir = pDot->GetAbsOrigin() - GetAbsOrigin();
				flDist = VectorLength(vecDir);
				VectorNormalize(vecDir);
				flDot = DotProduct(vecFwd, vecDir);
				if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
				{
					flMax = flDist * (1 - flDot);
					vecTarget = vecDir;
				}
			}
		}
	}

	QAngle vecAng;
	VectorAngles(vecTarget, vecAng);
	SetAbsAngles(vecAng);

	// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
	float flSpeed = GetAbsVelocity().Length();
	if (gpGlobals->curtime - m_flIgniteTime < 1.0)
	{
		SetAbsVelocity(GetAbsVelocity() * 0.2 + vecTarget * (flSpeed * 0.8 + 400));
		if (GetWaterLevel() == 3)
		{
			// go slow underwater
			if (GetAbsVelocity().Length() > 300)
			{
				Vector vecVel = GetAbsVelocity();
				VectorNormalize(vecVel);
				SetAbsVelocity(vecVel * 300);
			}

			UTIL_BubbleTrail(GetAbsOrigin() - GetAbsVelocity() * 0.1, GetAbsOrigin(), 4);
		}
		else
		{
			if (GetAbsVelocity().Length() > 2000)
			{
				Vector vecVel = GetAbsVelocity();
				VectorNormalize(vecVel);
				SetAbsVelocity(vecVel * 2000);
			}
		}
	}
	else
	{
		if (IsEffectActive(EF_DIMLIGHT))
		{
			ClearEffects();
		}

		SetAbsVelocity(GetAbsVelocity() * 0.2 + vecTarget * flSpeed * 0.798);

		if (GetWaterLevel() == 0 && GetAbsVelocity().Length() < 1500)
		{
			Detonate();
		}
	}

	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CRpgRocket::Detonate(void)
{
	StopSound("Weapon_RPG.RocketIgnite");
	BaseClass::Detonate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//
// Input  : &vecOrigin - 
//			&vecAngles - 
//			NULL - 
//
// Output : CRpgRocket
//-----------------------------------------------------------------------------
CRpgRocket *CRpgRocket::Create(const Vector &vecOrigin, const QAngle &angAngles, CBasePlayer *pentOwner)
{
	CRpgRocket *pRocket = (CRpgRocket *)CreateEntityByName("rpg_rocket");
	UTIL_SetOrigin(pRocket, vecOrigin);
	pRocket->SetAbsAngles(angAngles);
	pRocket->Spawn();
	pRocket->SetOwnerEntity(pentOwner);

	return pRocket;
}

#endif		// endif #ifndef CLIENT_DLL


//=============================================================================
// Laser Dot
//=============================================================================

IMPLEMENT_NETWORKCLASS_ALIASED(LaserHL1Dot, DT_LaserHL1Dot)

BEGIN_NETWORK_TABLE(CLaserHL1Dot, DT_LaserHL1Dot)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bIsOn)),
#else
SendPropBool(SENDINFO(m_bIsOn)),
#endif
END_NETWORK_TABLE()

#ifndef CLIENT_DLL
// a list of laser dots to search quickly
CEntityClassList<CLaserHL1Dot> g_LaserHL1DotList;
template <> CLaserHL1Dot *CEntityClassList<CLaserHL1Dot>::m_pClassList = NULL;
CLaserHL1Dot *GetLaserHL1DotList()
{
	return g_LaserHL1DotList.m_pClassList;
}

#endif

LINK_ENTITY_TO_CLASS(laser_spot, CLaserHL1Dot);

BEGIN_DATADESC(CLaserHL1Dot)
DEFINE_FIELD(m_vecSurfaceNormal, FIELD_VECTOR),
DEFINE_FIELD(m_hTargetEnt, FIELD_EHANDLE),
DEFINE_FIELD(m_bVisibleLaserHL1Dot, FIELD_BOOLEAN),
DEFINE_FIELD(m_bIsOn, FIELD_BOOLEAN),

//DEFINE_FIELD( m_pNext, FIELD_CLASSPTR ),	// don't save - regenerated by constructor
END_DATADESC()


//-----------------------------------------------------------------------------
// Finds missiles in cone
//-----------------------------------------------------------------------------
CBaseEntity *CreateLaserHL1Dot(const Vector &origin, CBaseEntity *pOwner, bool bVisibleDot)
{
	return CLaserHL1Dot::Create(origin, pOwner, bVisibleDot);
}

void SetLaserHL1DotTarget(CBaseEntity *pLaserHL1Dot, CBaseEntity *pTarget)
{
	CLaserHL1Dot *pDot = assert_cast< CLaserHL1Dot* >(pLaserHL1Dot);
	pDot->SetTargetEntity(pTarget);
}

void EnableLaserHL1Dot(CBaseEntity *pLaserHL1Dot, bool bEnable)
{
	CLaserHL1Dot *pDot = assert_cast< CLaserHL1Dot* >(pLaserHL1Dot);
	if (bEnable)
	{
		pDot->TurnOn();
	}
	else
	{
		pDot->TurnOff();
	}
}

CLaserHL1Dot::CLaserHL1Dot(void)
{
	m_hTargetEnt = NULL;
	m_bIsOn = true;
#ifndef CLIENT_DLL
	g_LaserHL1DotList.Insert(this);
#endif
}

CLaserHL1Dot::~CLaserHL1Dot(void)
{
#ifndef CLIENT_DLL
	g_LaserHL1DotList.Remove(this);
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
// Output : CLaserHL1Dot
//-----------------------------------------------------------------------------
CLaserHL1Dot *CLaserHL1Dot::Create(const Vector &origin, CBaseEntity *pOwner, bool bVisibleDot)
{
#ifndef CLIENT_DLL
	CLaserHL1Dot *pLaserHL1Dot = (CLaserHL1Dot *)CBaseEntity::Create("laser_spot", origin, QAngle(0, 0, 0));

	if (pLaserHL1Dot == NULL)
		return NULL;

	pLaserHL1Dot->m_bVisibleLaserHL1Dot = bVisibleDot;
	pLaserHL1Dot->SetMoveType(MOVETYPE_NONE);
	pLaserHL1Dot->AddSolidFlags(FSOLID_NOT_SOLID);
	pLaserHL1Dot->AddEffects(EF_NOSHADOW);
	UTIL_SetSize(pLaserHL1Dot, -Vector(6, 6, 6), Vector(6, 6, 6));

	pLaserHL1Dot->SetOwnerEntity(pOwner);

	pLaserHL1Dot->AddEFlags(EFL_FORCE_CHECK_TRANSMIT);

	if (!bVisibleDot)
	{
		pLaserHL1Dot->MakeInvisible();
	}

	return pLaserHL1Dot;
#else
	return NULL;
#endif
}

void CLaserHL1Dot::SetLaserPosition(const Vector &origin, const Vector &normal)
{
	SetAbsOrigin(origin);
	m_vecSurfaceNormal = normal;
}

Vector CLaserHL1Dot::GetChasePosition()
{
	return GetAbsOrigin() - m_vecSurfaceNormal * 10;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserHL1Dot::TurnOn(void)
{
	m_bIsOn = true;
	RemoveEffects(EF_NODRAW);

	if (m_bVisibleLaserHL1Dot)
	{
		//BaseClass::TurnOn();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserHL1Dot::TurnOff(void)
{
	m_bIsOn = false;
	AddEffects(EF_NODRAW);
	if (m_bVisibleLaserHL1Dot)
	{
		//BaseClass::TurnOff();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserHL1Dot::MakeInvisible(void)
{
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: Draw our sprite
//-----------------------------------------------------------------------------
int CLaserHL1Dot::DrawModel(int flags)
{
	color32 color = { 255, 255, 255, 255 };
	Vector	vecAttachment, vecDir;
	QAngle	angles;

	float	scale;
	Vector	endPos;

	C_HL1MP_Player *pOwner = ToHL1MPPlayer(GetOwnerEntity());

	if (pOwner != NULL && pOwner->IsDormant() == false)
	{
		// Always draw the dot in front of our faces when in first-person
		if (pOwner->IsLocalPlayer())
		{
			// Take our view position and orientation
			vecAttachment = CurrentViewOrigin();
			vecDir = CurrentViewForward();
		}
		else
		{
			// Take the eye position and direction
			vecAttachment = pOwner->EyePosition();

			QAngle angles = pOwner->EyeAngles();
			AngleVectors(angles, &vecDir);
		}

		trace_t tr;
		UTIL_TraceLine(vecAttachment, vecAttachment + (vecDir * MAX_TRACE_LENGTH), MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr);

		// Backup off the hit plane
		endPos = tr.endpos + (tr.plane.normal * 4.0f);
	}
	else
	{
		// Just use our position if we can't predict it otherwise
		endPos = GetAbsOrigin();
	}

	// Randomly flutter
	scale = 16.0f + random->RandomFloat(-4.0f, 4.0f);

	// Draw our laser dot in space
	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->Bind(m_hSpriteMaterial, this);
	DrawSprite(endPos, scale, scale, color);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Setup our sprite reference
//-----------------------------------------------------------------------------
void CLaserHL1Dot::OnDataChanged(DataUpdateType_t updateType)
{
	if (updateType == DATA_UPDATE_CREATED)
	{
		m_hSpriteMaterial.Init(RPG_LASER_SPRITE, TEXTURE_GROUP_CLIENT_EFFECTS);
	}
}

#endif	//CLIENT_DLL

//=============================================================================
// RPG Weapon
//=============================================================================

LINK_ENTITY_TO_CLASS(weapon_rpg_hl1, CWeaponHL1RPG);

PRECACHE_WEAPON_REGISTER(weapon_rpg_hl1);

//IMPLEMENT_SERVERCLASS_ST( CWeaponHL1RPG, DT_WeaponHL1RPG )
//END_SEND_TABLE()

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponHL1RPG, DT_WeaponHL1RPG)

BEGIN_DATADESC(CWeaponHL1RPG)
DEFINE_FIELD(m_bIntialStateUpdate, FIELD_BOOLEAN),
DEFINE_FIELD(m_bGuiding, FIELD_BOOLEAN),
#ifndef CLIENT_DLL
DEFINE_FIELD(m_hLaserHL1Dot, FIELD_EHANDLE),
#endif
DEFINE_FIELD(m_hMissile, FIELD_EHANDLE),
DEFINE_FIELD(m_bLaserHL1DotSuspended, FIELD_BOOLEAN),
DEFINE_FIELD(m_flLaserHL1DotReviveTime, FIELD_TIME),
END_DATADESC()


BEGIN_NETWORK_TABLE(CWeaponHL1RPG, DT_WeaponHL1RPG)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bIntialStateUpdate)),
RecvPropBool(RECVINFO(m_bGuiding)),
RecvPropBool(RECVINFO(m_bLaserHL1DotSuspended)),
//	RecvPropEHandle( RECVINFO( m_hMissile ), RecvProxy_MissileDied ),
//	RecvPropVector( RECVINFO( m_vecLaserHL1Dot ) ),
#else
SendPropBool(SENDINFO(m_bIntialStateUpdate)),
SendPropBool(SENDINFO(m_bGuiding)),
SendPropBool(SENDINFO(m_bLaserHL1DotSuspended)),
//	SendPropEHandle( SENDINFO( m_hMissile ) ),
//	SendPropVector( SENDINFO( m_vecLaserHL1Dot ) ),
#endif
END_NETWORK_TABLE()


BEGIN_PREDICTION_DATA(CWeaponHL1RPG)
#ifdef CLIENT_DLL
DEFINE_PRED_FIELD(m_bIntialStateUpdate, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_bGuiding, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_bLaserHL1DotSuspended, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_flLaserHL1DotReviveTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
#endif
END_PREDICTION_DATA()


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponHL1RPG::CWeaponHL1RPG(void)
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = true;
	m_bGuiding = true;
	m_bIntialStateUpdate = false;
	m_bLaserHL1DotSuspended = false;
}


CWeaponHL1RPG::~CWeaponHL1RPG()
{
#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		UTIL_Remove(m_hLaserHL1Dot);
		m_hLaserHL1Dot = NULL;
	}
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHL1RPG::ItemPostFrame(void)
{
	BaseClass::ItemPostFrame();

	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	//If we're pulling the weapon out for the first time, wait to draw the laser
	if (m_bIntialStateUpdate)
	{
		if (GetActivity() != ACT_VM_DRAW)
		{
			if (IsGuiding() && !m_bLaserHL1DotSuspended)
			{
#ifndef CLIENT_DLL
				if (m_hLaserHL1Dot != NULL)
				{
					m_hLaserHL1Dot->TurnOn();
				}
#endif
			}

			m_bIntialStateUpdate = false;
		}
		else
		{
			return;
		}
	}

	//Player has toggled guidance state
	if (pPlayer->m_afButtonPressed & IN_ATTACK2)
	{
		if (IsGuiding())
		{
			StopGuiding();
		}
		else
		{
			StartGuiding();
		}
	}

	//Move the laser
	UpdateSpot();
}


void CWeaponHL1RPG::Drop(const Vector &vecVelocity)
{
	StopGuiding();

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		UTIL_Remove(m_hLaserHL1Dot);
		m_hLaserHL1Dot = NULL;
	}
#endif

	BaseClass::Drop(vecVelocity);
}


int CWeaponHL1RPG::GetDefaultClip1(void) const
{
	if (g_pGameRules->IsMultiplayer())
	{
		// more default ammo in multiplay. 
		return BaseClass::GetDefaultClip1() * 2;
	}
	else
	{
		return BaseClass::GetDefaultClip1();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHL1RPG::Precache(void)
{
#ifndef CLIENT_DLL
	UTIL_PrecacheOther("laser_spot");
	UTIL_PrecacheOther("rpg_rocket");
#endif

	//	PrecacheModel( RPG_LASER_SPRITE );
	PrecacheModel("sprites/redglow_mp1.vmt");

	BaseClass::Precache();
}


bool CWeaponHL1RPG::Deploy(void)
{
	m_bIntialStateUpdate = true;
	m_bLaserHL1DotSuspended = false;
	CreateLaserPointer();
#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOff();
	}
#endif

	if (m_iClip1 <= 0)
	{
		return DefaultDeploy((char*)GetViewModel(), (char*)GetWorldModel(), ACT_RPG_DRAW_UNLOADED, (char*)GetAnimPrefix());
	}

	return DefaultDeploy((char*)GetViewModel(), (char*)GetWorldModel(), ACT_VM_DRAW, (char*)GetAnimPrefix());
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponHL1RPG::PrimaryAttack(void)
{
	// Can't have an active missile out
	if (m_hMissile != NULL)
		return;

	// Can't be reloading
	if (GetActivity() == ACT_VM_RELOAD)
		return;

	if (m_iClip1 <= 0)
	{
		if (!m_bFireOnEmpty)
		{
			Reload();
		}
		else
		{
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
		}
	}

	Vector vecOrigin;
	Vector vecForward;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	WeaponSound(SINGLE);
#ifndef CLIENT_DLL
	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 400, 0.2);
#endif
	pOwner->DoMuzzleFlash();

#ifndef CLIENT_DLL
	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime(gpGlobals->curtime + 0.5);
#endif

	Vector	vForward, vRight, vUp;

	pOwner->EyeVectors(&vForward, &vRight, &vUp);

	Vector	muzzlePoint = pOwner->Weapon_ShootPosition() + vForward * 16.0f + vRight * 8.0f + vUp * -8.0f;

#ifndef CLIENT_DLL
	QAngle vecAngles;
	VectorAngles(vForward, vecAngles);

	CRpgRocket * pMissile = CRpgRocket::Create(muzzlePoint, vecAngles, pOwner);
	pMissile->m_hOwner = this;
	pMissile->SetAbsVelocity(pMissile->GetAbsVelocity() + vForward * DotProduct(pOwner->GetAbsVelocity(), vForward));

	m_hMissile = pMissile;
#endif

	pOwner->ViewPunch(QAngle(-5, 0, 0));

	m_iClip1--;

	m_flNextPrimaryAttack = gpGlobals->curtime + 1.5;
	SetWeaponIdleTime(1.5);

	UpdateSpot();
}


void CWeaponHL1RPG::WeaponIdle(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner == NULL)
		return;

	if (!HasWeaponIdleTimeElapsed())
		return;

	int iAnim;
	float flRand = random->RandomFloat(0, 1);
	if (flRand <= 0.75 || IsGuiding())
	{
		if (m_iClip1 <= 0)
			iAnim = ACT_RPG_IDLE_UNLOADED;
		else
			iAnim = ACT_VM_IDLE;
	}
	else
	{
		if (m_iClip1 <= 0)
			iAnim = ACT_RPG_FIDGET_UNLOADED;
		else
			iAnim = ACT_VM_FIDGET;
	}

	SendWeaponAnim(iAnim);
}


void CWeaponHL1RPG::NotifyRocketDied(void)
{
	m_hMissile = NULL;

	// Can't be reloading
	if (GetActivity() == ACT_VM_RELOAD)
		return;

	//	Reload();
}


bool CWeaponHL1RPG::Reload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

#if 0
	if (pOwner == NULL)
		return false;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	WeaponSound(RELOAD);

	SendWeaponAnim(ACT_VM_RELOAD);

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOff();
	}
#endif

	m_bLaserHL1DotSuspended = true;
	m_flLaserHL1DotReviveTime = gpGlobals->curtime + 2.1;
	m_flNextPrimaryAttack = gpGlobals->curtime + 2.1;
	m_flNextSecondaryAttack = gpGlobals->curtime + 2.1;

	return true;
#endif

	// Can't be reloading
	if (GetActivity() == ACT_VM_RELOAD)
		return false;

	if (pOwner == NULL)
		return false;

	if (m_iClip1 > 0)
		return false;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	// because the RPG waits to autoreload when no missiles are active while  the LTD is on, the
	// weapons code is constantly calling into this function, but is often denied because 
	// a) missiles are in flight, but the LTD is on
	// or
	// b) player is totally out of ammo and has nothing to switch to, and should be allowed to
	//    shine the designator around
	//
	// Set the next attack time into the future so that WeaponIdle will get called more often
	// than reload, allowing the RPG LTD to be updated

	if ((m_hMissile != NULL) && IsGuiding())
	{
		// no reloading when there are active missiles tracking the designator.
		// ward off future autoreload attempts by setting next attack time into the future for a bit. 
		return false;
	}

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOff();
	}
#endif

	m_bLaserHL1DotSuspended = true;
	m_flLaserHL1DotReviveTime = gpGlobals->curtime + 2.1;
	m_flNextSecondaryAttack = gpGlobals->curtime + 2.1;

	return DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
}


bool CWeaponHL1RPG::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	// can't put away while guiding a missile.
	if (IsGuiding() && (m_hMissile != NULL))
		return false;

	//	StopGuiding();

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOff();
		UTIL_Remove(m_hLaserHL1Dot);
		m_hLaserHL1Dot = NULL;
	}
#endif

	m_bLaserHL1DotSuspended = false;

	return BaseClass::Holster(pSwitchingTo);
}


void CWeaponHL1RPG::UpdateSpot(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	CreateLaserPointer();

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot == NULL)
		return;
#endif

	if (IsGuiding() && m_bLaserHL1DotSuspended && (m_flLaserHL1DotReviveTime <= gpGlobals->curtime))
	{
#ifndef CLIENT_DLL
		m_hLaserHL1Dot->TurnOn();
#endif
		m_bLaserHL1DotSuspended = false;
	}

	//Move the laser dot, if active
	trace_t	tr;
	Vector	muzzlePos = pPlayer->Weapon_ShootPosition();

	Vector	forward;
	AngleVectors(pPlayer->EyeAngles() + pPlayer->m_Local.m_vecPunchAngle, &forward);

	Vector	endPos = muzzlePos + (forward * MAX_TRACE_LENGTH);

	// Trace out for the endpoint
	UTIL_TraceLine(muzzlePos, endPos, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

	// Move the laser sprite
	Vector	laserPos = tr.endpos + (tr.plane.normal * 2.0f);
#ifndef CLIENT_DLL
	m_hLaserHL1Dot->SetLaserPosition(laserPos, tr.plane.normal);
#endif
}


void CWeaponHL1RPG::CreateLaserPointer(void)
{
#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
		return;

	m_hLaserHL1Dot = CLaserHL1Dot::Create(GetAbsOrigin(), GetOwner());
	if (!IsGuiding())
	{
		if (m_hLaserHL1Dot)
		{
			m_hLaserHL1Dot->TurnOff();
		}
	}
#endif
}


bool CWeaponHL1RPG::IsGuiding(void)
{
	return m_bGuiding;
}


void CWeaponHL1RPG::StartGuiding(void)
{
	m_bGuiding = true;

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOn();
	}
#endif

	UpdateSpot();
}

void CWeaponHL1RPG::StopGuiding(void)
{
	m_bGuiding = false;

#ifndef CLIENT_DLL
	if (m_hLaserHL1Dot != NULL)
	{
		m_hLaserHL1Dot->TurnOff();
	}
#endif
}
