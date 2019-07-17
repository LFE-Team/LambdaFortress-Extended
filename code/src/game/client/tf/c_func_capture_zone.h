//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_FUNC_CAPTURE_ZONE_H
#define C_FUNC_CAPTURE_ZONE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_CaptureZone : public C_BaseEntity
{
	DECLARE_CLASS( C_CaptureZone, C_BaseEntity );

public:
	DECLARE_CLIENTCLASS();

	void Spawn( void );
};

#endif // C_FUNC_CAPTURE_ZONE_H