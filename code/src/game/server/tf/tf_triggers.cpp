//===== NOT Copyright ฉ 1996-2005, Valve Corporation, All rights reserved. =====//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "tf_triggers.h"
#include "tf_player.h"
#include "ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CTriggerAddTFPlayerCondition )
	DEFINE_KEYFIELD( m_flDuration, FIELD_FLOAT, "duration" ),
	DEFINE_KEYFIELD( m_nCondition,	FIELD_INTEGER,	"condition" ),
END_DATADESC()


LINK_ENTITY_TO_CLASS( trigger_add_tf_player_condition, CTriggerAddTFPlayerCondition );

//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//-----------------------------------------------------------------------------
void CTriggerAddTFPlayerCondition::Spawn( void )
{
	BaseClass::Spawn();
	Precache();

	m_flDuration = 0.0f;
	m_nCondition = 0;
	InitTrigger();
}

void CTriggerAddTFPlayerCondition::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAddTFPlayerCondition::Touch( CBaseEntity *pOther )
{	
	if (!PassesTriggerFilters(pOther))
		return;

	m_nCondition = 0;
	m_flDuration = 0.0f;

	// If we added him to our list, store the start time
	EHANDLE hOther;
	hOther = pOther;

	if ( pOther->IsPlayer() )
	{
		CTFPlayer *pPlayer = dynamic_cast<CTFPlayer*>( pOther );

		if ( m_flDuration == -1 )
		{
			pPlayer->m_Shared.AddCond( m_nCondition );
		}
		else
		{
			pPlayer->m_Shared.AddCond( m_nCondition, m_flDuration );
		}
	}
	else if ( pOther->IsNPC() )
	{
		CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC *>( pOther );

		if ( m_flDuration == -1 )
		{
			pNPC->AddCond( m_nCondition );
		}
		else
		{
			pNPC->AddCond( m_nCondition, m_flDuration );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity starts touching us.
// Input  : pOther - The entity that is touching us.
//-----------------------------------------------------------------------------
void CTriggerAddTFPlayerCondition::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );

	m_nCondition = 0;
	m_flDuration = 0.0f;

	// If we added him to our list, store the start time
	EHANDLE hOther;
	hOther = pOther;

	if ( pOther->IsPlayer() )
	{
		CTFPlayer *pPlayer = dynamic_cast<CTFPlayer*>( pOther );

		if ( m_flDuration == -1 )
		{
			pPlayer->m_Shared.AddCond( m_nCondition );
		}
		else
		{
			pPlayer->m_Shared.AddCond( m_nCondition, m_flDuration );
		}
	}
	else if ( pOther->IsNPC() )
	{
		CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC *>( pOther );

		if ( m_flDuration == -1 )
		{
			pNPC->AddCond( m_nCondition );
		}
		else
		{
			pNPC->AddCond( m_nCondition, m_flDuration );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CTriggerAddTFPlayerCondition::EndTouch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() )
	{
		CTFPlayer *pPlayer = dynamic_cast<CTFPlayer*>( pOther );

		if ( m_flDuration == -1 )
		{
			if ( pPlayer )
			{
				if ( !(pPlayer->GetFlags() & FL_DONTTOUCH ) )
					  pPlayer->m_Shared.RemoveCond( m_nCondition );
			}
		}
	}
	else if ( pOther->IsNPC() )
	{
		CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC*>( pOther );

		if ( m_flDuration == -1 )
		{
			if ( pNPC )
			{
				if ( !(pNPC->GetFlags() & FL_DONTTOUCH ) )
					  pNPC->RemoveCond( m_nCondition );
			}
		}
	}

	BaseClass::EndTouch( pOther );
}

BEGIN_DATADESC( CTriggerRemoveTFPlayerCondition )
	DEFINE_KEYFIELD( m_nCondition,	FIELD_INTEGER,	"condition" ),
END_DATADESC()


LINK_ENTITY_TO_CLASS( trigger_remove_tf_player_condition, CTriggerRemoveTFPlayerCondition );

//-----------------------------------------------------------------------------
// Purpose: Called when spawning, after keyvalues have been handled.
//-----------------------------------------------------------------------------
void CTriggerRemoveTFPlayerCondition::Spawn( void )
{
	BaseClass::Spawn();
	Precache();

	m_nCondition = 0;
	InitTrigger();
}

void CTriggerRemoveTFPlayerCondition::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerRemoveTFPlayerCondition::Touch( CBaseEntity *pOther )
{	
	if (!PassesTriggerFilters(pOther))
		return;

	m_nCondition = 0;

	// If we added him to our list, store the start time
	EHANDLE hOther;
	hOther = pOther;

	if ( pOther->IsPlayer() )
	{
		CTFPlayer *pPlayer = dynamic_cast<CTFPlayer*>( pOther );

		if ( pPlayer )
		{
			if ( !(pPlayer->GetFlags() & FL_DONTTOUCH ) )
				  pPlayer->m_Shared.RemoveCond( m_nCondition );
		}
	}
	else if ( pOther->IsNPC() )
	{
		CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC*>( pOther );

		if ( pNPC )
		{
			if ( !(pNPC->GetFlags() & FL_DONTTOUCH ) )
				  pNPC->RemoveCond( m_nCondition );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when an entity starts touching us.
// Input  : pOther - The entity that is touching us.
//-----------------------------------------------------------------------------
void CTriggerRemoveTFPlayerCondition::StartTouch( CBaseEntity *pOther )
{
	BaseClass::StartTouch( pOther );

	m_nCondition = 0;

	// If we added him to our list, store the start time
	EHANDLE hOther;
	hOther = pOther;

	if ( pOther->IsPlayer() )
	{
		CTFPlayer *pPlayer = dynamic_cast<CTFPlayer*>( pOther );

		if ( pPlayer )
		{
			if ( !(pPlayer->GetFlags() & FL_DONTTOUCH ) )
				  pPlayer->m_Shared.RemoveCond( m_nCondition );
		}
	}
	else if ( pOther->IsNPC() )
	{
		CAI_BaseNPC *pNPC = dynamic_cast<CAI_BaseNPC*>( pOther );

		if ( pNPC )
		{
			if ( !(pNPC->GetFlags() & FL_DONTTOUCH ) )
				  pNPC->RemoveCond( m_nCondition );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called when an entity stops touching us.
// Input  : pOther - The entity that was touching us.
//-----------------------------------------------------------------------------
void CTriggerRemoveTFPlayerCondition::EndTouch( CBaseEntity *pOther )
{
	BaseClass::EndTouch( pOther );
}