#include "stdafx.h"

#include "AnimationComponent.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include "PhysicsComponent.h"
#include "PollingStation.h"
#include <Scene.h>
#include <Instance.h>
#include <EmitterMessage.h>
#include "PlayerComponent.h"
#include "PlayerGraphicsComponent.h"
#include <PostMaster.h>
#include "SawBladeComponent.h"
#include "ScoreComponent.h"
#include "SteamComponent.h"
#include "SoundComponent.h"
#include "TriggerComponent.h"

Entity::Entity(const EntityData& aEntityData, Prism::Scene* aScene, const CU::Vector3<float>& aStartPosition,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale, const std::string& aSubType)
	: myScene(aScene)
	, myEntityData(aEntityData)
	, myEmitterConnection(nullptr)
	, mySubType(aSubType)
	, myIsEnemy(false)
	, myIsActive(true)
	, myTimeActiveBeforeKill(10.f)
	, myTimeActiveBeforeKillTimer(10.f)
	, myDelayAddToSceneTimer(0.f)
	, myDelayedAddToScene(false)
	, myStartPosition(aStartPosition)
	, myStartRotation(aRotation)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(myStartPosition);
	SetRotation(myStartRotation);


	if (aEntityData.myTriggerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::TRIGGER)] = new TriggerComponent(*this, aEntityData.myTriggerData);
	}

	if (aEntityData.myAnimationData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::ANIMATION)] = new AnimationComponent(*this, aEntityData.myAnimationData, aScene);
	}
	else if (aEntityData.myGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
		GetComponent<GraphicsComponent>()->SetScale(aScale);
	}
	else if (aEntityData.myPlayerGraphicsData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::PLAYER_GRAPHICS)] = new PlayerGraphicsComponent(*this, aEntityData.myPlayerGraphicsData, myOrientation, aScene);
	}

	if (aEntityData.mySoundData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SOUND)] = new SoundComponent(*this);
	}

	if (aEntityData.myPhysicsData.myExistsInEntity == true)
	{
		if (aEntityData.myAnimationData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, aEntityData.myAnimationData.myModelPath);
		}
		else if (aEntityData.myGraphicsData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, aEntityData.myGraphicsData.myModelPath);
		}
		else
		{
			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, "no path");
		}
	}

	if (aEntityData.myInputData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::INPUT)] = new InputComponent(*this, aEntityData.myInputData, myOrientation);
	}

	if (aEntityData.myMovementData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::MOVEMENT)] = new MovementComponent(*this, aEntityData.myMovementData, myOrientation, aScene);
	}

	if (aEntityData.mySawBladeData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SAW_BLADE)] = new SawBladeComponent(*this);
	}

	if (aEntityData.mySteamData.myExistsInEntity == true) // has to be after physics
	{
		myComponents[static_cast<int>(eComponentType::STEAM)] = new SteamComponent(*this, aScene, aRotation);
	}

	if (aEntityData.myScoreData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SCORE)] = new ScoreComponent(*this);
	}

	if (aEntityData.myPlayerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::PLAYER)] = new PlayerComponent(*this);
	}

	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Init();
		}
	}

	Reset();

};

Entity::~Entity()
{
	if (GetComponent<PhysicsComponent>() != nullptr)
	{
		SET_RUNTIME(false);
		GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		delete myComponents[i];
		myComponents[i] = nullptr;
	}
}

void Entity::Reset()
{
	myAlive = true;

	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
	
	myDelayAddToSceneTimer = 0.f;
	myDelayedAddToScene = false;

	ResetPosition();
}

void Entity::Update(float aDeltaTime)
{
	for each (Component* component in myComponents)
	{
		if (component != nullptr)
		{
			component->Update(aDeltaTime);
		}
	}

	if (myIsActive == false)
	{
		myTimeActiveBeforeKillTimer -= aDeltaTime;

		if (myTimeActiveBeforeKillTimer <= 0.f)
		{
			myTimeActiveBeforeKillTimer = myTimeActiveBeforeKill;
			myIsActive = true;
			Kill(false);
		}
	}

	if (myEntityData.myPhysicsData.myPhysicsType == ePhysics::DYNAMIC)
	{
		memcpy(&myOrientation.myMatrix[0], GetComponent<PhysicsComponent>()->GetOrientation(), sizeof(float) * 16);
	}

	if (myDelayedAddToScene == true)
	{
		myDelayAddToSceneTimer -= aDeltaTime;
		if (myDelayAddToSceneTimer <= 0.f)
		{
			myDelayedAddToScene = false;
			AddToScene();
		}
	}
}

void Entity::AddComponent(Component* aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent->GetType())] == nullptr, "Tried to add component several times");
	myComponents[int(aComponent->GetType())] = aComponent;
}

void Entity::RemoveComponent(eComponentType aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent)] != nullptr, "Tried to remove an nonexisting component");
	delete myComponents[int(aComponent)];
	myComponents[int(aComponent)] = nullptr;
}

void Entity::AddToScene()
{
	DL_ASSERT_EXP(myIsInScene == false, "Tried to add Entity to scene twice");

	if (myIsInScene == true)
	{
		return;
	}

	if (GetComponent<GraphicsComponent>() != nullptr && GetComponent<GraphicsComponent>()->GetInstance() != nullptr)
	{
		myScene->AddInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr && GetComponent<AnimationComponent>()->GetInstance() != nullptr)
	{
		myScene->AddInstance(GetComponent<AnimationComponent>()->GetInstance());
	}

	myIsInScene = true;
}

void Entity::RemoveFromScene()
{
	DL_ASSERT_EXP(myIsInScene == true, "Tried to remove Entity not in scene");

	if (myIsInScene == false)
	{
		return;
	}

	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		myScene->RemoveInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr)
	{
		myScene->RemoveInstance(GetComponent<AnimationComponent>()->GetInstance());
	}

	myIsInScene = false;
}

eEntityType Entity::GetType() const
{
	return myEntityData.myType;
}

void Entity::AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection)
{
	myEmitterConnection = anEmitterConnection;
}

Prism::ParticleEmitterInstance* Entity::GetEmitter()
{
	if (myEmitterConnection == nullptr)
		return nullptr;
	return myEmitterConnection;
}

void Entity::Kill(bool aRemoveFromPhysics)
{
	myAlive = false;

	if (myIsInScene == true)
	{
		RemoveFromScene();
		myIsInScene = false;
	}
	
	if (aRemoveFromPhysics == true && myEntityData.myPhysicsData.myExistsInEntity == true)
	{
		GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
}