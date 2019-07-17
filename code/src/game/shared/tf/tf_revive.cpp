//============== Copyright LFE-TEAM Not All rights reserved. ==================//
//
// Purpose: medic become super useful
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "tf_revive.h"

#ifdef CLIENT_DLL
#include "tf_hud_mediccallers.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFReviveMarker, DT_TFReviveMarker )

BEGIN_NETWORK_TABLE( CTFReviveMarker, DT_TFReviveMarker )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iHealth ) ),
	RecvPropInt( RECVINFO( m_iMaxHealth ) ),
	RecvPropEHandle( RECVINFO( m_hOwner ) ),
#else
	SendPropInt( SENDINFO( m_iHealth ), 13 ),
	SendPropInt( SENDINFO( m_iMaxHealth ), 13 ),
	SendPropEHandle( SENDINFO( m_hOwner ) ),
#endif
END_NETWORK_TABLE()

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFReviveMarker )
	DEFINE_THINKFUNC( ReviveThink ),
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( entity_revive_marker, CTFReviveMarker );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFReviveMarker::CTFReviveMarker()
{
#ifdef CLIENT_DLL
	m_pReviveMeEffect = NULL;
#endif
	m_iHealth = 0;
	m_iMaxHealth = 1;
	m_hOwner = NULL;
}
//-----------------------------------------------------------------------------
// Purpose: Deconstructor.
//-----------------------------------------------------------------------------
CTFReviveMarker::~CTFReviveMarker()
{
#ifdef CLIENT_DLL
	m_pReviveMeEffect = NULL;
#endif
	m_hOwner = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::Spawn()
{
	BaseClass::Spawn();

	SetModel( TF_REVIVEMARKER_MODEL );
	AddEffects( EF_NOSHADOW );
	ResetSequence( LookupSequence("idle") );

	#ifdef GAME_DLL
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	SetElasticity( 0.0f );
	SetGravity( 1.0f );
	m_iHealth = 0;
	#endif

	SetCollisionGroup( COLLISION_GROUP_WEAPON ); //
	SetSolid( SOLID_BBOX );
	SetSolidFlags( /*FSOLID_NOT_SOLID |*/ FSOLID_TRIGGER );

	//m_takedamage = DAMAGE_NO;

	// Setup the think function.
	SetThink( &CTFReviveMarker::ReviveThink );
	SetNextThink( gpGlobals->curtime );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::Precache()
{
	// Precache the models.
	PrecacheModel( TF_REVIVEMARKER_MODEL );

	// Precache the sounds.
	PrecacheScriptSound( "MVM.PlayerRevived" );

	// Precache the particles.
	PrecacheParticleSystem( "speech_revivecall" );
	PrecacheParticleSystem( "speech_revivecall_medium" );
	PrecacheParticleSystem( "speech_revivecall_hard" );
					 
	BaseClass::Precache();
}
#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFReviveMarker::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFReviveMarker::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	return FL_EDICT_ALWAYS;
}


CTFReviveMarker *CTFReviveMarker::Create( CTFPlayer *pOwner )
{
	CTFReviveMarker *pReviveMarker = static_cast<CTFReviveMarker *>( CreateEntityByName( "entity_revive_marker" ) );
	if ( pReviveMarker )
	{
		UTIL_SetOrigin( pReviveMarker, pOwner->GetAbsOrigin() );
		pReviveMarker->SetAbsAngles( pOwner->GetAbsAngles() );
		pReviveMarker->Spawn();
		pReviveMarker->SetOwner( pOwner );

		pReviveMarker->m_iHealth = 0;
		pReviveMarker->m_iMaxHealth = pOwner->GetMaxHealth();
		pReviveMarker->ChangeTeam( pOwner->GetTeamNumber() );

		//pReviveMarker->ResetSequence( pReviveMarker->LookupSequence("idle") );

		DispatchSpawn( pReviveMarker );
	}

	return pReviveMarker;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFReviveMarker::AddMarkerHealth( float flAmount )
{
	if (!m_hOwner)
	{
		Remove();
		return;
	}
	float flHealFraction = 0;
	float fTotalHealAmount = 0.0f;

	flHealFraction += gpGlobals->frametime * flAmount;
	fTotalHealAmount += flAmount;

	int nHealthToAdd = (int)flHealFraction;
	if ( nHealthToAdd > 0 )
	{
		flHealFraction -= nHealthToAdd;
		TakeHealth( nHealthToAdd, DMG_GENERIC );
	}
}

#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::ReviveOwner( void )
{
	#ifdef GAME_DLL
	if (!m_hOwner)
	{
		Remove();
		return;
	}
	m_hOwner->AllowInstantSpawn();
	m_hOwner->ForceRespawn();
	m_hOwner->EmitSound( "MVM.PlayerRevived" );
	m_hOwner->SpeakConceptIfAllowed( MP_CONCEPT_RESURRECTED );
	#else
	if ( m_pReviveMeEffect )
	{
		ParticleProp()->StopEmissionAndDestroyImmediately( m_pReviveMeEffect );
		m_pReviveMeEffect = NULL;
	}
	#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFReviveMarker::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	if ( collisionGroup == COLLISION_GROUP_NPC )
		return false;

	if ( collisionGroup == COLLISION_GROUP_PROJECTILE )
		return false;

	if ( collisionGroup == COLLISION_GROUP_WEAPON )
		return false;

	return BaseClass::ShouldCollide( collisionGroup, contentsMask );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::ReviveThink( void )
{
#ifdef GAME_DLL
	if (!m_hOwner)
	{
		UTIL_Remove(this);
		return;
	}
#endif

	/*if ( IsReviveInProgress() )
		PromptOwner();*/

#ifdef GAME_DLL
		// only red team has the revive markers
	if (m_hOwner && m_hOwner->GetTeamNumber() == TEAM_SPECTATOR)
		UTIL_Remove(this);
#endif

	if ( GetHealth() == GetMaxHealth() )
		ReviveOwner();

	SetNextThink( gpGlobals->curtime );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFReviveMarker::IsReviveInProgress( void )
{
	int iOldHealth = m_iHealth;
	if ( m_iHealth != iOldHealth )
		return true;

	return false;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFReviveMarker::CreateReviveMeEffect( void )
{
	if (!m_hOwner)
	{
		Remove();
		return;
	}
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	// Only show the bubble to teammates and players who are on the our disguise team.
	if ( GetTeamNumber() != pLocalPlayer->GetTeamNumber() )
		return;

	if ( m_pReviveMeEffect )
	{
		ParticleProp()->StopEmission( m_pReviveMeEffect );
		m_pReviveMeEffect = NULL;
	}

	//check TWICE because this crash is driving me insane
	if (!m_hOwner)
	{
		Remove();
		return;
	}

	if ( m_hOwner->GetDeathTime() > 8.0f )
		m_pReviveMeEffect = ParticleProp()->Create( "speech_revivecall_medium", PATTACH_POINT_FOLLOW, "mediccall" );
	else if ( m_hOwner->GetDeathTime() > 4.0f )
		m_pReviveMeEffect = ParticleProp()->Create( "speech_revivecall_hard", PATTACH_POINT_FOLLOW, "mediccall" );
	else
		m_pReviveMeEffect = ParticleProp()->Create( "speech_revivecall", PATTACH_POINT_FOLLOW, "mediccall" );

	// If the local player is a medic, add this player to our list of medic callers
	if ( pLocalPlayer && pLocalPlayer->IsPlayerClass( TF_CLASS_MEDIC ) && pLocalPlayer->IsAlive() == true )
	{
		Vector vecPos;
		if ( GetAttachmentLocal( LookupAttachment( "mediccall" ), vecPos ) )
		{
			vecPos += Vector(0,0,18);	// Particle effect is 18 units above the attachment
			CTFMedicCallerPanel::AddMedicCaller( this, 5.0, vecPos );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFReviveMarker::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );

		CreateReviveMeEffect();
	}

	BaseClass::OnDataChanged( updateType );
}
#else
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::SetOwner( CTFPlayer *pOwner )
{
	m_hOwner = pOwner;
}
#endif