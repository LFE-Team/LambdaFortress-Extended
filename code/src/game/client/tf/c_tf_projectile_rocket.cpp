//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_tf_projectile_rocket.h"
#include "particles_new.h"
#include "tf_gamerules.h"
#include "tempent.h"
#include "iefx.h"
#include "dlight.h"
#include "c_te_legacytempents.h"

extern ConVar lfe_muzzlelight;

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Rocket, DT_TFProjectile_Rocket )

BEGIN_NETWORK_TABLE( C_TFProjectile_Rocket, DT_TFProjectile_Rocket )
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::C_TFProjectile_Rocket( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::~C_TFProjectile_Rocket( void )
{
	ParticleProp()->StopEmission();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateRocketTrails();
		CreateLightEffects();
	}

	// Watch team changes and change trail accordingly.
	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateRocketTrails();
		CreateLightEffects();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::CreateRocketTrails( void )
{
	if ( IsDormant() )
		return;

	if ( enginetrace->GetPointContents( GetAbsOrigin() ) & MASK_WATER )
	{
		ParticleProp()->Create( "rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail" );
	}
	else
	{
		const char *pszParticleEffect = "rockettrail";
 		// Halloween Spell Effect Check
		int iHalloweenSpell = 0;
		CALL_ATTRIB_HOOK_INT_ON_OTHER( GetOwnerEntity(), iHalloweenSpell, halloween_pumpkin_explosions );

 		if ( iHalloweenSpell != 0 )
		{
			pszParticleEffect = "halloween_rockettrail";
		}
 		ParticleProp()->Create( pszParticleEffect, PATTACH_POINT_FOLLOW, "trail"  );
	}

	if ( m_bCritical )
	{
		const char *pszEffectName = ConstructTeamParticle( "critical_rocket_%s", GetTeamNumber() );
		ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
	}
}

void C_TFProjectile_Rocket::CreateLightEffects( void )
{
	// Handle the dynamic light
	if (lfe_muzzlelight.GetBool())
	{
		AddEffects( EF_DIMLIGHT );

		dlight_t *dl;
		if ( IsEffectActive( EF_DIMLIGHT ) )
		{	
			dl = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );
			dl->origin = GetAbsOrigin();
			dl->color.r = 255;
			dl->color.g = 100;
			dl->color.b = 10;
			switch ( GetTeamNumber() )
			{
			case TF_TEAM_RED:
				if ( !m_bCritical ) {
				dl->color.r = 255; dl->color.g = 100; dl->color.b = 10; }
				else {
				dl->color.r = 255; dl->color.g = 10; dl->color.b = 10; }
			break;

			case TF_TEAM_BLUE:
				if ( !m_bCritical ) {
				dl->color.r = 255; dl->color.g = 100; dl->color.b = 10; }
				else {
				dl->color.r = 10; dl->color.g = 10; dl->color.b = 255; }
				break;
			}
			dl->die = gpGlobals->curtime + 0.01f;
			dl->radius = 340.f;
			dl->decay = 512.0f;

			tempents->RocketFlare( GetAbsOrigin() );
		}
	}
}
