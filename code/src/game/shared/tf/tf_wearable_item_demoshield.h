//=============================================================================
//
// Purpose: whatever this is gonna be if i ever want to add it.
//
//=============================================================================
#ifndef TF_WEARABLE_H
#define TF_WEARABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "econ_wearable.h"

#ifdef CLIENT_DLL
#define CTFWearableDemoShield C_TFWearableDemoShield
#endif

class CTFWearableDemoShield : public CEconWearable
{
public:
	DECLARE_CLASS( CTFWearableDemoShield, CEconWearable );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	virtual void	Equip( CBasePlayer *pPlayer );
	void			UpdateModelToClass( void );
#endif
};

#endif // TF_WEARABLE_H
