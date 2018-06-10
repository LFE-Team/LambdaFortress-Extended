//=============================================================================//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "ai_basenpc_shared.h"
#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
#include "tf_shareddefs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
TF_NPCData g_aNPCData[] =
{
	// Friendly actors.
	{
		"npc_dog",
		TF_TEAM_RED,
		TFFL_BUILDING,
	},
	{
		"npc_eli",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_fisherman",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_gman",
		TF_TEAM_RED,
		TFFL_NOBACKSTAB | TFFL_NOHEALING | TFFL_FIREPROOF,
	},
	{
		"npc_kleiner",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_magnusson",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_mossman",
		TF_TEAM_RED,
		0,
	},
	// Vital allies.
	{
		"npc_alyx",
		TF_TEAM_RED,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_barney",
		TF_TEAM_RED,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_monk",
		TF_TEAM_RED,
		TFFL_NOBACKSTAB,
	},
	// Regular allies.
	{
		"npc_citizen",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_vortigaunt",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_conscript",
		TF_TEAM_BLUE,
		0,
	},
	{
		"npc_conscriptred",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_crematorred",
		TF_TEAM_RED,
		0,
	},
	{
		"npc_combine_synth",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_scientist",
		TF_TEAM_RED,
		0,
	},
	{
		"monster_barney",
		TF_TEAM_RED,
		0,
	},
	{
		"monster_human_grunt",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_alien_grunt",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_alien_slave",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_vortigaunt",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_headcrab",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_zombie",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_bullchicken",
		TF_TEAM_YELLOW,
		0,
	},
	{
		"monster_apache",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_nihilanth",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_hornet",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_snark",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_alien_controller",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_gargantua",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_ichthyosaur",
		TF_TEAM_YELLOW,
		0,
	},
	{
		"monster_barnacle",
		TF_TEAM_YELLOW,
		0,
	},
	{
		"monster_osprey",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_sentry",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_tentacle",
		TF_TEAM_GREEN,
		0,
	},
	{
		"monster_gman",
		TF_TEAM_RED,
		0,
	},
	{
		"monster_human_assassin",
		TF_TEAM_BLUE,
		0,
	},
	{
		"monster_leech",
		TF_TEAM_YELLOW,
		0,
	},
	{
		"monster_sitting_scientist",
		TF_TEAM_RED,
		0,
	},
	{
		"monster_houndeye",
		TF_TEAM_YELLOW,
		0,
	},
	// Enemy actors.
	{
		"npc_citizen",
		TF_TEAM_BLUE,
		0,
	},
	// Regular enemies.
	{
		"npc_combine_s",
		TF_TEAM_BLUE,
		0,
	},
	{
		"npc_metropolice",
		TF_TEAM_BLUE,
		0,
	},
	{
		"npc_stalker",
		TF_TEAM_BLUE,
		0,
	},
	{
		"npc_cremator",
		TF_TEAM_BLUE,
		0,
	},
	// Regular combine mechs.
	{
		"npc_cscanner",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_clawscanner",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_manhack",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	// Indestructible combine mechs.
	{
		"npc_combine_camera",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_rollermine",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_rollerminered",
		TF_TEAM_RED,
		TFFL_BUILDING,
	},
	{
		"npc_rollermine_hackable",
		TF_TEAM_RED,
		TFFL_BUILDING,
	},
	{
		"npc_rollermine_ep1",
		TF_TEAM_RED,
		TFFL_BUILDING,
	},
	{
		"npc_turret_ceiling",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_turret_floor",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_turret_ground",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	// Combine synths.
	{
		"npc_combinegunship",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_hunter",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_strider",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_helicopter",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_combinedropship",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_combineguard",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	// Misc Combine NPCs.
	{
		"npc_crabsynth",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_mortarsynth",
		TF_TEAM_BLUE,
		TFFL_BUILDING,
	},
	{
		"npc_sniper",
		TF_TEAM_BLUE,
		TFFL_NOHEALING,
	},
	// Headcrabs.
	{
		"npc_headcrab",
		TF_TEAM_GREEN,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_headcrab_fast",
		TF_TEAM_GREEN,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_headcrab_black",
		TF_TEAM_GREEN,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_headcrab_poison",
		TF_TEAM_GREEN,
		TFFL_NOBACKSTAB,
	},
	// Zombies.
	{
		"npc_zombie",
		TF_TEAM_GREEN,
		0,
	},
	{
		"npc_zombie_torso",
		TF_TEAM_GREEN,
		0,
	},
	{
		"npc_fastzombie",
		TF_TEAM_GREEN,
		0,
	},
	{
		"npc_fastzombie_torso",
		TF_TEAM_GREEN,
		0,
	},
	{
		"npc_poisonzombie",
		TF_TEAM_GREEN,
		0,
	},
	{
		"npc_zombine",
		TF_TEAM_GREEN,
		0,
	},
	// Antlions.
	{
		"npc_antlion",
		TF_TEAM_YELLOW,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_antlion_worker",
		TF_TEAM_YELLOW,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_antlionguard",
		TF_TEAM_YELLOW,
		TFFL_NOBACKSTAB,
	},
	// Neutral NPCs.
	{
		"npc_barnacle",
		TEAM_UNASSIGNED,
		TFFL_NOBACKSTAB | TFFL_NOHEALING,
	},
	// Birds
	{
		"npc_crow",
		TEAM_UNASSIGNED,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_pigeon",
		TEAM_UNASSIGNED,
		TFFL_NOBACKSTAB,
	},
	{
		"npc_seagull",
		TEAM_UNASSIGNED,
		TFFL_NOBACKSTAB,
	},
	// End marker.
	{
		NULL,
		0,
		0,
	},
};

extern ConVar tf_max_health_boost;

//-----------------------------------------------------------------------------
// Purpose: Add a condition and duration
// duration of PERMANENT_CONDITION means infinite duration
//-----------------------------------------------------------------------------
void CAI_BaseNPC::AddCond( int nCond, float flDuration /* = PERMANENT_CONDITION */ )
{
#ifdef GAME_DLL
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	int nCondFlag = nCond;
	int *pVar = NULL;
	if ( nCond < 96 )
	{
		if ( nCond < 64 )
		{
			if ( nCond < 32 )
			{
				pVar = &m_nPlayerCond.GetForModify();
			}
			else
			{
				pVar = &m_nPlayerCondEx.GetForModify();
				nCondFlag -= 32;
			}
		}
		else
		{
			pVar = &m_nPlayerCondEx2.GetForModify();
			nCondFlag -= 64;
		}
	}
	else
	{
		pVar = &m_nPlayerCondEx3.GetForModify();
		nCondFlag -= 96;
	}

	*pVar |= ( 1 << nCondFlag );
	m_flCondExpireTimeLeft.Set( nCond, flDuration );
#endif
	OnConditionAdded( nCond );
}

//-----------------------------------------------------------------------------
// Purpose: Forcibly remove a condition
//-----------------------------------------------------------------------------
void CAI_BaseNPC::RemoveCond( int nCond )
{
#ifdef GAME_DLL
	Assert( nCond >= 0 && nCond < TF_COND_LAST );
	int nCondFlag = nCond;
	int *pVar = NULL;
	if ( nCond < 96 )
	{
		if ( nCond < 64 )
		{
			if ( nCond < 32 )
			{
				pVar = &m_nPlayerCond.GetForModify();
			}
			else
			{
				pVar = &m_nPlayerCondEx.GetForModify();
				nCondFlag -= 32;
			}
		}
		else
		{
			pVar = &m_nPlayerCondEx2.GetForModify();
			nCondFlag -= 64;
		}
	}
	else
	{
		pVar = &m_nPlayerCondEx3.GetForModify();
		nCondFlag -= 96;
	}

	*pVar &= ~( 1 << nCondFlag );
	m_flCondExpireTimeLeft.Set( nCond, 0 );
#endif
	OnConditionRemoved( nCond );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CAI_BaseNPC::InCond( int nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );
/*
	int nCondFlag = nCond;
	const int *pVar = NULL;

	if ( nCond < 96 )
	{
		if ( nCond < 64 )
		{
			if ( nCond < 32 )
			{
				pVar = &m_nPlayerCond.Get();
			}
			else
			{
				pVar = &m_nPlayerCondEx.Get();
				nCondFlag -= 32;
			}
		}
		else
		{
			pVar = &m_nPlayerCondEx2.Get();
			nCondFlag -= 64;
		}
	}
	else
	{
		pVar = &m_nPlayerCondEx3.Get();
		nCondFlag -= 96;
	}

	return ( ( *pVar & ( 1 << nCondFlag ) ) != 0 );
*/
	return ( ( m_nPlayerCond /*|| m_nPlayerCondEx || m_nPlayerCondEx2 || m_nPlayerCondEx3*/ & (1<<nCond) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CAI_BaseNPC::GetConditionDuration( int nCond )
{
#ifdef GAME_DLL
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	if ( InCond( nCond ) )
	{
		return m_flCondExpireTimeLeft[nCond];
	}
#endif
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Remove any conditions affecting players
//-----------------------------------------------------------------------------
void CAI_BaseNPC::RemoveAllCond( void )
{
	int i;
	for ( i = 0; i < TF_COND_LAST; i++ )
	{
		if ( InCond( i ) )
		{
			RemoveCond( i );
		}
	}

	// Now remove all the rest
	m_nPlayerCond = 0;
	m_nPlayerCondEx = 0;
	m_nPlayerCondEx2 = 0;
	m_nPlayerCondEx3 = 0;
}


//-----------------------------------------------------------------------------
// Purpose: Called on both client and server. Server when we add the bit,
// and client when it recieves the new cond bits and finds one added
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnConditionAdded( int nCond )
{
	switch( nCond )
	{
	case TF_COND_HEALTH_BUFF:
#ifdef GAME_DLL
		m_flHealFraction = 0;
#endif
		break;

	case TF_COND_INVULNERABLE:
		OnAddInvulnerable();
		break;

	case TF_COND_BURNING:
		OnAddBurning();
		break;

	case TF_COND_HEALTH_OVERHEALED:
		//UpdateOverhealEffect();
		break;

	case TF_COND_SLOWED:
		OnAddSlowed();
		break;
/*
	case TF_COND_DISGUISING:
		OnAddDisguising();
		break;

	case TF_COND_DISGUISED:
		OnAddDisguised();
		break;

	case TF_COND_TAUNTING:
		{
			CTFWeaponBase *pWpn = m_pOuter->GetActiveTFWeapon();
			if ( pWpn )
			{
				// cancel any reload in progress.
				pWpn->AbortReload();
			}
		}
		break;
*/
	case TF_COND_CRITBOOSTED:
	case TF_COND_CRITBOOSTED_PUMPKIN:
	case TF_COND_CRITBOOSTED_USER_BUFF:
	case TF_COND_CRITBOOSTED_DEMO_CHARGE:
	case TF_COND_CRITBOOSTED_FIRST_BLOOD:
	case TF_COND_CRITBOOSTED_BONUS_TIME:
	case TF_COND_CRITBOOSTED_CTF_CAPTURE:
	case TF_COND_CRITBOOSTED_ON_KILL:
	case TF_COND_CRITBOOSTED_CARD_EFFECT:
	case TF_COND_CRITBOOSTED_RUNE_TEMP:
		OnAddCritboosted();
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called on both client and server. Server when we remove the bit,
// and client when it recieves the new cond bits and finds one removed
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnConditionRemoved( int nCond )
{
	switch( nCond )
	{
/*
	case TF_COND_ZOOMED:
		OnRemoveZoomed();
		break;
*/
	case TF_COND_BURNING:
		OnRemoveBurning();
		break;

	case TF_COND_HEALTH_BUFF:
#ifdef GAME_DLL
		m_flHealFraction = 0;
#endif
		break;

	case TF_COND_HEALTH_OVERHEALED:
		//UpdateOverhealEffect();
		break;

	case TF_COND_SLOWED:
		OnRemoveSlowed();
		break;

	case TF_COND_INVULNERABLE:
		OnRemoveInvulnerable();
		break;

	case TF_COND_CRITBOOSTED:
	case TF_COND_CRITBOOSTED_PUMPKIN:
	case TF_COND_CRITBOOSTED_USER_BUFF:
	case TF_COND_CRITBOOSTED_DEMO_CHARGE:
	case TF_COND_CRITBOOSTED_FIRST_BLOOD:
	case TF_COND_CRITBOOSTED_BONUS_TIME:
	case TF_COND_CRITBOOSTED_CTF_CAPTURE:
	case TF_COND_CRITBOOSTED_ON_KILL:
	case TF_COND_CRITBOOSTED_CARD_EFFECT:
	case TF_COND_CRITBOOSTED_RUNE_TEMP:
		OnRemoveCritboosted();
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CAI_BaseNPC::IsCritBoosted( void )
{
	// Oh man...
	if ( InCond( TF_COND_CRITBOOSTED ) ||
		InCond( TF_COND_CRITBOOSTED_PUMPKIN ) ||
		InCond( TF_COND_CRITBOOSTED_USER_BUFF ) ||
		InCond( TF_COND_CRITBOOSTED_DEMO_CHARGE ) ||
		InCond( TF_COND_CRITBOOSTED_FIRST_BLOOD ) ||
		InCond( TF_COND_CRITBOOSTED_BONUS_TIME ) ||
		InCond( TF_COND_CRITBOOSTED_CTF_CAPTURE ) ||
		InCond( TF_COND_CRITBOOSTED_ON_KILL ) ||
		InCond( TF_COND_CRITBOOSTED_CARD_EFFECT ) ||
		InCond( TF_COND_CRITBOOSTED_RUNE_TEMP ) )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CAI_BaseNPC::IsInvulnerable( void )
{
	// Oh man again...
	if ( InCond( TF_COND_INVULNERABLE ) ||
		InCond( TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGE ) ||
		InCond( TF_COND_INVULNERABLE_USER_BUFF ) ||
		InCond( TF_COND_INVULNERABLE_CARD_EFFECT ) ||
		InCond( TF_COND_INVULNERABLE_SPAWN_PROTECT ) )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CAI_BaseNPC::IsStealthed( void )
{
	if ( InCond( TF_COND_STEALTHED ) ||
		InCond( TF_COND_STEALTHED_USER_BUFF ) ||
		InCond( TF_COND_POWERUP_CLOAK ) )
		return true;

	return false;
}

int CAI_BaseNPC::GetMaxBuffedHealth( void )
{
	float flBoostMax = GetMaxHealth() * tf_max_health_boost.GetFloat();

	int iRoundDown = floor( flBoostMax / 5 );
	iRoundDown = iRoundDown * 5;

	return iRoundDown;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::Burn( CTFPlayer *pAttacker, CTFWeaponBase *pWeapon /*= NULL*/, float flFlameDuration /*= -1.0f*/ )
{
#ifdef CLIENT_DLL

#else
	// Don't bother igniting players who have just been killed by the fire damage.
	if ( !IsAlive() )
		return;

	if ( !InCond( TF_COND_BURNING ) )
	{
		// Start burning
		AddCond( TF_COND_BURNING );
		m_flFlameBurnTime = gpGlobals->curtime;	//asap
		// let the attacker know he burned me
		if ( pAttacker  )
		{
			pAttacker->OnBurnOther( this );
		}
	}

	float flFlameLife = TF_BURNING_FLAME_LIFE;

	if ( flFlameDuration != -1.0f )
		flFlameLife = flFlameDuration;

	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pAttacker, flFlameLife, mult_wpn_burntime );

	m_flFlameRemoveTime = gpGlobals->curtime + flFlameLife;
	m_hBurnAttacker = pAttacker;
	m_hBurnWeapon = pWeapon;

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnAddBurning( void )
{
#ifdef CLIENT_DLL
	// Start the burning effect
	if ( !m_pBurningEffect )
	{
		const char *pEffectName = ( GetTeamNumber() == TF_TEAM_BLUE ) ? "burningplayer_blue" : "burningplayer_red";
		m_pBurningEffect = ParticleProp()->Create( pEffectName, PATTACH_ABSORIGIN_FOLLOW );

		m_flBurnEffectStartTime = gpGlobals->curtime;
		m_flBurnEffectEndTime = gpGlobals->curtime + TF_BURNING_FLAME_LIFE;
	}
#endif

	// play a fire-starting sound
	EmitSound( "Fire.Engulf" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnRemoveBurning( void )
{
#ifdef CLIENT_DLL
	StopBurningSound();

	if ( m_pBurningEffect )
	{
		ParticleProp()->StopEmission( m_pBurningEffect );
		m_pBurningEffect = NULL;
	}

	m_flBurnEffectStartTime = 0;
	m_flBurnEffectEndTime = 0;
#else
	m_hBurnAttacker = NULL;
#endif
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnAddInvulnerable( void )
{
	// Stock uber removes negative conditions.
	if ( InCond( TF_COND_BURNING ) )
	{
		RemoveCond( TF_COND_BURNING );
	}

	if ( InCond( TF_COND_SLOWED ) )
	{
		RemoveCond( TF_COND_SLOWED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnRemoveInvulnerable( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnAddSlowed( void )
{
	m_flPlaybackRate = 0.6;
	m_flGroundSpeed = 0.6;
}

//-----------------------------------------------------------------------------
// Purpose: Remove slowdown effect
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnRemoveSlowed( void )
{
	m_flPlaybackRate = 1.0;
	m_flGroundSpeed = 1.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnAddCritboosted( void )
{
#ifdef CLIENT_DLL
	UpdateCritBoostEffect();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnRemoveCritboosted( void )
{
#ifdef CLIENT_DLL
	UpdateCritBoostEffect();
#endif
}