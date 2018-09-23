//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
//
//=============================================================================//
#ifndef TF_ITEM_H
#define TF_ITEM_H
#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
#include "c_props.h"
#else
#include "props.h"
#endif

#ifdef CLIENT_DLL
#define CTFItem C_TFItem
#endif

//=============================================================================
//
// TF Item
//
class CTFItem : public CDynamicProp
{
public:
	DECLARE_CLASS( CTFItem, CDynamicProp  )
	DECLARE_NETWORKCLASS();

	// Unique identifier.
	virtual unsigned int GetItemID();
	
	// Pick up and drop.
	virtual void PickUp( CBaseEntity *pPlayer, bool bInvisible );
	virtual void Drop( CBaseEntity *pPlayer, bool bVisible, bool bThrown = false, bool bMessage = true );

#ifdef CLIENT_DLL
	virtual bool ShouldDraw();
	virtual ShadowType_t ShadowCastType();
#endif
};

#endif // TF_ITEM_H