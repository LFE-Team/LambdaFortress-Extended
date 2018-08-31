#include "cbase.h"

class CLFEVersus : public CLogicalEntity
{
public:
	DECLARE_CLASS( CLFEVersus, CLogicalEntity );
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( lfe_logic_versus, CLFEVersus );

BEGIN_DATADESC( CLFEVersus )
 
END_DATADESC()