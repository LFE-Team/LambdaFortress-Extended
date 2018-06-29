//===================== 2018 ISPuddy reversed engineering =====================//
// Reverse Engie Training Annotations
// An in-world location-specific information bubble.
//=============================================================================//
#include "cbase.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_training_annotations.h"
#include "basecombatcharacter.h"
#include "in_buttons.h"
#include "tf_fx.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( CTrainingAnnotation )
	DEFINE_KEYFIELD( m_displayText, FIELD_STRING, "display_text" ),
	DEFINE_KEYFIELD( m_flLifetime , FIELD_TIME, "lifetime" ),
	DEFINE_KEYFIELD( m_flVerticalOffset , FIELD_FLOAT, "offset" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "Show", InputShow ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Hide", InputHide ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( training_annotation, CTrainingAnnotation );


CTrainingAnnotation::CTrainingAnnotation()
{
	//m_displayText = "";
	m_flLifetime = 10.0f;
	m_flVerticalOffset = 0.0f;
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTrainingAnnotation::Spawn( void )
{
	Precache();
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function 
//-----------------------------------------------------------------------------
void CTrainingAnnotation::Precache( void )
{
}


void CTrainingAnnotation::InputShow( inputdata_t &inputdata )
{
	Show( inputdata.pActivator );
}

void CTrainingAnnotation::InputHide( inputdata_t &inputdata )
{
	Hide( inputdata.pActivator );
}

void CTrainingAnnotation::Show( CBaseEntity *pActivator )
{
	IGameEvent *pEvent = gameeventmanager->CreateEvent( "show_annotation" );
	if ( pEvent )
	{
		pEvent->SetInt( "id", entindex() );
		//pEvent->SetString( "text", m_displayText );
		pEvent->SetInt( "lifetime", m_flLifetime );
		//pEvent->SetInt( "v", m_flVerticalOffset );
		pEvent->SetString( "play_sound", NULL );
		//pEvent->SetInt( "worldPosX", GetAbsAngles() );
		//pEvent->SetInt( "worldPosY", GetAbsOrigin() );
		//pEvent->SetInt( "worldPosZ", GetAbsOrigin() );
		gameeventmanager->FireEventClientSide( pEvent );
	}
}

void CTrainingAnnotation::Hide( CBaseEntity *pActivator )
{
	IGameEvent *pEvent = gameeventmanager->CreateEvent( "hide_annotation" );
	if ( pEvent )
	{
		pEvent->SetInt( "id", entindex() );
		gameeventmanager->FireEventClientSide( pEvent );
	}
}