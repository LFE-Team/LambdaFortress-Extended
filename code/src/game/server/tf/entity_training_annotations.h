//===================== 2018 ISPuddy reversed engineering =====================//
// Reverse Engie Training Annotations
// An in-world location-specific information bubble.
//=============================================================================//
#ifndef ENTITY_TRAINING_ANNOTATIONS_H
#define ENTITY_TRAINING_ANNOTATIONS_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

//=============================================================================

class CTrainingAnnotation : public CPointEntity
{
public:
	DECLARE_CLASS( CTrainingAnnotation, CPointEntity );
	DECLARE_DATADESC();

	CTrainingAnnotation();

	void	Spawn( void );
	void	Precache( void );

	void InputShow( inputdata_t &inputdata );
	void Show( CBaseEntity *pActivator );
	void InputHide( inputdata_t &inputdata );
	void Hide( CBaseEntity *pActivator );
private:

	string_t m_displayText;
	float m_flLifetime;
	float m_flVerticalOffset;
};

#endif // ENTITY_TRAINING_ANNOTATIONS_H