//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "model_types.h"
#include "c_point_camera.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_FuncMonitor : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_FuncMonitor, C_BaseEntity );
	DECLARE_CLIENTCLASS();

// C_BaseEntity.
public:
	virtual bool	ShouldDraw();
	virtual int		DrawModel(int flags);
	virtual int		DrawBrushModel(bool bTranslucent, int nFlags, bool bTwoPass);

private:
	CNetworkHandle(C_BaseEntity, m_hInfoCameraLink);
};

IMPLEMENT_CLIENTCLASS_DT( C_FuncMonitor, DT_FuncMonitor, CFuncMonitor )
RecvPropEHandle(RECVINFO(m_hInfoCameraLink)),
END_RECV_TABLE()

bool C_FuncMonitor::ShouldDraw()
{
	return true;
}

int C_FuncMonitor::DrawBrushModel(bool bTranslucent, int nFlags, bool bTwoPass)
{
	int ret = BaseClass::DrawBrushModel(bTranslucent, nFlags, bTwoPass);
	return ret;
}

int C_FuncMonitor::DrawModel(int flags)
{
	int ret = BaseClass::DrawModel(flags);
	return ret;
}
