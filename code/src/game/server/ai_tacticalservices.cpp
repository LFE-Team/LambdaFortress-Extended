//========= Copyright Valve Corporation, All rights reserved. ============//
//
//=============================================================================

#include "cbase.h"

#include "bitstring.h"

#include "ai_tacticalservices.h"
#include "ai_basenpc.h"
#include "ai_node.h"
#include "ai_network.h"
#include "ai_link.h"
#include "ai_moveprobe.h"
#include "ai_pathfinder.h"
#include "ai_navigator.h"
#include "ai_networkmanager.h"
#include "ai_hint.h"

#ifdef USE_NAV_MESH
#include "nav_mesh.h"
#include "nav_pathfind.h"
#include "nav_area.h"
#endif // USE_NAV_MESH

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar ai_find_lateral_cover( "ai_find_lateral_cover", "1" );
ConVar ai_find_lateral_los( "ai_find_lateral_los", "1" );

#ifdef _DEBUG
ConVar ai_debug_cover( "ai_debug_cover", "0" );
int g_AIDebugFindCoverNode = -1;
#define DebugFindCover( node, from, to, r, g, b ) \
	if ( !ai_debug_cover.GetBool() || \
		 (g_AIDebugFindCoverNode != -1 && g_AIDebugFindCoverNode != node) || \
		 !GetOuter()->m_bSelected ) \
		; \
	else \
		NDebugOverlay::Line( from, to, r, g, b, false, 1 )

#define DebugFindCover2( node, from, to, r, g, b ) \
	if ( ai_debug_cover.GetInt() < 2 || \
		 (g_AIDebugFindCoverNode != -1 && g_AIDebugFindCoverNode != node) || \
		 !GetOuter()->m_bSelected ) \
		; \
	else \
		NDebugOverlay::Line( from, to, r, g, b, false, 1 )

ConVar ai_debug_tactical_los( "ai_debug_tactical_los", "0" );
int g_AIDebugFindLosNode = -1;
#define ShouldDebugLos( node ) ( ai_debug_tactical_los.GetBool() && ( g_AIDebugFindLosNode == -1 || g_AIDebugFindLosNode == ( node ) ) && GetOuter()->m_bSelected )
#else
#define DebugFindCover( node, from, to, r, g, b ) ((void)0)
#define DebugFindCover2( node, from, to, r, g, b ) ((void)0)
#define ShouldDebugLos( node ) false
#endif

#ifdef USE_NAV_MESH
//--------------------------------------------------------------------------------------------------------------
/**
* If a player is at the given spot, return true
*/
bool IsSpotOccupied(CBaseEntity *me, const Vector &pos)
{
	trace_t tr;
	AI_TraceEntity(me, pos, pos + Vector(0, 0, 1), CONTENTS_MONSTER, &tr);
	CBaseEntity *player = tr.m_pEnt;
 	if (player != me)
	{
		if (player)
			return true;
	}
 	// is there is a hostage in this spot
	// BOTPORT: Implement hostage manager
	/*
	if (g_pHostages)
	{
	CHostage *hostage = g_pHostages->GetClosestHostage( *pos, &range );
	if (hostage && hostage != me && range < closeRange)
	return true;
	}
	*/
 	return false;
}
 //--------------------------------------------------------------------------------------------------------------
class CollectHidingSpotsFunctor
{
public:
	CollectHidingSpotsFunctor(CBaseEntity *me, const Vector &origin, float range, int flags, Place place = UNDEFINED_PLACE) : m_origin(origin)
	{
		m_me = me;
		m_count = 0;
		m_range = range;
		m_flags = (unsigned char)flags;
		m_place = place;
		m_totalWeight = 0;
	}
 	enum { MAX_SPOTS = 256 };
 	bool operator() (CNavArea *area)
	{
		// if a place is specified, only consider hiding spots from areas in that place
		if (m_place != UNDEFINED_PLACE && area->GetPlace() != m_place)
			return true;
 		// collect all the hiding spots in this area
		auto *list = area->GetHidingSpots();
 		FOR_EACH_VEC((*list), it)
		{
			const HidingSpot *spot = (*list)[it];
 			// if we've filled up, stop searching
			if (m_count == MAX_SPOTS)
			{
				return false;
			}
 			// make sure hiding spot is in range
			if (m_range > 0.0f)
			{
				if ((spot->GetPosition() - m_origin).IsLengthGreaterThan(m_range))
				{
					continue;
				}
			}
 			// if a Player is using this hiding spot, don't consider it
			if (IsSpotOccupied(m_me, spot->GetPosition()))
			{
				// player is in hiding spot
				/// @todo Check if player is moving or sitting still
				continue;
			}
 			if (spot->GetArea() && (spot->GetArea()->GetAttributes() & NAV_MESH_DONT_HIDE))
			{
				// the area has been marked as DONT_HIDE since the last analysis, so let's ignore it
				continue;
			}
 			// only collect hiding spots with matching flags
			if (m_flags & spot->GetFlags())
			{
				m_hidingSpot[m_count] = &spot->GetPosition();
				m_hidingSpotWeight[m_count] = m_totalWeight;
 				// if it's an 'avoid' area, give it a low weight
				if (spot->GetArea() && (spot->GetArea()->GetAttributes() & NAV_MESH_AVOID))
				{
					m_totalWeight += 1;
				}
				else
				{
					m_totalWeight += 2;
				}
 				++m_count;
			}
		}
 		return (m_count < MAX_SPOTS);
	}
 	/**
	* Remove the spot at index "i"
	*/
	void RemoveSpot(int i)
	{
		if (m_count == 0)
			return;
 		for (int j = i + 1; j<m_count; ++j)
			m_hidingSpot[j - 1] = m_hidingSpot[j];
 		--m_count;
	}
 	int GetRandomHidingSpot(void)
	{
		int weight = RandomInt(0, m_totalWeight - 1);
		for (int i = 0; i<m_count - 1; ++i)
		{
			// if the next spot's starting weight is over the target weight, this spot is the one
			if (m_hidingSpotWeight[i + 1] >= weight)
			{
				return i;
			}
		}
 		// if we didn't find any, it's the last one
		return m_count - 1;
	}
 	CBaseEntity *m_me;
	const Vector &m_origin;
	float m_range;
 	const Vector *m_hidingSpot[MAX_SPOTS];
	int m_hidingSpotWeight[MAX_SPOTS];
	int m_totalWeight;
	int m_count;
 	unsigned char m_flags;
 	Place m_place;
};
 /**
* Do a breadth-first search to find a nearby hiding spot and return it.
* Don't pick a hiding spot that a Player is currently occupying.
* @todo Clean up this mess
*/
const Vector *FindNearbyHidingSpot(CBaseEntity *me, const Vector &pos, float maxRange, bool isSniper, bool useNearest)
{
	CNavArea *startArea = TheNavMesh->GetNearestNavArea(pos);
	if (startArea == NULL)
		return NULL;
 	// collect set of nearby hiding spots
	if (isSniper)
	{
		CollectHidingSpotsFunctor collector(me, pos, maxRange, HidingSpot::IDEAL_SNIPER_SPOT);
		SearchSurroundingAreas(startArea, pos, collector, maxRange);
 		if (collector.m_count)
		{
			int which = collector.GetRandomHidingSpot();
			return collector.m_hidingSpot[which];
		}
		else
		{
			// no ideal sniping spots, look for "good" sniping spots
			CollectHidingSpotsFunctor collector(me, pos, maxRange, HidingSpot::GOOD_SNIPER_SPOT);
			SearchSurroundingAreas(startArea, pos, collector, maxRange);
 			if (collector.m_count)
			{
				int which = collector.GetRandomHidingSpot();
				return collector.m_hidingSpot[which];
			}
 			// no sniping spots at all.. fall through and pick a normal hiding spot
		}
	}
 	// collect hiding spots with decent "cover"
	CollectHidingSpotsFunctor collector(me, pos, maxRange, HidingSpot::IN_COVER);
	SearchSurroundingAreas(startArea, pos, collector, maxRange);
 	if (collector.m_count == 0)
	{
		// no hiding spots at all - if we're not a sniper, try to find a sniper spot to use instead
		if (!isSniper)
		{
			return FindNearbyHidingSpot(me, pos, maxRange, true, useNearest);
		}
 		return NULL;
	}
 	if (useNearest)
	{
		// return closest hiding spot
		const Vector *closest = NULL;
		float closeRangeSq = 9999999999.9f;
		for (int i = 0; i<collector.m_count; ++i)
		{
			float rangeSq = (*collector.m_hidingSpot[i] - pos).LengthSqr();
			if (rangeSq < closeRangeSq)
			{
				closeRangeSq = rangeSq;
				closest = collector.m_hidingSpot[i];
			}
		}
 		return closest;
	}
 	// select a hiding spot at random
	int which = collector.GetRandomHidingSpot();
	return collector.m_hidingSpot[which];
}
 //--------------------------------------------------------------------------------------------------------------
/**
* Select a random hiding spot among the nav areas that are tagged with the given place
*/
const Vector *FindRandomHidingSpot(CBaseEntity *me, Place place, bool isSniper)
{
	// collect set of nearby hiding spots
	if (isSniper)
	{
		CollectHidingSpotsFunctor collector(me, me->GetAbsOrigin(), -1.0f, HidingSpot::IDEAL_SNIPER_SPOT, place);
		TheNavMesh->ForAllAreas(collector);
 		if (collector.m_count)
		{
			int which = RandomInt(0, collector.m_count - 1);
			return collector.m_hidingSpot[which];
		}
		else
		{
			// no ideal sniping spots, look for "good" sniping spots
			CollectHidingSpotsFunctor collector(me, me->GetAbsOrigin(), -1.0f, HidingSpot::GOOD_SNIPER_SPOT, place);
			TheNavMesh->ForAllAreas(collector);
 			if (collector.m_count)
			{
				int which = RandomInt(0, collector.m_count - 1);
				return collector.m_hidingSpot[which];
			}
 			// no sniping spots at all.. fall through and pick a normal hiding spot
		}
	}
 	// collect hiding spots with decent "cover"
	CollectHidingSpotsFunctor collector(me, me->GetAbsOrigin(), -1.0f, HidingSpot::IN_COVER, place);
	TheNavMesh->ForAllAreas(collector);
 	if (collector.m_count == 0)
		return NULL;
 	// select a hiding spot at random
	int which = RandomInt(0, collector.m_count - 1);
	return collector.m_hidingSpot[which];
}
 //--------------------------------------------------------------------------------------------------------------------
/**
* Select a nearby retreat spot.
* Don't pick a hiding spot that a Player is currently occupying.
* If "avoidTeam" is nonzero, avoid getting close to members of that team.
*/
//const Vector *FindNearbyRetreatSpot(CBaseEntity *me, const Vector &start, float maxRange, int avoidTeam)
//{
//	CNavArea *startArea = TheNavMesh->GetNearestNavArea(start);
//	if (startArea == NULL)
//		return NULL;
//
//	// collect hiding spots with decent "cover"
//	CollectHidingSpotsFunctor collector(me, start, maxRange, HidingSpot::IN_COVER);
//	SearchSurroundingAreas(startArea, start, collector, maxRange);
//
//	if (collector.m_count == 0)
//		return NULL;
//
//	// find the closest unoccupied hiding spot that crosses the least lines of fire and has the best cover
//	for (int i = 0; i<collector.m_count; ++i)
//	{
//		// check if we would have to cross a line of fire to reach this hiding spot
//		if (IsCrossingLineOfFire(start, *collector.m_hidingSpot[i], me))
//		{
//			collector.RemoveSpot(i);
//
//			// back up a step, so iteration won't skip a spot
//			--i;
//
//			continue;
//		}
//
//		// check if there is someone on the avoidTeam near this hiding spot
//		if (avoidTeam)
//		{
//			float range;
//			if (UTIL_GetClosestPlayer(*collector.m_hidingSpot[i], avoidTeam, &range))
//			{
//				const float dangerRange = 150.0f;
//				if (range < dangerRange)
//				{
//					// there is an avoidable player too near this spot - remove it
//					collector.RemoveSpot(i);
//
//					// back up a step, so iteration won't skip a spot
//					--i;
//
//					continue;
//				}
//			}
//		}
//	}
//
//	if (collector.m_count <= 0)
//		return NULL;
//
//	// all remaining spots are ok - pick one at random
//	int which = RandomInt(0, collector.m_count - 1);
//	return collector.m_hidingSpot[which];
//}
#endif // USE_NAV_MESH

//-----------------------------------------------------------------------------

BEGIN_SIMPLE_DATADESC(CAI_TacticalServices)
	//						m_pNetwork	(not saved)
	//						m_pPathfinder	(not saved)
	DEFINE_FIELD( m_bAllowFindLateralLos, FIELD_BOOLEAN ),

END_DATADESC();

//-------------------------------------

void CAI_TacticalServices::Init( CAI_Network *pNetwork )
{
	Assert( pNetwork );
	m_pNetwork = pNetwork;
	m_pPathfinder = GetOuter()->GetPathfinder();
	Assert( m_pPathfinder );
}
	
//-------------------------------------

bool CAI_TacticalServices::FindLos(const Vector &threatPos, const Vector &threatEyePos, float minThreatDist, float maxThreatDist, float blockTime, FlankType_t eFlankType, const Vector &vecFlankRefPos, float flFlankParam, Vector *pResult)
{
	AI_PROFILE_SCOPE( CAI_TacticalServices_FindLos );

	MARK_TASK_EXPENSIVE();

	int node = FindLosNode( threatPos, threatEyePos, 
											 minThreatDist, maxThreatDist, 
											 blockTime, eFlankType, vecFlankRefPos, flFlankParam );
	
	if (node == NO_NODE)
		return false;

	*pResult = GetNodePos( node );
	return true;
}

//-------------------------------------

bool CAI_TacticalServices::FindLos(const Vector &threatPos, const Vector &threatEyePos, float minThreatDist, float maxThreatDist, float blockTime, Vector *pResult)
{
	return FindLos( threatPos, threatEyePos, minThreatDist, maxThreatDist, blockTime, FLANKTYPE_NONE, vec3_origin, 0, pResult );
}

//-------------------------------------

bool CAI_TacticalServices::FindBackAwayPos( const Vector &vecThreat, Vector *pResult )
{
	MARK_TASK_EXPENSIVE();

	Vector vMoveAway = GetAbsOrigin() - vecThreat;
	vMoveAway.NormalizeInPlace();

	if ( GetOuter()->GetNavigator()->FindVectorGoal( pResult, vMoveAway, 10*12, 10*12, true ) )
		return true;

	int node = FindBackAwayNode( vecThreat );
	
	if (node != NO_NODE)
	{
		*pResult = GetNodePos( node );
		return true;
	}

	if ( GetOuter()->GetNavigator()->FindVectorGoal( pResult, vMoveAway, GetHullWidth() * 4, GetHullWidth() * 2, true ) )
		return true;

	return false;
}

//-------------------------------------

bool CAI_TacticalServices::FindCoverPos( const Vector &vThreatPos, const Vector &vThreatEyePos, float flMinDist, float flMaxDist, Vector *pResult )
{
	return FindCoverPos( GetLocalOrigin(), vThreatPos, vThreatEyePos, flMinDist, flMaxDist, pResult );
}

//-------------------------------------

bool CAI_TacticalServices::FindCoverPos( const Vector &vNearPos, const Vector &vThreatPos, const Vector &vThreatEyePos, float flMinDist, float flMaxDist, Vector *pResult )
{
	AI_PROFILE_SCOPE( CAI_TacticalServices_FindCoverPos );

	MARK_TASK_EXPENSIVE();

	int node = FindCoverNode( vNearPos, vThreatPos, vThreatEyePos, flMinDist, flMaxDist );
	
	if (node == NO_NODE)
	{
#ifdef USE_NAV_MESH
		const Vector *pCover = FindNearbyHidingSpot(GetOuter(), vNearPos, flMaxDist, false, false);
		if (pCover == nullptr)
			return false;
 		*pResult = *pCover;
		return true;
#else
		return false;
#endif // USE_NAV_MESH
	}

	*pResult = GetNodePos( node );
	return true;
}

//-------------------------------------
// Checks lateral cover
//-------------------------------------
bool CAI_TacticalServices::TestLateralCover( const Vector &vecCheckStart, const Vector &vecCheckEnd, float flMinDist )
{
	trace_t	tr;

	if ( (vecCheckStart - vecCheckEnd).LengthSqr() > Square(flMinDist) )
	{
		if (GetOuter()->IsCoverPosition(vecCheckStart, vecCheckEnd + GetOuter()->GetViewOffset()))
		{
			if ( GetOuter()->IsValidCover ( vecCheckEnd, NULL ) )
			{
				AIMoveTrace_t moveTrace;
				GetOuter()->GetMoveProbe()->MoveLimit( NAV_GROUND, GetLocalOrigin(), vecCheckEnd, MASK_NPCSOLID, NULL, &moveTrace );
				if (moveTrace.fStatus == AIMR_OK)
				{
					DebugFindCover( NO_NODE, vecCheckEnd + GetOuter()->GetViewOffset(), vecCheckStart, 0, 255, 0 );
					return true;
				}
			}
		}
	}

	DebugFindCover( NO_NODE, vecCheckEnd + GetOuter()->GetViewOffset(), vecCheckStart, 255, 0, 0 );

	return false;
}


//-------------------------------------
// FindLateralCover - attempts to locate a spot in the world
// directly to the left or right of the caller that will
// conceal them from view of pSightEnt
//-------------------------------------

#define	COVER_CHECKS	5// how many checks are made
#define COVER_DELTA		48// distance between checks
bool CAI_TacticalServices::FindLateralCover( const Vector &vecThreat, float flMinDist, Vector *pResult )
{
	return FindLateralCover( vecThreat, flMinDist, COVER_CHECKS * COVER_DELTA, COVER_CHECKS, pResult );
}

bool CAI_TacticalServices::FindLateralCover( const Vector &vecThreat, float flMinDist, float distToCheck, int numChecksPerDir, Vector *pResult )
{
	return FindLateralCover( GetAbsOrigin(), vecThreat, flMinDist, distToCheck, numChecksPerDir, pResult );
}

bool CAI_TacticalServices::FindLateralCover( const Vector &vNearPos, const Vector &vecThreat, float flMinDist, float distToCheck, int numChecksPerDir, Vector *pResult )
{
	AI_PROFILE_SCOPE( CAI_TacticalServices_FindLateralCover );

	MARK_TASK_EXPENSIVE();

	Vector	vecLeftTest;
	Vector	vecRightTest;
	Vector	vecStepRight;
	Vector  vecCheckStart;
	int		i;

	if ( TestLateralCover( vecThreat, vNearPos, flMinDist ) )
	{
		*pResult = GetLocalOrigin();
		return true;
	}

	if( !ai_find_lateral_cover.GetBool() )
	{
		// Force the NPC to use the nodegraph to find cover. NOTE: We let the above code run
		// to detect the case where the NPC may already be standing in cover, but we don't 
		// make any additional lateral checks.
		return false;
	}

	Vector right =  vecThreat - vNearPos;
	float temp;

	right.z = 0;
	VectorNormalize( right );
	temp = right.x;
	right.x = -right.y;
	right.y = temp;

	vecStepRight = right * (distToCheck / (float)numChecksPerDir);
	vecStepRight.z = 0;

	vecLeftTest = vecRightTest = vNearPos;
 	vecCheckStart = vecThreat;

	for ( i = 0 ; i < numChecksPerDir ; i++ )
	{
		vecLeftTest = vecLeftTest - vecStepRight;
		vecRightTest = vecRightTest + vecStepRight;

		if (TestLateralCover( vecCheckStart, vecLeftTest, flMinDist ))
		{
			*pResult = vecLeftTest;
			return true;
		}

		if (TestLateralCover( vecCheckStart, vecRightTest, flMinDist ))
		{
			*pResult = vecRightTest;
			return true;
		}
	}

	return false;
}

//-------------------------------------
// Purpose: Find a nearby node that further away from the enemy than the
//			min range of my current weapon if there is one or just futher
//			away than my current location if I don't have a weapon.  
//			Used to back away for attacks
//-------------------------------------

int CAI_TacticalServices::FindBackAwayNode(const Vector &vecThreat )
{
	if ( !CAI_NetworkManager::NetworksLoaded() )
	{
		DevWarning( 2, "Graph not ready for FindBackAwayNode!\n" );
		return NO_NODE;
	}

	int iMyNode			= GetPathfinder()->NearestNodeToNPC();
	int iThreatNode		= GetPathfinder()->NearestNodeToPoint( vecThreat );

	if ( iMyNode == NO_NODE )
	{
		DevWarning( 2, "FindBackAwayNode() - %s has no nearest node!\n", GetEntClassname());
		return NO_NODE;
	}
	if ( iThreatNode == NO_NODE )
	{
		// DevWarning( 2, "FindBackAwayNode() - Threat has no nearest node!\n" );
		iThreatNode = iMyNode;
		// return false;
	}

	// A vector pointing to the threat.
	Vector vecToThreat;
	vecToThreat = vecThreat - GetLocalOrigin();

	// Get my current distance from the threat
	float flCurDist = VectorNormalize( vecToThreat );

	// Check my neighbors to find a node that's further away
	for (int link = 0; link < GetNetwork()->GetNode(iMyNode)->NumLinks(); link++) 
	{
		CAI_Link *nodeLink = GetNetwork()->GetNode(iMyNode)->GetLinkByIndex(link);

		if ( !m_pPathfinder->IsLinkUsable( nodeLink, iMyNode ) )
			continue;

		int destID = nodeLink->DestNodeID(iMyNode);

		float flTestDist = ( vecThreat - GetNetwork()->GetNode(destID)->GetPosition(GetHullType()) ).Length();

		if ( flTestDist > flCurDist )
		{
			// Make sure this node doesn't take me past the enemy's position.
			Vector vecToNode;
			vecToNode = GetNetwork()->GetNode(destID)->GetPosition(GetHullType()) - GetLocalOrigin();
			VectorNormalize( vecToNode );
		
			if( DotProduct( vecToNode, vecToThreat ) < 0.0 )
			{
				return destID;
			}
		}
	}
	return NO_NODE;
}

//-------------------------------------
// FindCover - tries to find a nearby node that will hide
// the caller from its enemy. 
//
// If supplied, search will return a node at least as far
// away as MinDist, but no farther than MaxDist. 
// if MaxDist isn't supplied, it defaults to a reasonable 
// value
//-------------------------------------

int CAI_TacticalServices::FindCoverNode(const Vector &vThreatPos, const Vector &vThreatEyePos, float flMinDist, float flMaxDist )
{
	return FindCoverNode(GetLocalOrigin(), vThreatPos, vThreatEyePos, flMinDist, flMaxDist );
}

//-------------------------------------

int CAI_TacticalServices::FindCoverNode(const Vector &vNearPos, const Vector &vThreatPos, const Vector &vThreatEyePos, float flMinDist, float flMaxDist )
{
	if ( !CAI_NetworkManager::NetworksLoaded() )
		return NO_NODE;

	AI_PROFILE_SCOPE( CAI_TacticalServices_FindCoverNode );

	MARK_TASK_EXPENSIVE();

	DebugFindCover( NO_NODE, GetOuter()->EyePosition(), vThreatEyePos, 0, 255, 255 );

	int iMyNode = GetPathfinder()->NearestNodeToPoint( vNearPos );

	if ( iMyNode == NO_NODE )
	{
		Vector pos = GetOuter()->GetAbsOrigin();
		DevWarning( 2, "FindCover() - %s has no nearest node! (Check near %f %f %f)\n", GetEntClassname(), pos.x, pos.y, pos.z);
		return NO_NODE;
	}

	if ( !flMaxDist )
	{
		// user didn't supply a MaxDist, so work up a crazy one.
		flMaxDist = 784;
	}

	if ( flMinDist > 0.5 * flMaxDist)
	{
		flMinDist = 0.5 * flMaxDist;
	}

	// ------------------------------------------------------------------------------------
	// We're going to search for a cover node by expanding to our current node's neighbors
	// and then their neighbors, until cover is found, or all nodes are beyond MaxDist
	// ------------------------------------------------------------------------------------
	AI_NearNode_t *pBuffer = (AI_NearNode_t *)stackalloc( sizeof(AI_NearNode_t) * GetNetwork()->NumNodes() );
	CNodeList list( pBuffer, GetNetwork()->NumNodes() );
	CVarBitVec wasVisited(GetNetwork()->NumNodes());	// Nodes visited

	// mark start as visited
	list.Insert( AI_NearNode_t(iMyNode, 0) ); 
	wasVisited.Set( iMyNode );
	float flMinDistSqr = flMinDist*flMinDist;
	float flMaxDistSqr = flMaxDist*flMaxDist;

	static int nSearchRandomizer = 0;		// tries to ensure the links are searched in a different order each time;

	// Search until the list is empty
	while( list.Count() )
	{
		// Get the node that is closest in the number of steps and remove from the list
		int nodeIndex = list.ElementAtHead().nodeIndex;
		list.RemoveAtHead();

		CAI_Node *pNode = GetNetwork()->GetNode(nodeIndex);
		Vector nodeOrigin = pNode->GetPosition(GetHullType());

		float dist = (vNearPos - nodeOrigin).LengthSqr();
		if (dist >= flMinDistSqr && dist < flMaxDistSqr)
		{
			Activity nCoverActivity = GetOuter()->GetCoverActivity( pNode->GetHint() );
			Vector vEyePos = nodeOrigin + GetOuter()->EyeOffset(nCoverActivity);

			if ( GetOuter()->IsValidCover( nodeOrigin, pNode->GetHint() ) )
			{
				// Check if this location will block the threat's line of sight to me
				if (GetOuter()->IsCoverPosition(vThreatEyePos, vEyePos))
				{
					// --------------------------------------------------------
					// Don't let anyone else use this node for a while
					// --------------------------------------------------------
					pNode->Lock( 1.0 );

					if ( pNode->GetHint() && ( pNode->GetHint()->HintType() == HINT_TACTICAL_COVER_MED || pNode->GetHint()->HintType() == HINT_TACTICAL_COVER_LOW ) )
					{
						if ( GetOuter()->GetHintNode() )
						{
							GetOuter()->GetHintNode()->Unlock(GetOuter()->GetHintDelay(GetOuter()->GetHintNode()->HintType()));
							GetOuter()->SetHintNode( NULL );
						}

						GetOuter()->SetHintNode( pNode->GetHint() );
					}

					// The next NPC who searches should use a slight different pattern
					nSearchRandomizer = nodeIndex;
					DebugFindCover( pNode->GetId(), vEyePos, vThreatEyePos, 0, 255, 0 );
					return nodeIndex;
				}
				else
				{
					DebugFindCover( pNode->GetId(), vEyePos, vThreatEyePos, 255, 0, 0 );
				}
			}
			else
			{
				DebugFindCover( pNode->GetId(), vEyePos, vThreatEyePos, 0, 0, 255 );
			}
		}

		// Add its children to the search list
		// Go through each link
		// UNDONE: Pass in a cost function to measure each link?
		for ( int link = 0; link < GetNetwork()->GetNode(nodeIndex)->NumLinks(); link++ ) 
		{
			int index = (link + nSearchRandomizer) % GetNetwork()->GetNode(nodeIndex)->NumLinks();
			CAI_Link *nodeLink = GetNetwork()->GetNode(nodeIndex)->GetLinkByIndex(index);

			if ( !m_pPathfinder->IsLinkUsable( nodeLink, iMyNode ) )
				continue;

			int newID = nodeLink->DestNodeID(nodeIndex);

			// If not already on the closed list, add to it and set its distance
			if (!wasVisited.IsBitSet(newID))
			{
				// Don't accept climb nodes or nodes that aren't ready to use yet
				if ( GetNetwork()->GetNode(newID)->GetType() != NODE_CLIMB && !GetNetwork()->GetNode(newID)->IsLocked() )
				{
					// UNDONE: Shouldn't we really accumulate the distance by path rather than
					// absolute distance.  After all, we are performing essentially an A* here.
					nodeOrigin = GetNetwork()->GetNode(newID)->GetPosition(GetHullType());
					dist = (vNearPos - nodeOrigin).LengthSqr();

					// use distance to threat as a heuristic to keep AIs from running toward
					// the threat in order to take cover from it.
					float threatDist = (vThreatPos - nodeOrigin).LengthSqr();

					// Now check this node is not too close towards the threat
					if ( dist < threatDist * 1.5 )
					{
						list.Insert( AI_NearNode_t(newID, dist) );
					}
				}
				// mark visited
				wasVisited.Set(newID);
			}
		}
	}

	// We failed.  Not cover node was found
	// Clear hint node used to set ducking
	GetOuter()->ClearHintNode();
	return NO_NODE;
}


//-------------------------------------
// Purpose:  Return node ID that has line of sight to target I want to shoot
//
// Input  :	pNPC			- npc that's looking for a place to shoot from
//			vThreatPos		- position of entity/location I'm trying to shoot
//			vThreatEyePos	- eye position of entity I'm trying to shoot. If 
//							  entity has no eye position, just give vThreatPos again
//			flMinThreatDist	- minimum distance that node must be from vThreatPos
//			flMaxThreadDist	- maximum distance that node can be from vThreadPos
//			vThreatFacing	- optional argument.  If given the returned node
//							  will also be behind the given facing direction (flanking)
//			flBlockTime		- how long to block this node from use
// Output :	int				- ID number of node that meets qualifications
//-------------------------------------

int CAI_TacticalServices::FindLosNode(const Vector &vThreatPos, const Vector &vThreatEyePos, float flMinThreatDist, float flMaxThreatDist, float flBlockTime, FlankType_t eFlankType, const Vector &vecFlankRefPos, float flFlankParam )
{
	if ( !CAI_NetworkManager::NetworksLoaded() )
		return NO_NODE;

	AI_PROFILE_SCOPE( CAI_TacticalServices_FindLosNode );

	MARK_TASK_EXPENSIVE();

	int iMyNode	= GetPathfinder()->NearestNodeToNPC();
	if ( iMyNode == NO_NODE )
	{
		Vector pos = GetOuter()->GetAbsOrigin();
		DevWarning( 2, "FindCover() - %s has no nearest node! (Check near %f %f %f)\n", GetEntClassname(), pos.x, pos.y, pos.z);
		return NO_NODE;
	}

	// ------------------------------------------------------------------------------------
	// We're going to search for a shoot node by expanding to our current node's neighbors
	// and then their neighbors, until a shooting position is found, or all nodes are beyond MaxDist
	// ------------------------------------------------------------------------------------
	AI_NearNode_t *pBuffer = (AI_NearNode_t *)stackalloc( sizeof(AI_NearNode_t) * GetNetwork()->NumNodes() );
	CNodeList list( pBuffer, GetNetwork()->NumNodes() );
	CVarBitVec wasVisited(GetNetwork()->NumNodes());	// Nodes visited

	// mark start as visited
	wasVisited.Set( iMyNode );
	list.Insert( AI_NearNode_t(iMyNode, 0) );

	static int nSearchRandomizer = 0;		// tries to ensure the links are searched in a different order each time;

	while ( list.Count() )
	{
		int nodeIndex = list.ElementAtHead().nodeIndex;
		// remove this item from the list
		list.RemoveAtHead();

		const Vector &nodeOrigin = GetNetwork()->GetNode(nodeIndex)->GetPosition(GetHullType());

		// HACKHACK: Can't we rework this loop and get rid of this?
		// skip the starting node, or we probably wouldn't have called this function.
		if ( nodeIndex != iMyNode )
		{
			bool skip = false;

			// See if the node satisfies the flanking criteria.
			switch ( eFlankType )
			{
				case FLANKTYPE_NONE:
					break;
					
				case FLANKTYPE_RADIUS:
				{
					Vector vecDist = nodeOrigin - vecFlankRefPos;
					if ( vecDist.Length() < flFlankParam )
					{
						skip = true;
					}
					
					break;
				}
				
				case FLANKTYPE_ARC:
				{
					Vector vecEnemyToRef = vecFlankRefPos - vThreatPos;
					VectorNormalize( vecEnemyToRef );

					Vector vecEnemyToNode = nodeOrigin - vThreatPos;
					VectorNormalize( vecEnemyToNode );
					
					float flDot = DotProduct( vecEnemyToRef, vecEnemyToNode );
					
					if ( RAD2DEG( acos( flDot ) ) < flFlankParam )
					{
						skip = true;
					}
					
					break;
				}
			}

			// Don't accept climb nodes, and assume my nearest node isn't valid because
			// we decided to make this check in the first place.  Keep moving
			if ( !skip && !GetNetwork()->GetNode(nodeIndex)->IsLocked() &&
				GetNetwork()->GetNode(nodeIndex)->GetType() != NODE_CLIMB )
			{
				// Now check its distance and only accept if in range
				float flThreatDist = ( nodeOrigin - vThreatPos ).Length();

				if ( flThreatDist < flMaxThreatDist &&
					 flThreatDist > flMinThreatDist )
				{
					CAI_Node *pNode = GetNetwork()->GetNode(nodeIndex);
					if ( GetOuter()->IsValidShootPosition( nodeOrigin, pNode, pNode->GetHint() ) )
					{
						if (GetOuter()->TestShootPosition(nodeOrigin,vThreatEyePos))
						{
							// Note when this node was used, so we don't try 
							// to use it again right away.
							GetNetwork()->GetNode(nodeIndex)->Lock( flBlockTime );

#if 0
							if ( GetOuter()->GetHintNode() )
							{
								GetOuter()->GetHintNode()->Unlock(GetOuter()->GetHintDelay(GetOuter()->GetHintNode()->HintType()));
								GetOuter()->SetHintNode( NULL );
							}

							// This used to not be set, why? (kenb)
							// @Note (toml 05-19-04): I think because stomping  the hint can lead to
							// unintended side effects. The hint node is primarily a high level
							// tool, and certain NPCs break if it gets slammed here. If we need
							// this, we should propagate it out and let the schedule selector
							// or task decide to set the hint node
							GetOuter()->SetHintNode( GetNetwork()->GetNode(nodeIndex)->GetHint() );
#endif
							if ( ShouldDebugLos( nodeIndex ) )
							{
								NDebugOverlay::Text( nodeOrigin, CFmtStr( "%d:los!", nodeIndex), false, 1 );
							}

							// The next NPC who searches should use a slight different pattern
							nSearchRandomizer = nodeIndex;
							return nodeIndex;
						}
						else
						{
							if ( ShouldDebugLos( nodeIndex ) )
							{
								NDebugOverlay::Text( nodeOrigin, CFmtStr( "%d:!shoot", nodeIndex), false, 1 );
							}
						}
					}
					else
					{
						if ( ShouldDebugLos( nodeIndex ) )
						{
							NDebugOverlay::Text( nodeOrigin, CFmtStr( "%d:!valid", nodeIndex), false, 1 );
						}
					}
				}
				else
				{
					if ( ShouldDebugLos( nodeIndex ) )
					{
						CFmtStr msg( "%d:%s", nodeIndex, ( flThreatDist < flMaxThreatDist ) ? "too close" : "too far" );
						NDebugOverlay::Text( nodeOrigin, msg, false, 1 );
					}
				}
			}
		}

		// Go through each link and add connected nodes to the list
		for (int link=0; link < GetNetwork()->GetNode(nodeIndex)->NumLinks();link++) 
		{
			int index = (link + nSearchRandomizer) % GetNetwork()->GetNode(nodeIndex)->NumLinks();
			CAI_Link *nodeLink = GetNetwork()->GetNode(nodeIndex)->GetLinkByIndex(index);

			if ( !m_pPathfinder->IsLinkUsable( nodeLink, iMyNode ) )
				continue;

			int newID = nodeLink->DestNodeID(nodeIndex);

			// If not already visited, add to the list
			if (!wasVisited.IsBitSet(newID))
			{
				float dist = (GetLocalOrigin() - GetNetwork()->GetNode(newID)->GetPosition(GetHullType())).LengthSqr();
				list.Insert( AI_NearNode_t(newID, dist) );
				wasVisited.Set( newID );
			}
		}
	}
	// We failed.  No range attack node node was found
	return NO_NODE;
}

//-------------------------------------
// Checks lateral LOS
//-------------------------------------
bool CAI_TacticalServices::TestLateralLos( const Vector &vecCheckStart, const Vector &vecCheckEnd )
{
	trace_t	tr;

	// it's faster to check the SightEnt's visibility to the potential spot than to check the local move, so we do that first.
	AI_TraceLOS(  vecCheckStart, vecCheckEnd + GetOuter()->GetViewOffset(), NULL, &tr );

	if (tr.fraction == 1.0)
	{
		if ( GetOuter()->IsValidShootPosition( vecCheckEnd, NULL, NULL ) )
			{
				if (GetOuter()->TestShootPosition(vecCheckEnd,vecCheckStart))
				{
					AIMoveTrace_t moveTrace;
					GetOuter()->GetMoveProbe()->MoveLimit( NAV_GROUND, GetLocalOrigin(), vecCheckEnd, MASK_NPCSOLID, NULL, &moveTrace );
					if (moveTrace.fStatus == AIMR_OK)
					{
						return true;
					}
				}
		}
	}

	return false;
}


//-------------------------------------

bool CAI_TacticalServices::FindLateralLos( const Vector &vecThreat, Vector *pResult )
{
	AI_PROFILE_SCOPE( CAI_TacticalServices_FindLateralLos );

	if( !m_bAllowFindLateralLos )
	{
		return false;
	}

	MARK_TASK_EXPENSIVE();

	Vector	vecLeftTest;
	Vector	vecRightTest;
	Vector	vecStepRight;
	Vector  vecCheckStart;
	bool	bLookingForEnemy = GetEnemy() && VectorsAreEqual(vecThreat, GetEnemy()->EyePosition(), 0.1f);
	int		i;

	if(  !bLookingForEnemy || GetOuter()->HasCondition(COND_SEE_ENEMY) || GetOuter()->HasCondition(COND_HAVE_ENEMY_LOS) || 
		 GetOuter()->GetTimeScheduleStarted() == gpGlobals->curtime ) // Conditions get nuked before tasks run, assume should try
	{
		// My current position might already be valid.
		if ( TestLateralLos(vecThreat, GetLocalOrigin()) )
		{
			*pResult = GetLocalOrigin();
			return true;
		}
	}

	if( !ai_find_lateral_los.GetBool() )
	{
		// Allows us to turn off lateral LOS at the console. Allow the above code to run 
		// just in case the NPC has line of sight to begin with.
		return false;
	}

	int iChecks = COVER_CHECKS;
	int iDelta = COVER_DELTA;

	// If we're limited in how far we're allowed to move laterally, don't bother checking past it
	int iMaxLateralDelta = GetOuter()->GetMaxTacticalLateralMovement();
	if ( iMaxLateralDelta != MAXTACLAT_IGNORE && iMaxLateralDelta < iDelta )
	{
		iChecks = 1;
		iDelta = iMaxLateralDelta;
	}

	Vector right;
	AngleVectors( GetLocalAngles(), NULL, &right, NULL );
	vecStepRight = right * iDelta;
	vecStepRight.z = 0;

	vecLeftTest = vecRightTest = GetLocalOrigin();
 	vecCheckStart = vecThreat;

	for ( i = 0 ; i < iChecks; i++ )
	{
		vecLeftTest = vecLeftTest - vecStepRight;
		vecRightTest = vecRightTest + vecStepRight;

		if (TestLateralLos( vecCheckStart, vecLeftTest ))
		{
			*pResult = vecLeftTest;
			return true;
		}

		if (TestLateralLos( vecCheckStart, vecRightTest ))
		{
			*pResult = vecRightTest;
			return true;
		}
	}

	return false;
}

//-------------------------------------

Vector CAI_TacticalServices::GetNodePos( int node )
{
	return GetNetwork()->GetNode((int)node)->GetPosition(GetHullType());
}

//-----------------------------------------------------------------------------
