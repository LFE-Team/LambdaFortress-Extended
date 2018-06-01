//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef NPC_ROLLERMINERED_H
#define NPC_ROLLERMINERED_H
#ifdef _WIN32
#pragma once
#endif

//------------------------------------
// Spawnflags
//------------------------------------
#define SF_ROLLERMINERED_FRIENDLY		(1 << 16)
#define SF_ROLLERMINERED_PROP_COLLISION		(1 << 17)

bool NPC_RollermineRed_IsRollermineRed( CBaseEntity *pEntity );
CBaseEntity *NPC_RollermineRed_DropFromPoint( const Vector &originStart, CBaseEntity *pOwner, const char *pszTemplate );

#endif // NPC_ROLLERMINERED_H
