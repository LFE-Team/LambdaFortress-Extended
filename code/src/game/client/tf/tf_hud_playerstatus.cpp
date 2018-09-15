//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "hud_numericdisplay.h"
#include "c_team.h"
#include "c_tf_player.h"
#include "tf_shareddefs.h"
#include "tf_hud_playerstatus.h"
#include "tf_hud_target_id.h"

using namespace vgui;


extern ConVar tf_max_health_boost;


static char *g_szBlueClassImages[] = 
{ 
	"",
	"../hud/class_scoutblue", 
	"../hud/class_sniperblue",
	"../hud/class_soldierblue",
	"../hud/class_demoblue",
	"../hud/class_medicblue",
	"../hud/class_heavyblue",
	"../hud/class_pyroblue",
	"../hud/class_spyblue",
	"../hud/class_engiblue",
	"",
};

static char *g_szRedClassImages[] = 
{ 
	"",
	"../hud/class_scoutred", 
	"../hud/class_sniperred",
	"../hud/class_soldierred",
	"../hud/class_demored",
	"../hud/class_medicred",
	"../hud/class_heavyred",
	"../hud/class_pyrored",
	"../hud/class_spyred",
	"../hud/class_engired",
	"",
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudPlayerClass::CTFHudPlayerClass( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pClassImage = new CTFClassImage( this, "PlayerStatusClassImage" );
	//m_pClassImageBG = new CTFClassImage(this, "PlayerStatusClassImageBG");
	m_pSpyImage = new CTFImagePanel( this, "PlayerStatusSpyImage" );
	m_pSpyOutlineImage = new CTFImagePanel( this, "PlayerStatusSpyOutlineImage" );

	// This is used by retail TF2 to display the characters as models instead of images
	//m_pClassModelPanel = new CTFPlayerModelPanel(this, "classmodelpanel");
	//m_pClassModelPanelBG = new CTFImagePanel(this, "classmodelpanelBG");
	// It's activated using cl_hud_playerclass_use_playermodel. The description for it is
	// "Use player model in player class HUD."

	m_nTeam = TEAM_UNASSIGNED;
	m_nClass = TF_CLASS_UNDEFINED;
	m_nDisguiseTeam = TEAM_UNASSIGNED;
	m_nDisguiseClass = TF_CLASS_UNDEFINED;
	m_flNextThink = 0.0f;

	ListenForGameEvent( "localplayer_changedisguise" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerClass::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;

	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "HudSpyDisguiseHide" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerClass::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudPlayerClass.res" );

	m_nTeam = TEAM_UNASSIGNED;
	m_nClass = TF_CLASS_UNDEFINED;
	m_nDisguiseTeam = TEAM_UNASSIGNED;
	m_nDisguiseClass = TF_CLASS_UNDEFINED;
	m_flNextThink = 0.0f;
	m_nCloakLevel = 0;

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerClass::OnThink()
{
	if ( m_flNextThink < gpGlobals->curtime )
	{
		C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
		bool bTeamChange = false;

		if ( pPlayer )
		{
			// set our background colors
			if ( m_nTeam != pPlayer->GetTeamNumber() )
			{
				bTeamChange = true;
				m_nTeam = pPlayer->GetTeamNumber();
			}

			int nCloakLevel = 0;
			bool bCloakChange = false;
			float flInvis = pPlayer->GetPercentInvisible();

			if ( flInvis > 0.9 )
			{
				nCloakLevel = 2;
			}
			else if ( flInvis > 0.1 )
			{
				nCloakLevel = 1;
			}

			if ( nCloakLevel != m_nCloakLevel )
			{
				m_nCloakLevel = nCloakLevel;
				bCloakChange = true;
			}

			// set our class image
			if ( m_nClass != pPlayer->GetPlayerClass()->GetClassIndex() || bTeamChange || bCloakChange ||
				( m_nClass == TF_CLASS_SPY && m_nDisguiseClass != pPlayer->m_Shared.GetDisguiseClass() ) ||
				( m_nClass == TF_CLASS_SPY && m_nDisguiseTeam != pPlayer->m_Shared.GetDisguiseTeam() ) )
			{
				m_nClass = pPlayer->GetPlayerClass()->GetClassIndex();

				if ( m_nClass == TF_CLASS_SPY && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
				{
					if ( !pPlayer->m_Shared.InCond( TF_COND_DISGUISING ) )
					{
						m_nDisguiseTeam = pPlayer->m_Shared.GetDisguiseTeam();
						m_nDisguiseClass = pPlayer->m_Shared.GetDisguiseClass();
					}
				}
				else
				{
					m_nDisguiseTeam = TEAM_UNASSIGNED;
					m_nDisguiseClass = TF_CLASS_UNDEFINED;
				}

				if ( m_pClassImage && m_pSpyImage )
				{
					int iCloakState = 0;
					if ( pPlayer->IsPlayerClass( TF_CLASS_SPY ) )
					{
						iCloakState = m_nCloakLevel;
					}

					if ( m_nDisguiseTeam != TEAM_UNASSIGNED || m_nDisguiseClass != TF_CLASS_UNDEFINED )
					{
						m_pSpyImage->SetVisible( true );
						m_pClassImage->SetClass( m_nDisguiseTeam, m_nDisguiseClass, iCloakState );
					}
					else
					{
						m_pSpyImage->SetVisible( false );
						m_pClassImage->SetClass( m_nTeam, m_nClass, iCloakState );
					}
				}
			}
		}

		m_flNextThink = gpGlobals->curtime + 0.05f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerClass::FireGameEvent( IGameEvent * event )
{
	if ( FStrEq( "localplayer_changedisguise", event->GetName() ) )
	{
		if ( m_pSpyImage && m_pSpyOutlineImage )
		{
			bool bFadeIn = event->GetBool( "disguised", false );

			if ( bFadeIn )
			{
				m_pSpyImage->SetAlpha( 0 );
			}
			else
			{
				m_pSpyImage->SetAlpha( 255 );
			}

			m_pSpyOutlineImage->SetAlpha( 0 );
			
			m_pSpyImage->SetVisible( true );
			m_pSpyOutlineImage->SetVisible( true );

			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( bFadeIn ? "HudSpyDisguiseFadeIn" : "HudSpyDisguiseFadeOut" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHealthPanel::CTFHealthPanel( Panel *parent, const char *name ) : vgui::Panel( parent, name )
{
	m_flHealth = 1.0f;

	m_iMaterialIndex = surface()->DrawGetTextureId( "hud/health_color" );
	if ( m_iMaterialIndex == -1 ) // we didn't find it, so create a new one
	{
		m_iMaterialIndex = surface()->CreateNewTextureID();	
	}

	surface()->DrawSetTextureFile( m_iMaterialIndex, "hud/health_color", true, false );

	m_iDeadMaterialIndex = surface()->DrawGetTextureId( "hud/health_dead" );
	if ( m_iDeadMaterialIndex == -1 ) // we didn't find it, so create a new one
	{
		m_iDeadMaterialIndex = surface()->CreateNewTextureID();	
	}
	surface()->DrawSetTextureFile( m_iDeadMaterialIndex, "hud/health_dead", true, false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHealthPanel::Paint()
{
	BaseClass::Paint();

	int x, y, w, h;
	GetBounds( x, y, w, h );

	Vertex_t vert[4];	
	float uv1 = 0.0f;
	float uv2 = 1.0f;
	int xpos = 0, ypos = 0;

	if ( m_flHealth <= 0 )
	{
		// Draw the dead material
		surface()->DrawSetTexture( m_iDeadMaterialIndex );
		
		vert[0].Init( Vector2D( xpos, ypos ), Vector2D( uv1, uv1 ) );
		vert[1].Init( Vector2D( xpos + w, ypos ), Vector2D( uv2, uv1 ) );
		vert[2].Init( Vector2D( xpos + w, ypos + h ), Vector2D( uv2, uv2 ) );				
		vert[3].Init( Vector2D( xpos, ypos + h ), Vector2D( uv1, uv2 ) );

		surface()->DrawSetColor( Color(255,255,255,255) );
	}
	else
	{
		float flDamageY = h * ( 1.0f - m_flHealth );

		// blend in the red "damage" part
		surface()->DrawSetTexture( m_iMaterialIndex );

		Vector2D uv11( uv1, uv2 - m_flHealth );
		Vector2D uv21( uv2, uv2 - m_flHealth );
		Vector2D uv22( uv2, uv2 );
		Vector2D uv12( uv1, uv2 );

		vert[0].Init( Vector2D( xpos, flDamageY ), uv11 );
		vert[1].Init( Vector2D( xpos + w, flDamageY ), uv21 );
		vert[2].Init( Vector2D( xpos + w, ypos + h ), uv22 );				
		vert[3].Init( Vector2D( xpos, ypos + h ), uv12 );

		surface()->DrawSetColor( GetFgColor() );
	}

	surface()->DrawTexturedPolygon( 4, vert );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudPlayerHealth::CTFHudPlayerHealth( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pHealthImage = new CTFHealthPanel( this, "PlayerStatusHealthImage" );	
	m_pHealthImageBG = new ImagePanel( this, "PlayerStatusHealthImageBG" );
	m_pHealthBonusImage = new ImagePanel( this, "PlayerStatusHealthBonusImage" );

	m_pHealthImageBuildingBG = new ImagePanel( this, "BuildingStatusHealthImageBG" );	

	// bleeds
	m_pHealthBleedImage = new ImagePanel( this, "PlayerStatusBleedImage" );
	//m_pHealthHookBleedImage = new ImagePanel( this, "PlayerStatusHookBleedImage" );
	m_pHealthHookBleedImage = dynamic_cast< ImagePanel * >(FindChildByName("PlayerStatusHookBleedImage"));

	// conds
	m_pHealthMilkImage = new ImagePanel( this, "PlayerStatusMilkImage" );	
	m_pHealthGasImage = new ImagePanel( this, "PlayerStatusGasImage" );	

	// fan o war and others.
	m_pHealthMiniCritImage = new ImagePanel( this, "PlayerStatusMarkedForDeathImage" );	
	m_pHealthMiniCritSlientImage = new ImagePanel( this, "PlayerStatusMarkedForDeathSilentImage" );	

	// vaccinator
	m_pHealthMedicBulletResist = new ImagePanel( this, "PlayerStatus_MedicUberBulletResistImage" );	
	m_pHealthMedicBlastResist = new ImagePanel( this, "PlayerStatus_MedicUberBlastResistImage" );	
	m_pHealthMedicFireResist = new ImagePanel( this, "PlayerStatus_MedicUberFireResistImage" );	
	m_pHealthMedicSmallBulletResist = new ImagePanel( this, "PlayerStatus_MedicSmallBulletResistImage" );	
	m_pHealthMedicSmallBlastResist = new ImagePanel( this, "PlayerStatus_MedicSmallBlastResistImage" );	
	m_pHealthMedicSmallFireResist = new ImagePanel( this, "PlayerStatus_MedicSmallFireResistImage" );	

	// ghost fort wheel of doom
	m_pHealthWheelOfDoom = new ImagePanel( this, "PlayerStatus_WheelOfDoom" );

	// banners
	m_pHealthSoldierOffenseBuff = new ImagePanel( this, "PlayerStatus_SoldierOffenseBuff" );	
	m_pHealthSoldierDefenseBuff = new ImagePanel( this, "PlayerStatus_SoldierDefenseBuff" );	
	m_pHealthSoldierHealOnHitBuff = new ImagePanel( this, "PlayerStatus_SoldierHealOnHitBuff" );	

	m_pHealthSpyMarked = new ImagePanel( this, "PlayerStatus_SpyMarked" );	

	// love & war
	m_pHealthParachute = new ImagePanel( this, "PlayerStatus_Parachute" );	

	// mannpower
	m_pHealthRuneStrength = new ImagePanel( this, "PlayerStatus_RuneStrength" );	
	m_pHealthRuneHaste = new ImagePanel( this, "PlayerStatus_RuneHaste" );	
	m_pHealthRuneRegen = new ImagePanel( this, "PlayerStatus_RuneRegen" );	
	m_pHealthRuneResist = new ImagePanel( this, "PlayerStatus_RuneResist" );	
	m_pHealthRuneVampire = new ImagePanel( this, "PlayerStatus_RuneVampire" );	
	m_pHealthRuneReflect = new ImagePanel( this, "PlayerStatus_RuneReflect" );	
	m_pHealthRunePrecision = new ImagePanel( this, "PlayerStatus_RunePrecision" );	
	m_pHealthRuneAgility = new ImagePanel( this, "PlayerStatus_RuneAgility" );	
	m_pHealthRuneKnockout = new ImagePanel( this, "PlayerStatus_RuneKnockout" );	
	m_pHealthRuneKing = new ImagePanel( this, "PlayerStatus_RuneKing" );	
	m_pHealthRunePlague = new ImagePanel( this, "PlayerStatus_RunePlague" );	
	m_pHealthRuneSupernova = new ImagePanel( this, "PlayerStatus_RuneSupernova" );

	// jungle inferno
	m_pHealthSlowed = new ImagePanel( this, "PlayerStatusSlowed" );

	m_flNextThink = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerHealth::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
	m_nHealth = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerHealth::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( GetResFilename() );

	if ( m_pHealthBonusImage )
	{
		m_pHealthBonusImage->GetBounds( m_nBonusHealthOrigX, m_nBonusHealthOrigY, m_nBonusHealthOrigW, m_nBonusHealthOrigH );
	}

	m_flNextThink = 0.0f;

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerHealth::SetHealth( int iNewHealth, int iMaxHealth, int	iMaxBuffedHealth )
{
	int nPrevHealth = m_nHealth;

	// set our health
	m_nHealth = iNewHealth;
	m_nMaxHealth = iMaxHealth;
	m_pHealthImage->SetHealth( (float)(m_nHealth) / (float)(m_nMaxHealth) );

	if ( m_pHealthImage )
	{
		m_pHealthImage->SetFgColor( Color( 255, 255, 255, 255 ) );
	}

	if ( m_nHealth <= 0 )
	{
		if ( m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( false );
		}

		if ( m_pHealthImageBuildingBG->IsVisible() )
		{
			m_pHealthImageBuildingBG->SetVisible( false );
		}

		HideHealthBonusImage();
	}
	else
	{
		if ( !m_pHealthImageBG->IsVisible() )
		{
			m_pHealthImageBG->SetVisible( true );
		}

		CTargetID *pTargetID = dynamic_cast<CTargetID *>( this->GetParent() );
		if ( NULL != pTargetID )
		{
			if ( cl_entitylist->GetEnt( pTargetID->GetTargetIndex() )->IsBaseObject() )
				m_pHealthImageBuildingBG->SetVisible( true );
			else
				m_pHealthImageBuildingBG->SetVisible( false );
		}

		C_TFPlayer *pPlayer = ToTFPlayer( C_BasePlayer::GetLocalPlayer() );
		if ( pPlayer )
		{
			if ( pPlayer->m_Shared.InCond( TF_COND_BLEEDING ) )
			{
				if ( !m_pHealthBleedImage->IsVisible() )
					m_pHealthBleedImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthBleedImage->IsVisible() )
					m_pHealthBleedImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_GRAPPLINGHOOK_BLEEDING ) )
			{
				if ( !m_pHealthHookBleedImage->IsVisible() )
					m_pHealthHookBleedImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthHookBleedImage->IsVisible() )
					m_pHealthHookBleedImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MAD_MILK ) )
			{
				if ( !m_pHealthMilkImage->IsVisible() )
					m_pHealthMilkImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMilkImage->IsVisible() )
					m_pHealthMilkImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_GAS ) )
			{
				if ( !m_pHealthGasImage->IsVisible() )
					m_pHealthGasImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthGasImage->IsVisible() )
					m_pHealthGasImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MARKEDFORDEATH ) )
			{
				if ( !m_pHealthMiniCritImage->IsVisible() )
					m_pHealthMiniCritImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMiniCritImage->IsVisible() )
					m_pHealthMiniCritImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MARKEDFORDEATH_SILENT ) )
			{
				if ( !m_pHealthMiniCritSlientImage->IsVisible() )
					m_pHealthMiniCritSlientImage->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMiniCritSlientImage->IsVisible() )
					m_pHealthMiniCritSlientImage->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_UBER_BULLET_RESIST ) )
			{
				if ( !m_pHealthMedicBulletResist->IsVisible() )
					m_pHealthMedicBulletResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicBulletResist->IsVisible() )
					m_pHealthMedicBulletResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_UBER_BLAST_RESIST ) )
			{
				if ( !m_pHealthMedicBlastResist->IsVisible() )
					m_pHealthMedicBlastResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicBlastResist->IsVisible() )
					m_pHealthMedicBlastResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_UBER_FIRE_RESIST ) )
			{
				if ( !m_pHealthMedicFireResist->IsVisible() )
					m_pHealthMedicFireResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicFireResist->IsVisible() )
					m_pHealthMedicFireResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_SMALL_BULLET_RESIST ) )
			{
				if ( !m_pHealthMedicSmallBulletResist->IsVisible() )
					m_pHealthMedicSmallBulletResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicSmallBulletResist->IsVisible() )
					m_pHealthMedicSmallBulletResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_SMALL_BLAST_RESIST ) )
			{
				if ( !m_pHealthMedicSmallBlastResist->IsVisible() )
					m_pHealthMedicSmallBlastResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicSmallBlastResist->IsVisible() )
					m_pHealthMedicSmallBlastResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_MEDIGUN_SMALL_FIRE_RESIST ) )
			{
				if ( !m_pHealthMedicSmallFireResist->IsVisible() )
					m_pHealthMedicSmallFireResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthMedicSmallFireResist->IsVisible() )
					m_pHealthMedicSmallFireResist->SetVisible( false );
			}

			// skip this one entirely
			if ( m_pHealthWheelOfDoom->IsVisible() )
			{
				m_pHealthWheelOfDoom->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_OFFENSEBUFF ) )
			{
				if ( !m_pHealthSoldierOffenseBuff->IsVisible() )
					m_pHealthSoldierOffenseBuff->SetVisible( true );
			}
			else
			{
				if ( m_pHealthSoldierOffenseBuff->IsVisible() )
					m_pHealthSoldierOffenseBuff->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_DEFENSEBUFF ) )
			{
				if ( !m_pHealthSoldierDefenseBuff->IsVisible() )
					m_pHealthSoldierDefenseBuff->SetVisible( true );
			}
			else
			{
				if ( m_pHealthSoldierDefenseBuff->IsVisible() )
					m_pHealthSoldierDefenseBuff->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_REGENONDAMAGEBUFF ) )
			{
				if ( !m_pHealthSoldierHealOnHitBuff->IsVisible() )
					m_pHealthSoldierHealOnHitBuff->SetVisible( true );
			}
			else
			{
				if ( m_pHealthSoldierHealOnHitBuff->IsVisible() )
					m_pHealthSoldierHealOnHitBuff->SetVisible( false );
			}

			// skip this one entirely because i don't know what it is -puddy
			if ( m_pHealthSpyMarked->IsVisible() )
			{
				m_pHealthSpyMarked->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_PARACHUTE_ACTIVE ) || pPlayer->m_Shared.InCond( TF_COND_PARACHUTE_DEPLOYED ) )
			{
				if ( !m_pHealthParachute->IsVisible() )
					m_pHealthParachute->SetVisible( true );
			}
			else
			{
				if ( m_pHealthParachute->IsVisible() )
					m_pHealthParachute->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_STRENGTH ) )
			{
				if ( !m_pHealthRuneStrength->IsVisible() )
					m_pHealthRuneStrength->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneStrength->IsVisible() )
					m_pHealthRuneStrength->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_HASTE ) )
			{
				if ( !m_pHealthRuneHaste->IsVisible() )
					m_pHealthRuneHaste->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneHaste->IsVisible() )
					m_pHealthRuneHaste->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_REGEN ) )
			{
				if ( !m_pHealthRuneRegen->IsVisible() )
					m_pHealthRuneRegen->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneRegen->IsVisible() )
					m_pHealthRuneRegen->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_RESIST ) )
			{
				if ( !m_pHealthRuneResist->IsVisible() )
					m_pHealthRuneResist->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneResist->IsVisible() )
					m_pHealthRuneResist->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_VAMPIRE ) )
			{
				if ( !m_pHealthRuneVampire->IsVisible() )
					m_pHealthRuneVampire->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneVampire->IsVisible() )
					m_pHealthRuneVampire->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_WARLOCK ) )
			{
				if ( !m_pHealthRuneReflect->IsVisible() )
					m_pHealthRuneReflect->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneReflect->IsVisible() )
					m_pHealthRuneReflect->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_PRECISION ) )
			{
				if ( !m_pHealthRunePrecision->IsVisible() )
					m_pHealthRuneReflect->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRunePrecision->IsVisible() )
					m_pHealthRunePrecision->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_AGILITY ) )
			{
				if ( !m_pHealthRuneAgility->IsVisible() )
					m_pHealthRuneAgility->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneAgility->IsVisible() )
					m_pHealthRuneAgility->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_KNOCKOUT ) )
			{
				if ( !m_pHealthRuneKnockout->IsVisible() )
					m_pHealthRuneKnockout->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneKnockout->IsVisible() )
					m_pHealthRuneKnockout->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_KING ) )
			{
				if ( !m_pHealthRuneKing->IsVisible() )
					m_pHealthRuneKing->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneKing->IsVisible() )
					m_pHealthRuneKing->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_PLAGUE ) )
			{
				if ( !m_pHealthRunePlague->IsVisible() )
					m_pHealthRunePlague->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRunePlague->IsVisible() )
					m_pHealthRunePlague->SetVisible( false );
			}

			if ( pPlayer->m_Shared.InCond( TF_COND_RUNE_SUPERNOVA ) )
			{
				if ( !m_pHealthRuneSupernova->IsVisible() )
					m_pHealthRuneSupernova->SetVisible( true );
			}
			else
			{
				if ( m_pHealthRuneSupernova->IsVisible() )
					m_pHealthRuneSupernova->SetVisible( false );
			}

			// not sure if this the correct cond
			if ( pPlayer->m_Shared.InCond( TF_COND_STUNNED ) || pPlayer->m_Shared.InCond( TF_COND_SLOWED ) )
			{
				if ( !m_pHealthSlowed->IsVisible() )
					m_pHealthSlowed->SetVisible( true );
			}
			else
			{
				if ( m_pHealthSlowed->IsVisible() )
					m_pHealthSlowed->SetVisible( false );
			}
		}

		// are we getting a health bonus?
		if ( m_nHealth > m_nMaxHealth )
		{
			if ( m_pHealthBonusImage )
			{
				if ( !m_pHealthBonusImage->IsVisible() )
				{
					m_pHealthBonusImage->SetVisible( true );
					g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( this, "HudHealthBonusPulse" );
				}

				m_pHealthBonusImage->SetDrawColor( Color( 255, 255, 255, 255 ) );

				// scale the flashing image based on how much health bonus we currently have
				float flBoostMaxAmount = ( iMaxBuffedHealth ) - m_nMaxHealth;
				float flPercent = min( 1.0 , ( m_nHealth - m_nMaxHealth ) / flBoostMaxAmount ); // clamped to 1 to not cut off for values above 150%

				int nPosAdj = RoundFloatToInt( flPercent * m_nHealthBonusPosAdj );
				int nSizeAdj = 2 * nPosAdj;

				m_pHealthBonusImage->SetBounds( m_nBonusHealthOrigX - nPosAdj, 
					m_nBonusHealthOrigY - nPosAdj, 
					m_nBonusHealthOrigW + nSizeAdj,
					m_nBonusHealthOrigH + nSizeAdj );
			}
		}
		// are we close to dying?
		else if ( m_nHealth < m_nMaxHealth * m_flHealthDeathWarning )
		{
			if ( m_pHealthBonusImage )
			{
				if ( !m_pHealthBonusImage->IsVisible() )
				{
					m_pHealthBonusImage->SetVisible( true );
					g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( this, "HudHealthDyingPulse" );
				}

				m_pHealthBonusImage->SetDrawColor( m_clrHealthDeathWarningColor );

				// scale the flashing image based on how much health bonus we currently have
				float flBoostMaxAmount = m_nMaxHealth * m_flHealthDeathWarning;
				float flPercent = ( flBoostMaxAmount - m_nHealth ) / flBoostMaxAmount;

				int nPosAdj = RoundFloatToInt( flPercent * m_nHealthBonusPosAdj );
				int nSizeAdj = 2 * nPosAdj;

				m_pHealthBonusImage->SetBounds( m_nBonusHealthOrigX - nPosAdj, 
					m_nBonusHealthOrigY - nPosAdj, 
					m_nBonusHealthOrigW + nSizeAdj,
					m_nBonusHealthOrigH + nSizeAdj );
			}

			if ( m_pHealthImage )
			{
				m_pHealthImage->SetFgColor( m_clrHealthDeathWarningColor );
			}
		}
		// turn it off
		else
		{
			HideHealthBonusImage();
		}
	}

	// set our health display value
	if ( nPrevHealth != m_nHealth )
	{
		if ( m_nHealth > 0 )
		{
			SetDialogVariable( "Health", m_nHealth );
		}
		else
		{
			SetDialogVariable( "Health", "" );
		}

		// are we lower than max?
		if ( m_nHealth < m_nMaxHealth )
		{
			SetDialogVariable( "MaxHealth", m_nMaxHealth );
		}
		else
		{
			SetDialogVariable( "MaxHealth", "" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerHealth::HideHealthBonusImage( void )
{
	if ( m_pHealthBonusImage && m_pHealthBonusImage->IsVisible() )
	{
		m_pHealthBonusImage->SetBounds( m_nBonusHealthOrigX, m_nBonusHealthOrigY, m_nBonusHealthOrigW, m_nBonusHealthOrigH );
		m_pHealthBonusImage->SetVisible( false );
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( this, "HudHealthBonusPulseStop" );
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( this, "HudHealthDyingPulseStop" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerHealth::OnThink()
{
	if ( m_flNextThink < gpGlobals->curtime )
	{
		C_TFPlayer *pPlayer = ToTFPlayer( C_BasePlayer::GetLocalPlayer() );

		if ( pPlayer )
		{
			SetHealth( pPlayer->GetHealth(), pPlayer->GetPlayerClass()->GetMaxHealth(), pPlayer->m_Shared.GetMaxBuffedHealth() );
		}

		m_flNextThink = gpGlobals->curtime + 0.05f;
	}
}


DECLARE_HUDELEMENT( CTFHudPlayerStatus );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudPlayerStatus::CTFHudPlayerStatus( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudPlayerStatus" ) 
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pHudPlayerClass = new CTFHudPlayerClass( this, "HudPlayerClass" );
	m_pHudPlayerHealth = new CTFHudPlayerHealth( this, "HudPlayerHealth" );

	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerStatus::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// HACK: Work around the scheme application order failing
	// to reload the player class hud element's scheme in minmode.
	ConVarRef cl_hud_minmode( "cl_hud_minmode", true );
	if ( cl_hud_minmode.IsValid() && cl_hud_minmode.GetBool() )
	{
		m_pHudPlayerClass->InvalidateLayout( false, true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudPlayerStatus::Reset()
{
	if ( m_pHudPlayerClass )
	{
		m_pHudPlayerClass->Reset();
	}

	if ( m_pHudPlayerHealth )
	{
		m_pHudPlayerHealth->Reset();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFClassImage::SetClass( int iTeam, int iClass, int iCloakstate )
{
	char szImage[128];
	szImage[0] = '\0';

	switch (iTeam)
	{
		case TF_TEAM_RED:
			Q_strncpy(szImage, g_szRedClassImages[iClass], sizeof(szImage));
			break;
		case TF_TEAM_BLUE:
			Q_strncpy(szImage, g_szBlueClassImages[iClass], sizeof(szImage));
			break;
	}

	switch( iCloakstate )
	{
	case 2:
		Q_strncat( szImage, "_cloak", sizeof(szImage), COPY_ALL_CHARACTERS );
		break;
	case 1:
		Q_strncat( szImage, "_halfcloak", sizeof(szImage), COPY_ALL_CHARACTERS );
		break;
	default:
		break;
	}

	if ( Q_strlen( szImage ) > 0 )
	{
		SetImage( szImage );
	}
}
