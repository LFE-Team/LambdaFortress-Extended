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
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( entity_revive_marker, CTFReviveMarker );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFReviveMarker::CTFReviveMarker()
{
#ifdef CLIENT_DLL
	m_pReviveMeEffect = NULL;
#endif
}
//-----------------------------------------------------------------------------
// Purpose: Deconstructor.
//-----------------------------------------------------------------------------
CTFReviveMarker::~CTFReviveMarker()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::Spawn()
{
	BaseClass::Spawn();

	SetModel( TF_REVIVEMARKER_MODEL );
	SetHealth( 0 );
	AddEffects( EF_NOSHADOW );
	ResetSequence( LookupSequence("idle") );

	SetContextThink( NULL, 0, "ReviveThink" );
	SetMoveType( MOVETYPE_FLYGRAVITY );

	SetCollisionGroup( COLLISION_GROUP_WEAPON ); //
	SetSolid( SOLID_BBOX );
	SetSolidFlags( /*FSOLID_NOT_SOLID |*/ FSOLID_TRIGGER );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::Precache()
{
	// Precache the player models and gibs.
	PrecacheModel( TF_REVIVEMARKER_MODEL );

	// Precache the player sounds.
	PrecacheScriptSound( "MVM.PlayerRevived" );

	PrecacheParticleSystem( "speech_revivecall" );
	PrecacheParticleSystem( "speech_revivecall_medium" );
	PrecacheParticleSystem( "speech_revivecall_hard" );
					 
	BaseClass::Precache();
}
#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFReviveMarker::ShouldTransmit( const CCheckTransmitInfo *pInfo )
{
	return FL_EDICT_ALWAYS;
}


CTFReviveMarker *CTFReviveMarker::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, int iClass )
{
	CTFReviveMarker *pReviveMarker = static_cast<CTFReviveMarker *>( CBaseAnimating::CreateNoSpawn( "entity_revive_marker", vecOrigin, vecAngles, pOwner ) );
	if ( pReviveMarker )
	{
		pReviveMarker->InitReviveMarker( pOwner, iClass );
		pReviveMarker->SetSequence( pReviveMarker->LookupSequence("idle") );
		pReviveMarker->SetOwnerEntity( pOwner );

		DispatchSpawn( pReviveMarker );
	}

	return pReviveMarker;
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::RevivePlayer( void )
{
	#ifdef GAME_DLL
	CTFPlayer *pPlayer = dynamic_cast < CTFPlayer * >( GetOwnerEntity() );
	pPlayer->AllowInstantSpawn();
	pPlayer->ForceRespawn();
	pPlayer->EmitSound( "MVM.PlayerRevived" );
	pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_RESURRECTED );
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
//-----------------------------------------------------------------------------
void CTFReviveMarker::InitReviveMarker( CBaseEntity *pOwner, int iClass )
{
	#ifdef GAME_DLL
	CTFPlayer *pPlayer = ToTFPlayer( pOwner );
	SetHealth( 1 );
	SetMaxHealth( pPlayer->GetPlayerClass()->GetMaxHealth() );
	ChangeTeam( pPlayer->GetTeamNumber() );

	ResetSequence( LookupSequence("idle") );
	#else
	CreateReviveMeEffect();
	SetBodygroup( FindBodygroupByName( "class" ), iClass );
	#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFReviveMarker::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	return BaseClass::ShouldCollide( collisionGroup, contentsMask );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::ReviveThink( void )
{
	#ifdef GAME_DLL
	TakeHealth( 25, DMG_GENERIC );
	#endif

	if ( GetHealth() == GetMaxHealth() )
	{
		RevivePlayer();
		SetContextThink( NULL, 0, "ReviveThink" );
	}
	SetContextThink( &CTFReviveMarker::ReviveThink, gpGlobals->curtime + 0.1f, "ReviveThink" );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFReviveMarker::CreateReviveMeEffect( void )
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	// Only show the bubble to teammates and players who are on the our disguise team.
	if ( GetTeamNumber() != pLocalPlayer->GetTeamNumber() )
		return;

	if ( m_pReviveMeEffect )
	{
		ParticleProp()->StopEmission( m_pReviveMeEffect );
		m_pReviveMeEffect = NULL;
	}

	m_pReviveMeEffect = ParticleProp()->Create( "speech_revivecall", PATTACH_POINT_FOLLOW, "mediccall" );
	/*if ( m_pReviveMeEffect )
	{
		// Set "redness" of the bubble based on player's health.
		float flHealthRatio = clamp( (float)GetHealth() / (float)GetMaxHealth(), 0.0f, 1.0f );
		m_pReviveMeEffect->SetControlPoint( 1, Vector( flHealthRatio ) );
	}*/

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
#endif