//=============================================================================//
//	BY ALISTAIR 'peachfuzz' LESZKIEWICZ
//	peachfux@gmail.com
//=============================================================================//

#include "cbase.h"
#include "vcollide_parse.h"
#include "vehicle_base.h"
#include "npc_vehicledriver.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "saverestore_utlvector.h"
#include "KeyValues.h"
#include "studio.h"
#include "bone_setup.h"
#include "collisionutils.h"
#include "animation.h"
#include "env_player_surface_trigger.h"
#include "vehicle_passengerserver.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar g_debug_vehicleexit;


BEGIN_DATADESC( CPassengerServerVehicle )

	DEFINE_AUTO_ARRAY( m_savedPassengerViewOffset, FIELD_POSITION_VECTOR ),

END_DATADESC()


//========================================================================================================================================
// MULTIPASSENGER VEHICLE SERVER VEHICLE
//========================================================================================================================================
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------



IPassengerVehicle* CPassengerServerVehicle::GetPassengerVehicle( void )
{
	return (IPassengerVehicle *)m_pDrivableVehicle;
}

//-----------------------------------------------------------------------------
// Purpose: Modify the player view/camera while in a vehicle
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::GetVehicleViewPosition( int nRole, Vector *pAbsOrigin, QAngle *pAbsAngles )
{
	if( nRole == VEHICLE_ROLE_DRIVER )
		return BaseClass::GetVehicleViewPosition( nRole, pAbsOrigin, pAbsAngles );
	

	CBasePlayer *pPlayer = GetPassenger( nRole );
	Assert( pPlayer );

	*pAbsAngles = pPlayer->EyeAngles(); // yuck. this is an in/out parameter.

	char attachment[256];
	Q_snprintf( attachment, 256, "vehicle_passenger%d_eyes", nRole );
	
	GetFourWheelVehicle()->GetPhysics()->GetVehicleViewPosition( attachment, 1.0f, pAbsOrigin, pAbsAngles );

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBasePlayer	*CPassengerServerVehicle::GetPassenger( int iRole ) 
{ 
	//return BaseClass::GetPassenger( iRole );

	if( iRole == VEHICLE_ROLE_DRIVER )
	{
		return ToBasePlayer( GetPassengerVehicle()->GetDriver() );
	}
	else
	{
		return ToBasePlayer( GetPassengerVehicle()->GetPassenger( iRole ) );
	}
	
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPassengerServerVehicle::GetPassengerRole( CBasePlayer *pPassenger )
{
	//return BaseClass::GetPassengerRole( pPassenger );
	/*if (pPassenger == GetDrivableVehicle()->GetDriver())
		return VEHICLE_DRIVER;
	return -1;*/

	for (int i = VEHICLE_ROLE_DRIVER; i<LAST_SHARED_VEHICLE_ROLE; i++)
	{	
		CBasePlayer* pPlayer = ToBasePlayer( GetPassengerVehicle()->GetPassenger(i));

		if (pPassenger == pPlayer )
		{
			return i;
		}
	}
	
	Assert( 0 );
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move )
{
	return BaseClass::SetupMove( player, ucmd, pHelper, move );
	GetDrivableVehicle()->SetupMove( player, ucmd, pHelper, move );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData )
{
	return BaseClass::ProcessMovement( pPlayer, pMoveData );
	GetDrivableVehicle()->ProcessMovement( pPlayer, pMoveData );

	trace_t	tr;
	UTIL_TraceLine( pPlayer->GetAbsOrigin(), pPlayer->GetAbsOrigin() - Vector( 0, 0, 256 ), MASK_PLAYERSOLID, GetVehicleEnt(), COLLISION_GROUP_NONE, &tr );

	// If our gamematerial has changed, tell any player surface triggers that are watching
	IPhysicsSurfaceProps *physprops = MoveHelper()->GetSurfaceProps();
	surfacedata_t *pSurfaceProp = physprops->GetSurfaceData( tr.surface.surfaceProps );
	char cCurrGameMaterial = pSurfaceProp->game.material;

	// Changed?
	if ( m_chPreviousTextureType != cCurrGameMaterial )
	{
		CEnvPlayerSurfaceTrigger::SetPlayerSurface( pPlayer, cCurrGameMaterial );
	}

	m_chPreviousTextureType = cCurrGameMaterial;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::FinishMove( CBasePlayer *player, CUserCmd *ucmd, CMoveData *move )
{
	//return BaseClass::FinishMove( player, ucmd, move );
	GetDrivableVehicle()->FinishMove( player, ucmd, move );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::ItemPostFrame( CBasePlayer *player )
{
	//return BaseClass::ItemPostFrame( player );

	//Assert( player == GetDriver() );

	GetDrivableVehicle()->ItemPostFrame( player );

	if ( player->m_afButtonPressed & IN_USE )
	{
		if ( GetDrivableVehicle()->CanExitVehicle(player) )
		{
			if ( !HandlePassengerExit( player ) && ( player != NULL ) )
			{
				player->PlayUseDenySound();
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Get and set the current driver. Use PassengerRole_t enum in shareddefs.h for adding passengers
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::SetPassenger( int nRole, CBasePlayer *pPassenger )
{
	//return BaseClass::SetPassenger( nRole, pPassenger );
	// Baseclass only handles vehicles with a single passenger
	if( nRole == VEHICLE_ROLE_DRIVER )
		return BaseClass::SetPassenger( nRole, pPassenger );

	// Getting in? or out?
	if ( pPassenger )
	{
		m_savedPassengerViewOffset[nRole] = pPassenger->GetViewOffset();
		pPassenger->SetViewOffset( vec3_origin );
		pPassenger->ShowCrosshair( false );

		GetPassengerVehicle()->PassengerEnterVehicle( pPassenger, nRole );

	}
	else
	{
		// Restore the exiting player's view offset
		CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(GetPassenger(nRole));
		if ( pPlayer )
		{
			pPlayer->SetViewOffset( m_savedPassengerViewOffset[nRole] );
			pPlayer->ShowCrosshair( true );
		}

		GetDrivableVehicle()->ExitVehicle( nRole );
		GetDrivableVehicle()->SetVehicleEntryAnim( false );
		UTIL_Remove( m_hExitBlocker );

	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::HandlePassengerEntry( CBasePlayer *pPlayer, bool bAllowEntryOutsideZone )
{
	//return BaseClass::HandlePassengerEntry( pPlayer, bAllowEntryOutsideZone );

	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( !pAnimating )
		return;

	// Find out which hitbox the player's eyepoint is within
	int iEntryAnim = GetEntryAnimForPoint( pPlayer->EyePosition() );
	
	int role = GetEntryRoleForPoint( pPlayer->EyePosition() );

	//not in any hitbox areas
	if( role == -1 )
		return;

	m_CurrentEntryExitRole = role;

	// Are we in an entrypoint zone? 
	if ( iEntryAnim == ACTIVITY_NOT_AVAILABLE )
	{
		// Normal get in refuses to allow entry
		/*if ( !bAllowEntryOutsideZone )
			return;*/

		// We failed to find a valid entry anim, but we've got to get back in because the player's
		// got stuck exiting the vehicle. For now, just use the first get in anim
		// UNDONE: We need a better solution for this.
		iEntryAnim = pAnimating->LookupSequence( m_EntryAnimations[0].szAnimName );
	}

	// Check to see if this vehicle can be controlled or if it's locked
	if ( GetPassengerVehicle()->CanEnterVehicle(pPlayer) )
	{
		/*int role = GetPassengerVehicle()->GetNumPassengers();
		m_CurrentEntryExitRole = role;*/

		pPlayer->GetInVehicle( this, m_CurrentEntryExitRole);

		// Setup the "enter" vehicle sequence and skip the animation if it isn't present.
		pAnimating->SetCycle( 0 );
		pAnimating->m_flAnimTime = gpGlobals->curtime;
		pAnimating->ResetSequence( iEntryAnim );
		pAnimating->ResetClientsideFrame();
		GetDrivableVehicle()->SetVehicleEntryAnim( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CPassengerServerVehicle::HandlePassengerExit( CBasePlayer *pPlayer )
{

	//return BaseClass::HandlePassengerExit( pPlayer );
	// Clear hud hints
	UTIL_HudHintText( pPlayer, "" );

	vbs_sound_update_t params;
	//InitSoundParams(params);
	params.bExitVehicle = true;
	//SoundState_Update( params );

	// Find the right exit anim to use based on available exit points.
	Vector vecExitPoint;
	bool bAllPointsBlocked;
	int iSequence = GetExitAnimToUse( vecExitPoint, bAllPointsBlocked );

	// If all exit points were blocked and this vehicle doesn't allow exiting in
	// these cases, bail.
	Vector vecNewPos = pPlayer->GetAbsOrigin();
	QAngle angNewAngles = pPlayer->GetAbsAngles();

	m_CurrentEntryExitRole = GetPassengerRole( pPlayer );
	if ( ( bAllPointsBlocked ) || ( iSequence == ACTIVITY_NOT_AVAILABLE ) )
	{
		// Animation-driven exit points are all blocked, or we have none. Fall back to the more simple static exit points.
		if ( !GetPassengerExitPoint( m_CurrentEntryExitRole, &vecNewPos, &angNewAngles ) && !GetDrivableVehicle()->AllowBlockedExit( pPlayer, m_CurrentEntryExitRole ) )
		{
			return false;
		}
	}

	// Now we either have an exit sequence to play, a valid static exit position, or we don't care
	// whether we're blocked or not. We're getting out, one way or another.
	GetDrivableVehicle()->PreExitVehicle( pPlayer, m_CurrentEntryExitRole );

	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
		if ( pAnimating )
		{
			pAnimating->SetCycle( 0 );
			pAnimating->m_flAnimTime = gpGlobals->curtime;
			pAnimating->ResetSequence( iSequence );
			pAnimating->ResetClientsideFrame();
			GetDrivableVehicle()->SetVehicleExitAnim( true, vecExitPoint );

			// Re-deploy our weapon
			if ( pPlayer && pPlayer->IsAlive() )
			{
				if ( pPlayer->GetActiveWeapon() )
				{
					pPlayer->GetActiveWeapon()->Deploy();
					pPlayer->ShowCrosshair( true );
				}
			}

			// To prevent anything moving into the volume the player's going to occupy at the end of the exit
			// NOTE: Set the player as the blocker's owner so the player is allowed to intersect it
			Vector vecExitFeetPoint = vecExitPoint - VEC_VIEW;
			m_hExitBlocker = CEntityBlocker::Create( vecExitFeetPoint, VEC_HULL_MIN, VEC_HULL_MAX, pPlayer, true );
			return true;
		}
	}

	// Couldn't find an animation, so exit immediately
	pPlayer->LeaveVehicle( vecNewPos, angNewAngles );
	return true;
}

	
//-----------------------------------------------------------------------------
// Purpose: Get a position in *world space* inside the vehicle for the player to start at
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::GetPassengerStartPoint( int nRole, Vector *pPoint, QAngle *pAngles )
{
	//return BaseClass::GetPassengerStartPoint( nRole, pPoint, pAngles );

	//Assert( nRole == VEHICLE_DRIVER ); 

	// NOTE: We don't set the angles, which causes them to remain the same
	// as they were before entering the vehicle

	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( pAnimating )
	{
		char pAttachmentName[32];
		Q_snprintf( pAttachmentName, sizeof( pAttachmentName ), "vehicle_feet_passenger%d", nRole );
		int nFeetAttachmentIndex = pAnimating->LookupAttachment(pAttachmentName);
		if ( nFeetAttachmentIndex > 0 )
		{
			QAngle vecAngles;
			pAnimating->GetAttachment( nFeetAttachmentIndex, *pPoint, vecAngles );
			return;
		}
	}

	// Couldn't find the attachment point, so just use the origin
	*pPoint = m_pVehicle->GetAbsOrigin();
}


//-----------------------------------------------------------------------------
// Purpose: Where does this passenger exit the vehicle?
//-----------------------------------------------------------------------------
bool CPassengerServerVehicle::GetPassengerExitPoint( int nRole, Vector *pExitPoint, QAngle *pAngles )
{ 
	if( nRole == VEHICLE_ROLE_DRIVER )
		return BaseClass::GetPassengerExitPoint( nRole, pExitPoint, pAngles );

	//Assert( nRole == VEHICLE_DRIVER ); 

	// First, see if we've got an attachment point
	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( pAnimating )
	{
		Vector vehicleExitOrigin;
		QAngle vehicleExitAngles;

		char attachment[256];
		Q_snprintf( attachment, 256, "vehicle_passenger%d_exit", nRole );

		if ( pAnimating->GetAttachment( attachment, vehicleExitOrigin, vehicleExitAngles ) )
		{
			// Make sure it's clear
			trace_t tr;
			UTIL_TraceHull( vehicleExitOrigin + Vector(0, 0, 12), vehicleExitOrigin, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, m_pVehicle, COLLISION_GROUP_NONE, &tr );
			if ( !tr.startsolid )
			{
				*pAngles = vehicleExitAngles;
				*pExitPoint = tr.endpos;
				return true;
			}
		}
	}

	// left side 
	if( CheckExitPoint( 90, 90, pExitPoint ) )	// angle from car, distance from origin, actual exit point
		return true;

	// right side
	if( CheckExitPoint( -90, 90, pExitPoint ) )
		return true;

	// front
	if( CheckExitPoint( 0, 100, pExitPoint ) )
		return true;

	// back
	if( CheckExitPoint( 180, 170, pExitPoint ) )
		return true;

	// All else failed, try popping them out the top.
	Vector vecWorldMins, vecWorldMaxs;
	m_pVehicle->CollisionProp()->WorldSpaceAABB( &vecWorldMins, &vecWorldMaxs );
	pExitPoint->x = (vecWorldMins.x + vecWorldMaxs.x) * 0.5f;
	pExitPoint->y = (vecWorldMins.y + vecWorldMaxs.y) * 0.5f;
	pExitPoint->z = vecWorldMaxs.z + 50.0f;

	// Make sure it's clear
	trace_t tr;
	UTIL_TraceHull( m_pVehicle->CollisionProp()->WorldSpaceCenter(), *pExitPoint, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, m_pVehicle, COLLISION_GROUP_NONE, &tr );
	if ( !tr.startsolid )
	{
		return true;
	}

	// No clear exit point available!
	return false;
}



//---------------------------------------------------------------------------------
// Check Exit Point for leaving vehicle.
//
// Input: yaw/roll from vehicle angle to check for exit
//		  distance from origin to drop player (allows for different shaped vehicles
// Output: returns true if valid location, pEndPoint
//         updated with actual exit point
//---------------------------------------------------------------------------------
bool CPassengerServerVehicle::CheckExitPoint( float yaw, int distance, Vector *pEndPoint )
{
	//return BaseClass::CheckExitPoint( yaw, distance, pEndPoint );
	QAngle vehicleAngles = m_pVehicle->GetLocalAngles();
  	Vector vecStart = m_pVehicle->GetAbsOrigin();
  	Vector vecDir;
   
  	vecStart.z += 12;		// always 12" from ground
  	vehicleAngles[YAW] += yaw;	
  	AngleVectors( vehicleAngles, NULL, &vecDir, NULL );
	// Vehicles are oriented along the Y axis
	vecDir *= -1;
  	*pEndPoint = vecStart + vecDir * distance;
  
  	trace_t tr;
  	UTIL_TraceHull( vecStart, *pEndPoint, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, m_pVehicle, COLLISION_GROUP_NONE, &tr );

	if ( tr.fraction < 1.0 )
		return false;
  
  	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPassengerServerVehicle::GetEntryAnimForPoint( const Vector &vecEyePoint )
{
	//return BaseClass::GetEntryAnimForPoint( vecEyePoint );
	// Parse the vehicle animations the first time they get in the vehicle
	if ( !m_bParsedAnimations )
	{
		// Load the entry/exit animations from the vehicle
		ParseEntryExitAnims();
		m_bParsedAnimations = true;
	}

	// No entry anims? Vehicles with no entry anims are always enterable.
	if ( !m_EntryAnimations.Count() )
		return 0;

	// Figure out which entrypoint hitbox the player is in
	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( !pAnimating )
		return 0;

	CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();
	if (!pStudioHdr)
		return 0;
	int iHitboxSet = FindHitboxSetByName( pStudioHdr, "entryboxes" );
	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( iHitboxSet );
	if ( !set || !set->numhitboxes )
		return 0;

	// Loop through the hitboxes and find out which one we're in
	for ( int i = 0; i < set->numhitboxes; i++ )
	{
		mstudiobbox_t *pbox = set->pHitbox( i );

		Vector vecPosition;
		QAngle vecAngles;
		pAnimating->GetBonePosition( pbox->bone, vecPosition, vecAngles );

		// Build a rotation matrix from orientation
		matrix3x4_t fRotateMatrix;
		AngleMatrix( vecAngles, vecPosition, fRotateMatrix);

		Vector localEyePoint;
		VectorITransform( vecEyePoint, fRotateMatrix, localEyePoint );
		if ( IsPointInBox( localEyePoint, pbox->bbmin, pbox->bbmax ) )
		{
			// Find the entry animation for this hitbox
			int iCount = m_EntryAnimations.Count();
			for ( int entry = 0; entry < iCount; entry++ )
			{
				if ( m_EntryAnimations[entry].iHitboxGroup == pbox->group )
				{
					// Get the sequence for the animation
					return pAnimating->LookupSequence( m_EntryAnimations[entry].szAnimName );
				}
			}
		}
	}

	// Fail
	return ACTIVITY_NOT_AVAILABLE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CPassengerServerVehicle::GetEntryRoleForPoint( const Vector &vecEyePoint )
{

	// Figure out which entrypoint hitbox the player is in
	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( !pAnimating )
		return 0;

	CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();
	if (!pStudioHdr)
		return 0;
	int iHitboxSet = FindHitboxSetByName( pStudioHdr, "entryboxes" );
	mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( iHitboxSet );
	if ( !set || !set->numhitboxes )
		return 0;

	// Loop through the hitboxes and find out which one we're in
	for ( int i = 0; i < set->numhitboxes; i++ )
	{
		mstudiobbox_t *pbox = set->pHitbox( i );

		Vector vecPosition;
		QAngle vecAngles;
		pAnimating->GetBonePosition( pbox->bone, vecPosition, vecAngles );

		// Build a rotation matrix from orientation
		matrix3x4_t fRotateMatrix;
		AngleMatrix( vecAngles, vecPosition, fRotateMatrix);

		Vector localEyePoint;
		VectorITransform( vecEyePoint, fRotateMatrix, localEyePoint );
		if ( IsPointInBox( localEyePoint, pbox->bbmin, pbox->bbmax ) )
		{
			return i;
		}
	}

	// Fail
	//Assert( 0 );
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Find an exit animation that'll get the player to a valid position
// Input  : vecEyeExitEndpoint - Returns with the final eye position after exiting.
//			bAllPointsBlocked - Returns whether all exit points were found to be blocked.
// Output : 
//-----------------------------------------------------------------------------
int CPassengerServerVehicle::GetExitAnimToUse( int nRole, Vector &vecEyeExitEndpoint, bool &bAllPointsBlocked )
{
	//return BaseClass::GetExitAnimToUse( vecEyeExitEndpoint, bAllPointsBlocked );

	bAllPointsBlocked = false;
	
	// Parse the vehicle animations the first time they get in the vehicle
	if ( !m_bParsedAnimations )
	{
		// Load the entry/exit animations from the vehicle
		ParseEntryExitAnims();
		m_bParsedAnimations = true;
	}

	// No exit anims? 
	if ( !m_ExitAnimations.Count() )
		return ACTIVITY_NOT_AVAILABLE;

	// Figure out which entrypoint hitbox the player is in
	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( !pAnimating )
		return ACTIVITY_NOT_AVAILABLE;

	CStudioHdr *pStudioHdr = pAnimating->GetModelPtr();
	if (!pStudioHdr)
		return ACTIVITY_NOT_AVAILABLE;

	bool bUpright = IsVehicleUpright();

	// Loop through the exit animations and find one that ends in a clear position
	// Also attempt to choose the animation which brings you closest to your view direction.
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(GetPassenger(nRole));
	if (!pPlayer)
		return ACTIVITY_NOT_AVAILABLE;


	int nBestExitAnim = -1;
	bool bBestExitIsEscapePoint = true;
	Vector vecViewDirection, vecViewOrigin, vecBestExitPoint( 0, 0, 0 );
	vecViewOrigin = pPlayer->EyePosition();
	pPlayer->EyeVectors( &vecViewDirection );
	vecViewDirection.z = 0.0f;
	VectorNormalize( vecViewDirection );

	float flMaxCosAngleDelta = -2.0f;

	int iCount = m_ExitAnimations.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		if ( m_ExitAnimations[i].bUpright != bUpright )
			continue;

		// Don't use an escape point if we found a non-escape point already
		if ( !bBestExitIsEscapePoint && m_ExitAnimations[i].bEscapeExit )
			continue;

		trace_t tr;
		Vector vehicleExitOrigin;
		QAngle vehicleExitAngles;

		// Ensure the endpoint is clear by dropping a point down from above
		//pAnimating->GetAttachment( m_ExitAnimations[i].iAttachment, vehicleExitOrigin, vehicleExitAngles ); //FixMe

		// Don't bother checking points which are farther from our view direction.
		Vector vecDelta;
		VectorSubtract( vehicleExitOrigin, vecViewOrigin, vecDelta );
		vecDelta.z = 0.0f;
		VectorNormalize( vecDelta );
		float flCosAngleDelta = DotProduct( vecDelta, vecViewDirection );

		// But always check non-escape exits if our current best exit is an escape exit.
		if ( !bBestExitIsEscapePoint || m_ExitAnimations[i].bEscapeExit )
		{
			if ( flCosAngleDelta < flMaxCosAngleDelta )
				continue;
		}

		// The attachment points are where the driver's eyes will end up, so we subtract the view offset
		// to get the actual exit position.
		vehicleExitOrigin -= VEC_VIEW;

		Vector vecMove(0,0,64);
		Vector vecStart = vehicleExitOrigin + vecMove;
		Vector vecEnd = vehicleExitOrigin - vecMove;

		// First try a zero-length ray to check for being partially stuck in displacement surface.
	  	UTIL_TraceHull( vecStart, vecStart, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, NULL, COLLISION_GROUP_NONE, &tr );
		if ( !tr.startsolid )
		{
			// Now trace down to find the ground (or water), where we will put the player.
	  		UTIL_TraceHull( vecStart, vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, MASK_WATER | MASK_PLAYERSOLID, NULL, COLLISION_GROUP_NONE, &tr );
		}

		if ( tr.startsolid )
		{
			// Started in solid, try again starting at the exit point itself (might be under an overhang).
			vecStart = vehicleExitOrigin;

			// First try a zero-length ray to check for being partially stuck in displacement surface.
		  	UTIL_TraceHull( vecStart, vecStart, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID, NULL, COLLISION_GROUP_NONE, &tr );
			if ( !tr.startsolid )
			{
				// Now trace down to find the ground (or water), where we will put the player.
			  	UTIL_TraceHull( vecStart, vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, MASK_WATER | MASK_PLAYERSOLID, NULL, COLLISION_GROUP_NONE, &tr );
			}

			if ( g_debug_vehicleexit.GetBool() )
			{
				NDebugOverlay::Box( vecStart, VEC_HULL_MIN, VEC_HULL_MAX, 0,255,0, 8, 10 );
				NDebugOverlay::Box( vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, 255,255,255, 8, 10 );
			}
		}
		else if ( g_debug_vehicleexit.GetBool() )
		{
			NDebugOverlay::Box( vecStart, VEC_HULL_MIN, VEC_HULL_MAX, 0,255,0, 8, 10 );
			NDebugOverlay::Box( vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, 255,255,255, 8, 10 );
		}

		// Disallow exits at blocked exit points or where we can't find the ground below the exit point,
		// so that we don't exit off of a cliff (although some vehicles allow this).
		if ( tr.startsolid || ( ( tr.fraction == 1.0 ) && !GetDrivableVehicle()->AllowMidairExit( pPlayer, nRole ) ) )
		{
			if ( g_debug_vehicleexit.GetBool() )
			{
				NDebugOverlay::Box( tr.endpos, VEC_HULL_MIN, VEC_HULL_MAX, 255,0,0, 64, 10 );
			}
			continue;
		}

		// Calculate the exit endpoint & viewpoint
		Vector vecExitEndPoint;
		VectorLerp( vecStart, vecEnd, tr.fraction, vecExitEndPoint );

		// Make sure we can trace to the center of the exit point
		UTIL_TraceLine( vecViewOrigin, vecExitEndPoint, MASK_PLAYERSOLID, pAnimating, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction != 1.0 )
		{
			if ( g_debug_vehicleexit.GetBool() )
			{
				NDebugOverlay::Line( vecViewOrigin, vecExitEndPoint, 255,0,0, true, 10 );
			}
			continue;
		}

		bBestExitIsEscapePoint = m_ExitAnimations[i].bEscapeExit;
		vecBestExitPoint = vecExitEndPoint;
		nBestExitAnim = i;
		flMaxCosAngleDelta = flCosAngleDelta;
	}

	if ( nBestExitAnim >= 0 )
	{
		m_vecCurrentExitEndPoint = vecBestExitPoint;

		if ( g_debug_vehicleexit.GetBool() )
		{
			NDebugOverlay::Cross3D( m_vecCurrentExitEndPoint, 16, 0, 255, 0, true, 10 );
			NDebugOverlay::Box( m_vecCurrentExitEndPoint, VEC_HULL_MIN, VEC_HULL_MAX, 255,255,255, 8, 10 );
		}

		vecEyeExitEndpoint = vecBestExitPoint + VEC_VIEW;
		m_iCurrentExitAnim = nBestExitAnim;
		return pAnimating->LookupSequence( m_ExitAnimations[m_iCurrentExitAnim].szAnimName );
	}

	// Fail, all exit points were blocked.
	bAllPointsBlocked = true;
	return ACTIVITY_NOT_AVAILABLE;	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPassengerServerVehicle::HandleEntryExitFinish( int nRole, bool bExitAnimOn, bool bResetAnim )
{
	//return BaseClass::HandleEntryExitFinish( bExitAnimOn, bResetAnim );
	// Parse the vehicle animations. This is needed because they may have 
	// saved, and loaded during exit anim, which would clear the exit anim.
	if ( !m_bParsedAnimations )
	{
		// Load the entry/exit animations from the vehicle
		ParseEntryExitAnims();
		m_bParsedAnimations = true;
	}


	// Figure out which entrypoint hitbox the player is in
	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	if ( !pAnimating )
		return;
		
	// Did the entry anim just finish?
	if ( bExitAnimOn )
	{

		// The exit animation just finished
		CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>( GetPassenger( nRole ) );
		if ( pPlayer )
		{
			Vector vecEyes;
			QAngle vecEyeAng;
			if ( m_iCurrentExitAnim >= 0 && m_iCurrentExitAnim < m_ExitAnimations.Count() )
			{
				pAnimating->GetAttachment( m_ExitAnimations[m_iCurrentExitAnim].szAnimName, vecEyes, vecEyeAng );

				// Use the endpoint we figured out when we exited
				vecEyes = m_vecCurrentExitEndPoint;
			}
			else
			{
				pAnimating->GetAttachment( "vehicle_driver_eyes", vecEyes, vecEyeAng );
			}

			if ( g_debug_vehicleexit.GetBool() )
			{
				NDebugOverlay::Box( vecEyes, -Vector(2,2,2), Vector(2,2,2), 255,0,0, 64, 10.0 );
			}

			pPlayer->LeaveVehicle( vecEyes, vecEyeAng );
		}
	}

	// Only reset the animation if we're told to
	if ( bResetAnim )
	{
		// Start the vehicle idling again
		int iSequence = pAnimating->SelectWeightedSequence( ACT_IDLE );
		if ( iSequence > ACTIVITY_NOT_AVAILABLE )
		{
			pAnimating->SetCycle( 0 );
			pAnimating->m_flAnimTime = gpGlobals->curtime;
			pAnimating->ResetSequence( iSequence );
			pAnimating->ResetClientsideFrame();
		}
	}

	GetDrivableVehicle()->SetVehicleEntryAnim( false );
	GetDrivableVehicle()->SetVehicleExitAnim( false, vec3_origin );

}