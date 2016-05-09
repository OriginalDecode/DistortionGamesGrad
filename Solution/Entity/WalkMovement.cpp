#include "stdafx.h"
#include "ContactNote.h"
#include "WalkMovement.h"
#include "MovementComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>

WalkMovement::WalkMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myHasContact(true)
{
}


WalkMovement::~WalkMovement()
{
}

void WalkMovement::Reset()
{
	myVelocity.x = 0.f;
	myVelocity.y = 0.f;
	myHasContact = true;
}

void WalkMovement::Update(float aDeltaTime)
{
	if (myHasContact == false)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
		return;
	}
	HandleContact();

	//myVelocity.y += myData.myGravity * aDeltaTime;
	myVelocity.y = 0;

	Drag(aDeltaTime);
	Walk(aDeltaTime);

	Translate();

	myPreviousPosition = myOrientation.GetPos();
}

void WalkMovement::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myDirectionTarget = aDirection;
}

void WalkMovement::Impulse()
{
	//myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
	myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
	CU::Vector3<float> offset(myOrientation.GetPos());
	offset.y += 0.1f;
	offset.z = 0.f;
	myOrientation.SetPos(offset);
	myMovementComponent.Impulse();
}

void WalkMovement::Impulse(const CU::Vector2<float>& aVelocity)
{
	myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
	myMovementComponent.Impulse(aVelocity);
}

void WalkMovement::Activate()
{
	myVelocity.x = 0.f;
	myVelocity.y = 0.f;
	myHasContact = true;

	myIsActive = true;
}

void WalkMovement::DeActivate()
{
	myHasContact = false;
	myIsActive = false;
}

void WalkMovement::SetVelocity(const CU::Vector2<float>& aVelocity)
{
	myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
	myMovementComponent.SetVelocity(aVelocity);
}

void WalkMovement::ReceiveNote(const ContactNote&)
{
}

void WalkMovement::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
	, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (myIsActive == false) return;
	if (aComponent != nullptr)
	{
		myHasContact = true;
		CU::Vector3<float> resetPos(myOrientation.GetPos());
		resetPos.z = 0.f;
		if (aHitNormal.y > 0.f)
		{
			resetPos.y = aHitPosition.y + GC::PlayerHeightWithLegs;
		}
		else if (aDirection.x > 0)
		{
			resetPos.x = aHitPosition.x - GC::PlayerRadius;
			myVelocity.x = 0;
		}
		else if (aDirection.x < 0)
		{
			resetPos.x = aHitPosition.x + GC::PlayerRadius;
			myVelocity.x = 0;
		}

		myOrientation.SetPos(resetPos);
	}
}

void WalkMovement::HandleContact()
{
	CU::Vector3<float> leftOrigin(myOrientation.GetPos().x - GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> rightOrigin(myOrientation.GetPos().x + GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> centerPosition(myOrientation.GetPos());

	CU::Vector3<float> down(0.f, -1.f, 0.f);

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, down, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, down, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());

	CU::Vector3<float> left(-1.f, 0.f, 0.f);
	CU::Vector3<float> right(1.f, 0.f, 0.f);

	if (myVelocity.x < 0)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(centerPosition, left, GC::PlayerRadius, myRaycastHandler
			, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	}
	else if (myVelocity.x > 0)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(centerPosition, right, GC::PlayerRadius, myRaycastHandler
			, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	}


	myHasContact = false;
}

void WalkMovement::Drag(float aDeltaTime)
{
	if (myVelocity.x > 0)
	{
		myVelocity.x = fmaxf(myVelocity.x - myData.myWalkDrag * aDeltaTime, 0);
	}
	else if (myVelocity.x < 0)
	{
		myVelocity.x = fminf(myVelocity.x + myData.myWalkDrag * aDeltaTime, 0);
	}
	if (myVelocity.y > 0)
	{
		myVelocity.y = fmaxf(myVelocity.y - myData.myDrag.y * aDeltaTime, 0);
	}
	else if (myVelocity.y < 0)
	{
		myVelocity.y = fminf(myVelocity.y + myData.myDrag.y * aDeltaTime, 0);
	}
}



void WalkMovement::Walk(float aDeltaTime)
{
	if (fabs(myDirectionTarget.x) > myData.myDeadZone)
	{
		CU::Vector2<float> target(CU::GetNormalized(myDirectionTarget));

		myVelocity.x = myDirectionTarget.x * myData.myWalkSpeed * aDeltaTime;
	}
}

void WalkMovement::Translate()
{
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity, 0));

	//myOrientation.SetPos(CU::Vector3<float>(myOrientation.GetPos().x, fmaxf(myOrientation.GetPos().y, 0), myOrientation.GetPos().z));

	//if (myOrientation.GetPos().y == 0)
	//{
	//	myVelocity.y = 0;
	//}


	//only for debugging, keeping player inside screen:
	//if (myOrientation.GetPos().x < -15.f)
	//{
	//	myVelocity.x = fmaxf(myVelocity.x, 0);
	//}
	//else if (myOrientation.GetPos().x > 15.f)
	//{
	//	myVelocity.x = fminf(myVelocity.x, 0);
	//}
	//debugging out
}