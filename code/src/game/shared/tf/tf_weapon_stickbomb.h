//==================== sigsegv-mvm's reversed engineering =====================
//
// Purpose: Demo's Caber
//
//=============================================================================

#ifndef TF_WEAPON_STICKBOMB_H
#define TF_WEAPON_STICKBOMB_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weapon_bottle.h"

#ifdef CLIENT_DLL
#define CTFStickBomb C_TFStickBomb
#endif

class CTFStickBomb : public CTFBottle
{
public:
	DECLARE_CLASS(CTFStickBomb, CTFBottle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	
	CTFStickBomb();
	CTFStickBomb( const CTFStickBomb& ) = delete;
	
	virtual int GetWeaponID() const override { return TF_WEAPON_STICKBOMB; }
	virtual const char *GetWorldModel() const override;
	virtual void Precache() override;
	virtual void Smack() override;
	virtual void SwitchBodyGroups() override;
	virtual void WeaponRegenerate() override;
	virtual void WeaponReset() override;
	
#ifdef CLIENT_DLL
	virtual int GetWorldModelIndex() override;
#endif
	
private:
	CNetworkVar( int, m_iDetonated );
};


#endif // TF_WEAPON_STICKBOMB_H