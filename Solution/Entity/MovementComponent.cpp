#include "stdafx.h"

#include <AudioInterface.h>
#include "CharacterAnimationNote.h"
#include "DashAimMovement.h"
#include "DashFlyMovement.h"
#include "FlyMovement.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include "PostMaster.h"
#include "PlayerActiveMessage.h"
#include "SoundComponent.h"
#include "WalkMovement.h"
#include <PostMaster.h>
#include <EmitterMessage.h>
#include "PlayerGraphicsComponent.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aData)
	, myCurrentMovement(eMovementType::FLY)
	, myDashCooldown(0.f)
	, myDeltaTime(0.f)
	, myCollisionTimer(0.f)
	, myVelocities(8)
	, myVelocityIndex(0)
	, myAverageVelocityTimer(0.5f)
	, myShouldCollide(true)
{
	myMovements[eMovementType::FLY] = new FlyMovement(aData, anOrientation, *this);
	myMovements[eMovementType::WALK] = new WalkMovement(aData, anOrientation, *this);
	myMovements[eMovementType::DASH_AIM] = new DashAimMovement(aData, anOrientation, *this, aScene);
	myMovements[eMovementType::DASH_FLY] = new DashFlyMovement(aData, anOrientation, *this);
	myMovements[myCurrentMovement]->Activate({ 0.f, 0.f });

	for (int i = 0; i < 10.f; ++i)
	{
		myVelocities.Add(CU::Vector2<float>());
	}

	PostMaster::GetInstance()->Subscribe(this, eMessageType::PLAYER_ACTIVE);
}

MovementComponent::~MovementComponent()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		SAFE_DELETE(myMovements[i]);
	}

	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void MovementComponent::Init()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Init();
	}
}

void MovementComponent::Reset()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Reset();
	}
	myDashCooldown = 0.f;
	myVelocity = CU::Vector2<float>();
	myShouldCollide = false;
}

void MovementComponent::Update(float aDeltaTime)
{
	DEBUG_PRINT(myEntity.GetOrientation().GetPos());
	DEBUG_PRINT(CU::Length(GetVelocity()));

	myCollisionTimer -= aDeltaTime;
	myDeltaTime = aDeltaTime;
	myDashCooldown -= aDeltaTime;
	myAverageVelocityTimer -= aDeltaTime;

	myShouldCollide = myCollisionTimer <= 0.f;
	myMovements[myCurrentMovement]->Update(aDeltaTime, myShouldCollide);

	if (myIsInSteam == true)
	{
		float length = CU::Length(myEntity.GetOrientation().GetPos() - mySteamOrigin) - GC::PlayerRadius;
		float dist = fmax(1.f - (length / mySteamLength), 0.f);
		dist *= 1.f - myData.mySteamMinForce;
		dist += myData.mySteamMinForce;

		myMovements[myCurrentMovement]->Impulse(mySteamVelocity * aDeltaTime * dist);
	}

	myVelocity = myMovements[myCurrentMovement]->GetVelocity();

	if (myAverageVelocityTimer < 0.f)
	{
		myAverageVelocityTimer = 0.05f;
		myVelocities[myVelocityIndex] = myVelocity;
		++myVelocityIndex;
		if (myVelocityIndex >= myVelocities.Size())
		{
			myVelocityIndex = 0;
		}

		myAverageVelocity = CU::Vector2<float>();
		for each (const CU::Vector2<float>& velocity in myVelocities)
		{
			myAverageVelocity += velocity;
		}
		myAverageVelocity /= static_cast<float>(myVelocities.Size());
	}
}

void MovementComponent::Render()
{
	myMovements[myCurrentMovement]->Render();
}

void MovementComponent::ReceiveNote(const ContactNote& aNote)
{
	myMovements[myCurrentMovement]->ReceiveNote(aNote);
}

void MovementComponent::Impulse()
{
	myMovements[myCurrentMovement]->Impulse();
	if (myCurrentMovement == eMovementType::FLY
		|| myCurrentMovement == eMovementType::WALK)
	{
		SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
		if (soundComp != nullptr)
		{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Impulse", soundComp->GetAudioSFXID());
				if (myEntity.GetComponent<PlayerGraphicsComponent>()->GetHeadActive() == true)
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ImpulseVoice", soundComp->GetAudioSFXID());
				}
		}
	}
}

void MovementComponent::Impulse(const CU::Vector2<float>& aVelocity)
{
	myMovements[myCurrentMovement]->Impulse(aVelocity);
}

void MovementComponent::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myMovements[myCurrentMovement]->SetDirectionTarget(aDirection);
}

void MovementComponent::RightTriggerDown()
{
	if (myDashCooldown <= 0.f && myShouldCollide == true)
	{
		SetState(eMovementType::DASH_AIM, { 0.f, 0.f });
	}
}

void MovementComponent::RightTriggerUp()
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		SetState(eMovementType::DASH_FLY, { 0.f, 0.f });
	}
}

void MovementComponent::SetState(eMovementType aState, const CU::Vector2<float>& aVelocity)
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		myDashCooldown = myData.myDashCooldown;
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Dash_Recharge", &myEntity, myDashCooldown));
	}
	myMovements[myCurrentMovement]->DeActivate();

	myCurrentMovement = aState;
	myMovements[myCurrentMovement]->Activate(aVelocity);

	switch (myCurrentMovement)
	{
	case MovementComponent::FLY:
		myEntity.SendNote(CharacterAnimationNote(eCharacterAnimationType::FLY));
		break;
	case MovementComponent::WALK:
		myEntity.SendNote(CharacterAnimationNote(eCharacterAnimationType::IDLE));
		break;
	case MovementComponent::DASH_AIM:
		myEntity.SendNote(CharacterAnimationNote(eCharacterAnimationType::DASH_AIM));
		break;
	case MovementComponent::DASH_FLY:
		myEntity.SendNote(CharacterAnimationNote(eCharacterAnimationType::DASH_FLY));
		break;
	}
}

void MovementComponent::SetInSteam(bool aIsInSteam, float aForce, float aSteamLength
	, const CU::Vector2<float>& aDirection, const CU::Vector3<float>& anOrigion)
{
	if (myIsInSteam == true && aIsInSteam == true)
	{
		mySteamVelocity.x += (aDirection.x * aForce) * aDirection.x;
		mySteamVelocity.y += (aDirection.y * aForce) * aDirection.y;
	}
	else
	{
		mySteamVelocity = aForce * aDirection;
	}

	myIsInSteam = aIsInSteam;
	mySteamOrigin = anOrigion;
	mySteamLength = aSteamLength;

	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->SetIsInSteam(aIsInSteam);
	}
}

void MovementComponent::SetVelocity(const CU::Vector2<float>& aVelocity)
{
	myMovements[myCurrentMovement]->SetVelocity(aVelocity);
	myVelocity = aVelocity;
}

const CU::Vector2<float>& MovementComponent::GetVelocity() const
{
	return myVelocity;
}

void MovementComponent::ReceiveNote(const DeathNote&)
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->DeActivate();
	}

	for (int i = 0; i < myVelocities.Size(); ++i)
	{
		myVelocities[i] = CU::Vector2<float>();
	}
	myAverageVelocity = CU::Vector2<float>();
}

void MovementComponent::ReceiveNote(const SpawnNote&)
{
	myAverageVelocityTimer = 0.5f;
	myCollisionTimer = 0.4f;
	myCurrentMovement = eMovementType::FLY;
	//mySpawnVelocity = { 0.05f, 0.01f };
	myMovements[myCurrentMovement]->Activate(mySpawnVelocity);
	//myEntity.ResetPosition();

	if (mySpawnVelocity.x < 0)
	{
		myEntity.GetComponent<InputComponent>()->SetIsFlipped(true);
	}
}