//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interface that entities can use to redirect scoring to other entities.
//			i.e. A rocket redirects scoring to the player that fired it.
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISCORER_H
#define ISCORER_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: Interface that entities can use to redirect scoring to other entities.
//			i.e. A rocket redirects scoring to the player that fired it.
//-----------------------------------------------------------------------------
abstract_class IScorer
{
public:
#ifndef TF_CLASSIC
	// Return the entity that should receive the score
	virtual CBasePlayer *GetScorer( void ) = 0;
	// Return the entity that should get assistance credit
	virtual CBasePlayer *GetAssistant( void ) = 0;
#else // replace with cbaseentity because we have npcs.
	// Return the entity that should receive the score
	virtual CBaseEntity *GetScorer( void ) = 0;
	// Return the entity that should get assistance credit
	virtual CBaseEntity *GetAssistant( void ) = 0;
#endif
};


#endif // ISCORER_H
