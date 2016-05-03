#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SoundComponentData.h"
#include "InputComponentData.h"
#include "SawBladeComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	PhysicsComponentData myPhysicsData;
	TriggerComponentData myTriggerData;
	SoundComponentData mySoundData;
	InputComponentData myInputData;
	SawBladeComponentData mySawBladeData;
	std::string mySubType;
};