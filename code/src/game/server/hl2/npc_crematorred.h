//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef NPC_CREMATORREDHL1_H
#define NPC_CREMATORREDHL1_H

#include "ai_basenpc.h"
#include "npc_cremator.h"
//=========================================================
//=========================================================
class CNPC_CrematorRed : public CNPC_Cremator
{
	DECLARE_CLASS( CNPC_CrematorRed, CAI_BaseNPC );
public:
	void Spawn( void );
	void Precache( void );
	Class_T	Classify ( void );
};


#endif //NPC_CREMATORREDHL1_H