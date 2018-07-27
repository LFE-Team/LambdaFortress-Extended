//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_tf_viewmodeladdon.h"
#include "c_tf_player.h"
#include "tf_viewmodel.h"
#include "model_types.h"
#include "tf_weaponbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar r_drawothermodels;

bool C_ViewmodelAttachmentModel::InitializeAsClientEntity( const char *pszModelName, RenderGroup_t renderGroup )
{
	if ( BaseClass::InitializeAsClientEntity( pszModelName, renderGroup ) )
	{
		AddEffects( EF_BONEMERGE | EF_BONEMERGE_FASTCULL );
		return true;
	}

	return false;
}

void C_ViewmodelAttachmentModel::SetViewmodel( C_TFViewModel *vm )
{
	m_viewmodel.Set( vm );
}

int C_ViewmodelAttachmentModel::InternalDrawModel( int flags )
{
	CMatRenderContextPtr pRenderContext( materials );

	if ( m_viewmodel->ShouldFlipViewModel() )
		pRenderContext->CullMode( MATERIAL_CULLMODE_CW );

	int ret = BaseClass::InternalDrawModel( flags );

	pRenderContext->CullMode( MATERIAL_CULLMODE_CCW );

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: We're overriding this because DrawModel keeps calling the FollowEntity DrawModel function
//			which in this case is CTFViewModel::DrawModel.
//			This is basically just a straight copy of C_BaseAnimating::DrawModel, without the FollowEntity part
//-----------------------------------------------------------------------------
int C_ViewmodelAttachmentModel::DrawOverriddenViewmodel( int flags )
{
	if ( !m_bReadyToDraw )
		return 0;

	int drawn = 0;

	ValidateModelIndex();

	if ( r_drawothermodels.GetInt() )
	{
		MDLCACHE_CRITICAL_SECTION();

		int extraFlags = 0;
		if ( r_drawothermodels.GetInt() == 2 )
		{
			extraFlags |= STUDIO_WIREFRAME;
		}

		if ( flags & STUDIO_SHADOWDEPTHTEXTURE )
		{
			extraFlags |= STUDIO_SHADOWDEPTHTEXTURE;
		}

		if ( flags & STUDIO_SSAODEPTHTEXTURE )
		{
			extraFlags |= STUDIO_SSAODEPTHTEXTURE;
		}

		// Necessary for lighting blending
		CreateModelInstance();

		drawn = InternalDrawModel( flags | extraFlags );
	}

	// If we're visualizing our bboxes, draw them
	DrawBBoxVisualizations();

	return drawn;
}

int C_ViewmodelAttachmentModel::DrawModel( int flags )
{
	if ( !IsVisible() )
		return 0;

	if ( m_viewmodel.Get() == NULL )
		return 0;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	C_TFPlayer *pPlayer = ToTFPlayer( m_viewmodel.Get()->GetOwner() );

	if ( pLocalPlayer && pLocalPlayer->IsObserver()
		&& pLocalPlayer->GetObserverTarget() != m_viewmodel.Get()->GetOwner() )
		return false;

	if ( pLocalPlayer && !pLocalPlayer->IsObserver() && ( pLocalPlayer != pPlayer ) )
		return false;
	
	//UpdateExtraWearables();

	return BaseClass::DrawModel( flags );
}

//-----------------------------------------------------------------------------
// Purpose: i don't even know if this is the right place to put.
//-----------------------------------------------------------------------------
void C_ViewmodelAttachmentModel::UpdateExtraWearables( void )
{
	// Get the player and active weapon.
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer )
		return;

	C_TFWeaponBase *pWeapon = pPlayer->GetActiveTFWeapon();

	C_TFWeaponAttachmentModel *pAttachedModel = m_hAttachmentModel.Get();

	if ( pWeapon )
	{
		CEconItemView *pItemView = pWeapon->GetItem();
		CEconItemDefinition *pItemDef = pItemView->GetStaticData();

		if ( pItemDef )
		{
			if ( pItemView->GetStaticData() && ( !pItemView->GetStaticData()->lfe_attached_models ) )
			{
				if ( pAttachedModel )
				{
					if ( pAttachedModel->GetModelIndex() == modelinfo->GetModelIndex( pItemView->GetAttachedDisplayModel() ) )
					{
						pAttachedModel->m_nSkin = GetSkin();
	
						if ( pPlayer != GetOwner() ) // Spectator fix
						{
							pAttachedModel->FollowEntity( this );
							pAttachedModel->m_nRenderFX = m_nRenderFX;
							pAttachedModel->UpdateVisibility();
						}
						return; // we already have the correct add-on
					}
					else
					{
						if ( m_hAttachmentModel.Get() )
						{
							m_hAttachmentModel->SetModel( "" );
							m_hAttachmentModel->Remove();
						}
					}
				}

				pAttachedModel = new C_TFWeaponAttachmentModel();

				if ( pAttachedModel->InitializeAsClientEntity( pItemView->GetAttachedDisplayModel(), RENDER_GROUP_VIEW_MODEL_TRANSLUCENT ) == false )
				{
					pAttachedModel->Release();
					return;
				}

				m_hAttachmentModel = pAttachedModel;

				pAttachedModel->m_nSkin = GetSkin();
				//pAttachedModel->SetOwner( GetOwner() );
				pAttachedModel->FollowEntity( this );
				pAttachedModel->UpdateVisibility();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_ViewmodelAttachmentModel::StandardBlendingRules( CStudioHdr *hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask )
{
	BaseClass::StandardBlendingRules( hdr, pos, q, currentTime, boneMask );

	CTFWeaponBase *pWeapon = ( CTFWeaponBase * )m_viewmodel->GetOwningWeapon();
	if ( !pWeapon )
		return;

	if ( m_viewmodel->GetViewModelType() == VMTYPE_TF2 )
	{
		pWeapon->m_iMuzzleAttachment = LookupAttachment( "muzzle" );
		pWeapon->m_iBrassAttachment = LookupAttachment( "eject_brass" );
	}

	pWeapon->ViewModelAttachmentBlending( hdr, pos, q, currentTime, boneMask );
}