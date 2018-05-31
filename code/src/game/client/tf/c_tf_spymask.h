//=============================================================================
//
// Purpose: 
//
//=============================================================================

#ifndef C_TF_SPYMASK_H
#define C_TF_SPYMASK_H

#ifdef _WIN32
#pragma once
#endif

#include "c_playerattachedmodel.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class C_TFSpyMask : public C_PlayerAttachedModel
{
public:
	DECLARE_CLASS( C_TFSpyMask, C_PlayerAttachedModel );

	C_TFSpyMask();

	static C_TFSpyMask *Create( C_BaseEntity *pOwner );

	bool	ShouldDraw( void );
	int		GetSkin( void );
};

#endif // C_TF_SPYMASK_H
