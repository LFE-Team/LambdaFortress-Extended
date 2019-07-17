//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef FUNC_FORCEFIELD_H
#define FUNC_FORCEFIELD_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "modelentities.h"
#include "triggers.h"

//-----------------------------------------------------------------------------
// Purpose: Visualizes a respawn room to the enemy team
//-----------------------------------------------------------------------------
DECLARE_AUTO_LIST(IFuncForceFieldAutoList);
class CFuncForceField : public CFuncBrush, public IFuncForceFieldAutoList
{
	DECLARE_CLASS( CFuncForceField, CFuncBrush );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void Spawn( void );

	virtual int		UpdateTransmitState( void );
	virtual int		ShouldTransmit( const CCheckTransmitInfo *pInfo );
	virtual bool	ShouldCollide( int collisionGroup, int contentsMask ) const;

	void SetActive( bool bActive );
};

#endif // FUNC_FORCEFIELD_H