//=============================================================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_tf_spymask.h"
#include "c_tf_player.h"

C_TFSpyMask::C_TFSpyMask()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
C_TFSpyMask *C_TFSpyMask::Create( C_BaseEntity *pOwner )
{
	C_TFSpyMask *pMask = new C_TFSpyMask();

	if ( pMask->Initialize( TF_SPY_MASK_MODEL, pOwner ) )
	{
		return pMask;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Show mask to player's teammates.
//-----------------------------------------------------------------------------
bool C_TFSpyMask::ShouldDraw( void )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( pOwner )
	{
		if ( pOwner->IsEnemyPlayer() && pOwner->m_Shared.GetDisguiseClass() != TF_CLASS_SPY )
			return false;
	}

	return BaseClass::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: Show player's disguise class.
//-----------------------------------------------------------------------------
int C_TFSpyMask::GetSkin( void )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	
	if ( pOwner && pOwner->m_Shared.InCond( TF_COND_DISGUISED ) )
	{
		// If this is an enemy spy disguised as a spy show a fake disguise class.
		if ( pOwner->IsEnemyPlayer() && pOwner->m_Shared.GetDisguiseClass() == TF_CLASS_SPY )
		{
			return ( pOwner->m_Shared.GetMaskClass() - 1 );
		}
		else
		{
			return ( pOwner->m_Shared.GetDisguiseClass() - 1 );
		}

	}

	return BaseClass::GetSkin();
}
