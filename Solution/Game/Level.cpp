#include "stdafx.h"

#include "Level.h"
#include <Scene.h>
#include <EntityFactory.h>

#include <PhysicsInterface.h>
Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
{
	Prism::PhysicsInterface::Create(std::bind(&Level::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		, std::bind(&Level::ContactCallback, this, std::placeholders::_1, std::placeholders::_2));

	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);

	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");
	myPlayer = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, CU::Vector3<float>());
	myPlayer->AddToScene();


}

Level::~Level()
{
	SAFE_DELETE(myScene);
	SAFE_DELETE(myPlayer);
	myEntities.DeleteAll();

#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif
	Prism::PhysicsInterface::Destroy();
}

void Level::Update(float aDelta)
{
	myPlayer->Update(aDelta);
}

void Level::Render()
{
	myScene->Render();
}

void Level::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	/*Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	switch (first.GetType())
	{
	case eEntityType::TRIGGER:
		HandleTrigger(first, second, aHasEntered);
		break;
	case eEntityType::EXPLOSION:
		if (aHasEntered == true)
		{
			HandleExplosion(first, second);
		}
		break;
	}*/
}

void Level::ContactCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond)
{
	//Entity& first = aFirst->GetEntity();
	//Entity& second = aSecond->GetEntity();

	//if (aFirst->GetPhysicsType() == ePhysics::DYNAMIC)
	//{
	//	if (first.GetComponent<SoundComponent>() != nullptr)
	//	{
	//		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PropBounce", first.GetComponent<SoundComponent>()->GetAudioSFXID());
	//	}
	//}
	//else if (aSecond->GetPhysicsType() == ePhysics::DYNAMIC)
	//{
	//	if (second.GetComponent<SoundComponent>() != nullptr)
	//	{
	//		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PropBounce", second.GetComponent<SoundComponent>()->GetAudioSFXID());
	//	}
	//}
	//else
	//{
	//	int whatIsCollidingHere = 15;
	//	whatIsCollidingHere;
	//}
}