#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "InputComponentData.h"
#include "MovementComponentData.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SoundComponentData.h"
#include "SawBladeComponentData.h"
#include "SteamComponentData.h"
#include "PlayerGraphicsComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	InputComponentData myInputData;
	MovementComponentData myMovementData;
	PhysicsComponentData myPhysicsData;
	TriggerComponentData myTriggerData;
	SoundComponentData mySoundData;
	SawBladeComponentData mySawBladeData;
	PlayerGraphicsComponentData myPlayerGraphicsData;
	SteamComponentData mySteamData;
	std::string mySubType;
};