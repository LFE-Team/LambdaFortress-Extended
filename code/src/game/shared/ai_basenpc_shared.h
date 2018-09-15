//=============================================================================//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef AI_BASENPC_SHARED_H
#define AI_BASENPC_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CAI_BaseNPC C_AI_BaseNPC
#endif

class CAI_BaseNPC;

#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
//-------------------------------------
// TF2 flags
//-------------------------------------
#define TFFL_MECH					( 1 << 0 ) // show "gears" icon in Target ID
#define TFFL_NOBACKSTAB				( 1 << 1 )
#define TFFL_NOHEALING				( 1 << 2 )
#define TFFL_FIREPROOF				( 1 << 3 )
#define TFFL_NODEFLECT				( 1 << 4 )
#define TFFL_NOJAR					( 1 << 6 )
#define TFFL_NODEATHNOTICE			( 1 << 7 )
#define TFFL_NOREWARD				( 1 << 8 )
#define TFFL_NOBLEED				( 1 << 9 )

#define TFFL_BUILDING				( TFFL_MECH | TFFL_NOBACKSTAB | TFFL_NOHEALING | TFFL_FIREPROOF | TFFL_NOJAR | TFFL_NOBLEED )

struct TF_NPCData
{
	const char *pszName;
	int iTeam;
	int nFlags;
};

extern TF_NPCData g_aNPCData[];
#endif

// Shared header file for NPCs
#if defined( CLIENT_DLL )
#include "c_ai_basenpc.h"
#else
#include "ai_basenpc.h"
#endif

#endif // AI_BASENPC_SHARED_H
