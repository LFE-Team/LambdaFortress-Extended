//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_ai_basenpc.h"
#include "engine/ivdebugoverlay.h"

#if defined( HL2_DLL ) || defined( HL2_EPISODIC )
#include "c_basehlplayer.h"
#endif

#include "death_pose.h"

#ifdef TF_CLASSIC_CLIENT
#include "c_tf_player.h"
#include "tf_shareddefs.h"
#include "iclientmode.h"
#include "vgui/ILocalize.h"
#include "soundenvelope.h"
#include "IEffects.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined( CAI_BaseNPC )
#undef CAI_BaseNPC
#endif

#define PING_MAX_TIME	2.0

IMPLEMENT_CLIENTCLASS_DT( C_AI_BaseNPC, DT_AI_BaseNPC, CAI_BaseNPC )
	RecvPropInt( RECVINFO( m_lifeState ) ),
	RecvPropBool( RECVINFO( m_bPerformAvoidance ) ),
	RecvPropBool( RECVINFO( m_bIsMoving ) ),
	RecvPropBool( RECVINFO( m_bFadeCorpse ) ),
	RecvPropInt( RECVINFO ( m_iDeathPose) ),
	RecvPropInt( RECVINFO( m_iDeathFrame) ),
	RecvPropInt( RECVINFO( m_iHealth ) ),
	RecvPropInt( RECVINFO( m_iMaxHealth ) ),
	RecvPropInt( RECVINFO( m_iSpeedModRadius ) ),
	RecvPropInt( RECVINFO( m_iSpeedModSpeed ) ),
	RecvPropInt( RECVINFO( m_bSpeedModActive ) ),
	RecvPropBool( RECVINFO( m_bImportanRagdoll ) ),
	RecvPropFloat( RECVINFO( m_flTimePingEffect ) ),
	RecvPropString( RECVINFO( m_szClassname ) ),
#ifdef TF_CLASSIC_CLIENT
	RecvPropInt( RECVINFO( m_nPlayerCond ) ),
	RecvPropInt( RECVINFO( m_nPlayerCondEx ) ),
	RecvPropInt( RECVINFO( m_nPlayerCondEx2 ) ),
	RecvPropInt( RECVINFO( m_nPlayerCondEx3 ) ),
	RecvPropArray3( RECVINFO_ARRAY( m_flCondExpireTimeLeft ), RecvPropFloat( RECVINFO( m_flCondExpireTimeLeft[0] ) ) ),
	RecvPropInt( RECVINFO( m_nNumHealers ) ),
	RecvPropBool( RECVINFO( m_bBurningDeath ) ),
	RecvPropInt( RECVINFO( m_nTFFlags ) )
#endif
END_RECV_TABLE()
#ifdef TF_CLASSIC_CLIENT
BEGIN_PREDICTION_DATA_NO_BASE( C_AI_BaseNPC )
	DEFINE_PRED_FIELD( m_nPlayerCond, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nPlayerCondEx, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nPlayerCondEx2, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nPlayerCondEx3, FIELD_INTEGER, FTYPEDESC_INSENDTABLE )
END_PREDICTION_DATA()
#endif
extern ConVar cl_npc_speedmod_intime;

bool NPC_IsImportantNPC( C_BaseAnimating *pAnimating )
{
	C_AI_BaseNPC *pBaseNPC = dynamic_cast < C_AI_BaseNPC* > ( pAnimating );

	if ( pBaseNPC == NULL )
		return false;

	return pBaseNPC->ImportantRagdoll();
}

C_AI_BaseNPC::C_AI_BaseNPC()
{
#ifdef TF_CLASSIC_CLIENT
	m_pBurningSound = NULL;
	m_pBurningEffect = NULL;
	m_flBurnEffectStartTime = 0;
	m_flBurnEffectEndTime = 0;
	m_hRagdoll.Set( NULL );
	m_pCritSound = NULL;
	m_pCritEffect = NULL;
#endif
}

//-----------------------------------------------------------------------------
// Makes ragdolls ignore npcclip brushes
//-----------------------------------------------------------------------------
unsigned int C_AI_BaseNPC::PhysicsSolidMaskForEntity( void ) const 
{
	// This allows ragdolls to move through npcclip brushes
	if ( !IsRagdoll() )
	{
		return MASK_NPCSOLID; 
	}
	return MASK_SOLID;
}


void C_AI_BaseNPC::ClientThink( void )
{
	BaseClass::ClientThink();

#ifdef HL2_DLL
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer*>( C_BasePlayer::GetLocalPlayer() );

	if ( ShouldModifyPlayerSpeed() == true )
	{
		if ( pPlayer )
		{
			float flDist = (GetAbsOrigin() - pPlayer->GetAbsOrigin()).LengthSqr();

			if ( flDist <= GetSpeedModifyRadius() )
			{
				if ( pPlayer->m_hClosestNPC )
				{
					if ( pPlayer->m_hClosestNPC != this )
					{
						float flDistOther = (pPlayer->m_hClosestNPC->GetAbsOrigin() - pPlayer->GetAbsOrigin()).Length();

						//If I'm closer than the other NPC then replace it with myself.
						if ( flDist < flDistOther )
						{
							pPlayer->m_hClosestNPC = this;
							pPlayer->m_flSpeedModTime = gpGlobals->curtime + cl_npc_speedmod_intime.GetFloat();
						}
					}
				}
				else
				{
					pPlayer->m_hClosestNPC = this;
					pPlayer->m_flSpeedModTime = gpGlobals->curtime + cl_npc_speedmod_intime.GetFloat();
				}
			}
		}
	}
#endif // HL2_DLL

#ifdef HL2_EPISODIC
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer*>( C_BasePlayer::GetLocalPlayer() );

	if ( pPlayer && m_flTimePingEffect > gpGlobals->curtime )
	{
		float fPingEffectTime = m_flTimePingEffect - gpGlobals->curtime;
		
		if ( fPingEffectTime > 0.0f )
		{
			Vector vRight, vUp;
			Vector vMins, vMaxs;

			float fFade;

			if( fPingEffectTime <= 1.0f )
			{
				fFade = 1.0f - (1.0f - fPingEffectTime);
			}
			else
			{
				fFade = 1.0f;
			}

			GetRenderBounds( vMins, vMaxs );
			AngleVectors (pPlayer->GetAbsAngles(), NULL, &vRight, &vUp );
			Vector p1 = GetAbsOrigin() + vRight * vMins.x + vUp * vMins.z;
			Vector p2 = GetAbsOrigin() + vRight * vMaxs.x + vUp * vMins.z;
			Vector p3 = GetAbsOrigin() + vUp * vMaxs.z;

			int r = 0 * fFade;
			int g = 255 * fFade;
			int b = 0 * fFade;

			debugoverlay->AddLineOverlay( p1, p2, r, g, b, true, 0.05f );
			debugoverlay->AddLineOverlay( p2, p3, r, g, b, true, 0.05f );
			debugoverlay->AddLineOverlay( p3, p1, r, g, b, true, 0.05f );
		}
	}
#endif
}

void C_AI_BaseNPC::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

#ifdef TF_CLASSIC_CLIENT
	m_iOldTeam = GetTeamNumber();
	m_nOldConditions = m_nPlayerCond;
	m_nOldConditionsEx = m_nPlayerCondEx;
	m_nOldConditionsEx2 = m_nPlayerCondEx2;
	m_nOldConditionsEx3 = m_nPlayerCondEx3;
	m_bWasCritBoosted = IsCritBoosted();
	m_bWasMiniCritBoosted = IsMiniCritBoosted();
#endif
}

void C_AI_BaseNPC::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( ( ShouldModifyPlayerSpeed() == true ) || ( m_flTimePingEffect > gpGlobals->curtime ) )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

#ifdef TF_CLASSIC_CLIENT
	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );

		InitInvulnerableMaterial();
	}

	// Update conditions from last network change
	SyncConditions( m_nPlayerCond, m_nOldConditions, 0, 0 );
	SyncConditions( m_nPlayerCondEx, m_nOldConditionsEx, 0, 32 );
	SyncConditions( m_nPlayerCondEx2, m_nOldConditionsEx2, 0, 64 );
	SyncConditions( m_nPlayerCondEx3, m_nOldConditionsEx3, 0, 96 );

	m_nOldConditions = m_nPlayerCond;
	m_nOldConditionsEx = m_nPlayerCondEx;
	m_nOldConditionsEx2 = m_nPlayerCondEx2;
	m_nOldConditionsEx3 = m_nPlayerCondEx3;

	C_BaseCombatWeapon *pActiveWpn = this->GetActiveWeapon();
	if ( pActiveWpn )
	{
		UpdateCritBoostEffect();
	}

	if ( InCond( TF_COND_BURNING ) && !m_pBurningSound )
	{
		StartBurningSound();
	}
/*
	int nNewWaterLevel = GetWaterLevel();

	if ( nNewWaterLevel != m_nOldWaterLevel )
	{
		if ( ( m_nOldWaterLevel == WL_NotInWater ) && ( nNewWaterLevel > WL_NotInWater ) )
		{
			// Set when we do a transition to/from partially in water to completely out
			m_flWaterEntryTime = gpGlobals->curtime;
		}

		// If player is now up to his eyes in water and has entered the water very recently (not just bobbing eyes in and out), play a bubble effect.
		if ( ( nNewWaterLevel == WL_Eyes ) && ( gpGlobals->curtime - m_flWaterEntryTime ) < 0.5f ) 
		{
			CNewParticleEffect *pEffect = ParticleProp()->Create( "water_playerdive", PATTACH_ABSORIGIN_FOLLOW );
			ParticleProp()->AddControlPoint( pEffect, 1, NULL, PATTACH_WORLDORIGIN, NULL, WorldSpaceCenter() );
		}
		// If player was up to his eyes in water and is now out to waist level or less, play a water drip effect
		else if ( m_nOldWaterLevel == WL_Eyes && ( nNewWaterLevel < WL_Eyes ) && !bJustSpawned )
		{
			CNewParticleEffect *pWaterExitEffect = ParticleProp()->Create( "water_playeremerge", PATTACH_ABSORIGIN_FOLLOW );
			ParticleProp()->AddControlPoint( pWaterExitEffect, 1, this, PATTACH_ABSORIGIN_FOLLOW );
			m_bWaterExitEffectActive = true;
		}
	}
*/
#endif
}

#ifdef TF_CLASSIC_CLIENT
//-----------------------------------------------------------------------------
// Purpose: check the newly networked conditions for changes
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::SyncConditions( int nCond, int nOldCond, int nUnused, int iOffset )
{
	if ( nCond == nOldCond )
		return;

	int nCondChanged = nCond ^ nOldCond;
	int nCondAdded = nCondChanged & nCond;
	int nCondRemoved = nCondChanged & nOldCond;

	int i;
	for ( i = 0; i < 32; i++ )
	{
		if ( nCondAdded & (1<<i) )
		{
			OnConditionAdded( i + iOffset );
		}
		else if ( nCondRemoved & (1<<i) )
		{
			OnConditionRemoved( i + iOffset );
		}
	}
}
#endif

void C_AI_BaseNPC::UpdateOnRemove( void )
{
#ifdef TF_CLASSIC_CLIENT
	ParticleProp()->OwnerSetDormantTo( true );
	ParticleProp()->StopParticlesInvolving( this );

	RemoveAllCond();
#endif

	BaseClass::UpdateOnRemove();
}

bool C_AI_BaseNPC::GetRagdollInitBoneArrays( matrix3x4_t *pDeltaBones0, matrix3x4_t *pDeltaBones1, matrix3x4_t *pCurrentBones, float boneDt )
{
	bool bRet = true;

	if ( !ForceSetupBonesAtTime( pDeltaBones0, gpGlobals->curtime - boneDt ) )
		bRet = false;

	GetRagdollCurSequenceWithDeathPose( this, pDeltaBones1, gpGlobals->curtime, m_iDeathPose, m_iDeathFrame );
	float ragdollCreateTime = PhysGetSyncCreateTime();
	if ( ragdollCreateTime != gpGlobals->curtime )
	{
		// The next simulation frame begins before the end of this frame
		// so initialize the ragdoll at that time so that it will reach the current
		// position at curtime.  Otherwise the ragdoll will simulate forward from curtime
		// and pop into the future a bit at this point of transition
		if ( !ForceSetupBonesAtTime( pCurrentBones, ragdollCreateTime ) )
			bRet = false;
	}
	else
	{
		if ( !SetupBones( pCurrentBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, gpGlobals->curtime ) )
			bRet = false;
	}

	return bRet;
}

#ifdef TF_CLASSIC_CLIENT
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	C_AI_BaseNPC::InternalDrawModel( int flags )
{
	bool bUseInvulnMaterial = InCond( TF_COND_INVULNERABLE );
	if ( bUseInvulnMaterial )
	{
		modelrender->ForcedMaterialOverride( *GetInvulnMaterialRef() );
	}

	int ret = BaseClass::InternalDrawModel( flags );

	if ( bUseInvulnMaterial )
	{
		modelrender->ForcedMaterialOverride( NULL );
	}

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: Don't take damage decals while invulnerable
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::AddDecal( const Vector& rayStart, const Vector& rayEnd,
							const Vector& decalCenter, int hitbox, int decalIndex, bool doTrace, trace_t& tr, int maxLODToDecal )
{
	if ( InCond( TF_COND_STEALTHED ) )
	{
		return;
	}

	if ( InCond( TF_COND_INVULNERABLE ) )
	{ 
		Vector vecDir = rayEnd - rayStart;
		VectorNormalize(vecDir);
		g_pEffects->Ricochet( rayEnd - (vecDir * 8), -vecDir );
		return;
	}

	// don't decal from inside NPC
	if ( tr.startsolid )
	{
		return;
	}

	BaseClass::AddDecal( rayStart, rayEnd, decalCenter, hitbox, decalIndex, doTrace, tr, maxLODToDecal );
}

C_BaseAnimating *C_AI_BaseNPC::BecomeRagdollOnClient()
{
	C_BaseAnimating *pRagdoll = BaseClass::BecomeRagdollOnClient();
	if ( pRagdoll )
	{
		m_hRagdoll.Set( pRagdoll );
		if ( m_bBurningDeath )
			pRagdoll->ParticleProp()->Create( "burningplayer_corpse", PATTACH_ABSORIGIN_FOLLOW );
	}

	return pRagdoll;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
// Output : IRagdoll*
//-----------------------------------------------------------------------------
IRagdoll* C_AI_BaseNPC::GetRepresentativeRagdoll() const
{
	if ( m_hRagdoll.Get() )
	{
		C_BaseAnimating *pRagdoll = static_cast<C_BaseAnimating *>( m_hRagdoll.Get() );
		if ( !pRagdoll )
			return NULL;

		return pRagdoll->m_pRagdoll;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
Vector C_AI_BaseNPC::GetObserverCamOrigin( void )
{
	if ( !IsAlive() )
	{
		IRagdoll *pRagdoll = GetRepresentativeRagdoll();
		if ( pRagdoll )
			return pRagdoll->GetRagdollOrigin();
	}

	return BaseClass::GetObserverCamOrigin();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes )
{
	sIDString[0] = '\0';

	C_TFPlayer *pLocalTFPlayer = C_TFPlayer::GetLocalTFPlayer();
	
	if ( !pLocalTFPlayer )
		return;

	if ( InSameTeam( pLocalTFPlayer ) || pLocalTFPlayer->IsPlayerClass( TF_CLASS_SPY ) || pLocalTFPlayer->GetTeamNumber() == TEAM_SPECTATOR )
	{
		const char *pszClassname = GetClassname();
		wchar_t *wszNPCName;
		wszNPCName = g_pVGuiLocalize->Find( pszClassname );

		if ( !wszNPCName )
		{
			wchar_t wszNPCNameBuf[MAX_PLAYER_NAME_LENGTH];
			g_pVGuiLocalize->ConvertANSIToUnicode( pszClassname, wszNPCNameBuf, sizeof(wszNPCNameBuf) );
			wszNPCName = wszNPCNameBuf;
		}

		const char *printFormatString = NULL;

		if ( pLocalTFPlayer->GetTeamNumber() == TEAM_SPECTATOR || InSameTeam( pLocalTFPlayer ) )
		{
			printFormatString = "#TF_playerid_sameteam";
		}
		else if ( pLocalTFPlayer->IsPlayerClass( TF_CLASS_SPY ) )
		{
			// Spy can see enemy's health.
			printFormatString = "#TF_playerid_diffteam";
		}

		wchar_t *wszPrepend = L"";

		if ( printFormatString )
		{
			g_pVGuiLocalize->ConstructString( sIDString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString), 3, wszPrepend, wszNPCName );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes )
{
	sDataString[0] = '\0';
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::StartBurningSound( void )
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( !m_pBurningSound )
	{
		CLocalPlayerFilter filter;
		m_pBurningSound = controller.SoundCreate( filter, entindex(), "Player.OnFire" );
	}

	controller.Play( m_pBurningSound, 0.0, 100 );
	controller.SoundChangeVolume( m_pBurningSound, 1.0, 0.1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::StopBurningSound( void )
{
	if ( m_pBurningSound )
	{
		CSoundEnvelopeController::GetController().SoundDestroy( m_pBurningSound );
		m_pBurningSound = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::InitInvulnerableMaterial( void )
{
	const char *pszMaterial = NULL;

	int iTeam = GetTeamNumber();

	switch ( iTeam )
	{
		case TF_TEAM_RED:
			pszMaterial = "models/effects/invulnfx_red.vmt";
			break;
		case TF_TEAM_BLUE:
			pszMaterial = "models/effects/invulnfx_blue.vmt";
			break;
		case TF_TEAM_GREEN:
			pszMaterial = "models/effects/invulnfx_green.vmt";
			break;
		case TF_TEAM_YELLOW:
			pszMaterial = "models/effects/invulnfx_yellow.vmt";
			break;
		default:
			break;
	}

	if ( pszMaterial )
	{
		m_InvulnerableMaterial.Init( pszMaterial, TEXTURE_GROUP_CLIENT_EFFECTS );
	}
	else
	{
		m_InvulnerableMaterial.Shutdown();
	}
}

void C_AI_BaseNPC::UpdateCritBoostEffect( bool bForceHide /*= false*/ )
{
	bool bShouldShow = !bForceHide;

	if ( bShouldShow )
	{
		if ( !IsCritBoosted() )
		{
			bShouldShow = false;
		}
		else if ( IsStealthed() )
		{
			bShouldShow = false;
		}
	}

	if ( bShouldShow )
	{
		// Update crit effect model.
		if ( m_pCritEffect )
		{
			if ( m_hCritEffectHost.Get() )
				m_hCritEffectHost->ParticleProp()->StopEmission( m_pCritEffect );

			m_pCritEffect = NULL;
		}

			C_BaseCombatWeapon *pWeapon = this->GetActiveWeapon();

		// Don't add crit effect to weapons without a model.
		if ( pWeapon && pWeapon->GetWorldModelIndex() != 0 )
		{
			m_hCritEffectHost = pWeapon;
		}
		else
		{
			m_hCritEffectHost = this;
		}

		if ( m_hCritEffectHost.Get() )
		{
			const char *pszEffect = ConstructTeamParticle( "critgun_weaponmodel_%s", this->GetTeamNumber(), true, g_aTeamNamesShort );
			m_pCritEffect = m_hCritEffectHost->ParticleProp()->Create( pszEffect, PATTACH_ABSORIGIN_FOLLOW );
		}

		if ( !m_pCritSound )
		{
			CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();
			CLocalPlayerFilter filter;
			m_pCritSound = controller.SoundCreate( filter, this->entindex(), "Weapon_General.CritPower" );
			controller.Play( m_pCritSound, 1.0, 100 );
		}
	}
	else
	{
		if ( m_pCritEffect )
		{
			if ( m_hCritEffectHost.Get() )
				m_hCritEffectHost->ParticleProp()->StopEmission( m_pCritEffect );

			m_pCritEffect = NULL;
		}

		m_hCritEffectHost = NULL;

		if ( m_pCritSound )
		{
			CSoundEnvelopeController::GetController().SoundDestroy( m_pCritSound );
			m_pCritSound = NULL;
		}
	}
}
#endif