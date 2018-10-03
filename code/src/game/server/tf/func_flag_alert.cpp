//======= Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: CTF Flag Alert trigger.
//
//=============================================================================//

#include "cbase.h"
#include "func_flag_alert.h"
#include "entity_capture_flag.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


BEGIN_DATADESC( CFuncFlagAlertZone )

	DEFINE_KEYFIELD( m_bPlaySound, FIELD_BOOLEAN, "playsound" ),
	DEFINE_KEYFIELD( m_nAlertDelay, FIELD_FLOAT, "alert_delay" ),

	// Outputs.
	DEFINE_OUTPUT( m_OnTriggeredByTeam1, "OnTriggeredByTeam1" ),
	DEFINE_OUTPUT( m_OnTriggeredByTeam2, "OnTriggeredByTeam2" ),

END_DATADESC()

IMPLEMENT_AUTO_LIST( IFlagAlertZoneAutoList );

LINK_ENTITY_TO_CLASS( func_flagdetectionzone, CFuncFlagAlertZone );


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::Spawn( void )
{
	BaseClass::Spawn();

	AddSpawnFlags( SF_TRIGGER_ALLOW_CLIENTS | SF_TRIGGER_ALLOW_NPCS );

	InitTrigger();

	if ( m_bDisabled )
	{
		SetDisabled( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::SetDisabled( bool bDisabled )
{
	m_bDisabled = bDisabled;

	if ( bDisabled )
	{
		BaseClass::Disable();
		//SetTouch( NULL );
	}
	else
	{
		BaseClass::Enable();
		//SetTouch( &CFuncFlagAlertZone::Touch );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::InputEnable( inputdata_t &inputdata )
{
	SetDisabled( false );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::InputDisable( inputdata_t &inputdata )
{
	SetDisabled( true );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CFuncFlagAlertZone::EntityIsFlagCarrier( CBaseEntity *pEntity )
{
	CTFPlayer *pPlayer = ToTFPlayer( pEntity );
	//CAI_BaseNPC *pNPC = pEntity->MyNPCPointer();
	if ( pPlayer && pPlayer->HasItem() )
	{
		CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag*>( pPlayer->GetItem() );
		if ( pFlag )
			return true;
	}
	//else if ( pNPC && pNPC->HasItem() )
	//{
	//	CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag*>( pNPC->GetItem() );
	//	if ( pFlag )
	//		return true;
	//}
	
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::FlagCaptured( CBaseEntity *pPlayer )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void HandleFlagCapturedInAlertZone( CBaseEntity *pPlayer )
{
	for ( int i = 0; i < IFlagAlertZoneAutoList::AutoList().Count(); i++ )
	{
		CFuncFlagAlertZone *pZone = static_cast< CFuncFlagAlertZone *>( IFlagAlertZoneAutoList::AutoList()[i] );
		if ( pZone && !pZone->IsDisabled() )
		{
			pZone->FlagCaptured( pPlayer );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::StartTouch( CBaseEntity *pOther )
{
	if ( IsDisabled() || !pOther )
		return;

	BaseClass::StartTouch( pOther );

	if ( IsTouching( pOther ) && EntityIsFlagCarrier( pOther ) )
	{
		switch ( pOther->GetTeamNumber() )
		{
		case TF_TEAM_RED:
			m_OnTriggeredByTeam1.FireOutput( this, this );
			break;
		case TF_TEAM_BLUE:
			m_OnTriggeredByTeam2.FireOutput( this, this );
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::EndTouch( CBaseEntity *pOther )
{
	if ( !pOther )
		return;

	BaseClass::EndTouch( pOther );
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFuncFlagAlertZone::FlagPickedUp( CBaseEntity *pPlayer )
{
	if ( pPlayer && IsTouching( pPlayer ) )
	{
		switch ( pPlayer->GetTeamNumber() )
		{
		case TF_TEAM_RED:
			m_OnTriggeredByTeam1.FireOutput( this, this );
			break;
		case TF_TEAM_BLUE:
			m_OnTriggeredByTeam2.FireOutput( this, this );
			break;
		}
	};
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void HandleFlagPickedUpInAlertZone( CBaseEntity *pPlayer )
{
	for ( int i = 0; i < IFlagAlertZoneAutoList::AutoList().Count(); i++ )
	{
		CFuncFlagAlertZone *pZone = static_cast< CFuncFlagAlertZone *>( IFlagAlertZoneAutoList::AutoList()[i] );
		if ( pZone && !pZone->IsDisabled() )
		{
			pZone->FlagPickedUp( pPlayer );
		}
	}
}