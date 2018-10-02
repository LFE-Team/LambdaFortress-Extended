//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "tf_revive.h"


IMPLEMENT_NETWORKCLASS_ALIASED( TFReviveMarker, DT_TFReviveMarker )

BEGIN_NETWORK_TABLE( CTFReviveMarker, DT_TFReviveMarker )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( entity_revive_marker, CTFReviveMarker );

#define REVIVEMARKER_MODEL "models/props_mvm/mvm_revive_tombstone.mdl"

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFReviveMarker::CTFReviveMarker()
{

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

	SetModel( REVIVEMARKER_MODEL );
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
	PrecacheModel( REVIVEMARKER_MODEL );

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
		pReviveMarker->InitReviveMarker( ToTFPlayer( pOwner ), iClass );
		pReviveMarker->SetSequence( pReviveMarker->LookupSequence("idle") );

		DispatchSpawn( pReviveMarker );
	}

	return pReviveMarker;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::RevivePlayer( void )
{
	CTFPlayer *pPlayer = dynamic_cast < CTFPlayer * >( GetOwnerEntity() );
	pPlayer->ForceRespawn();
	pPlayer->EmitSound( "MVM.PlayerRevived" );
	pPlayer->SpeakConceptIfAllowed( MP_CONCEPT_RESURRECTED );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFReviveMarker::InitReviveMarker( CTFPlayer *pOwner, int iClass )
{
	SetOwnerEntity( pOwner );
	SetHealth( 1 );
	SetMaxHealth( pOwner->GetPlayerClass()->GetMaxHealth() );
	ChangeTeam( pOwner->GetTeamNumber() );
	SetBodygroup( 1, iClass );

	ResetSequence( LookupSequence("idle") );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFReviveMarker::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	if ( collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		switch( GetTeamNumber() )
		{
		case TF_TEAM_RED:
			if ( !( contentsMask & CONTENTS_REDTEAM ) )
				return false;
			break;

		case TF_TEAM_BLUE:
			if ( !( contentsMask & CONTENTS_BLUETEAM ) )
				return false;
			break;

		case TF_TEAM_GREEN:
			if ( !(contentsMask & CONTENTS_GREENTEAM ) )
				return false;
			break;

		case TF_TEAM_YELLOW:
			if ( !(contentsMask & CONTENTS_YELLOWTEAM ) )
				return false;
			break;
		}
	}
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
		#ifdef GAME_DLL
		RevivePlayer();
		#endif
		SetContextThink( NULL, 0, "ReviveThink" );
	}
}