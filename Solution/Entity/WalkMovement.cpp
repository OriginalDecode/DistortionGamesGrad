#include "stdafx.h"
#include "ContactNote.h"
#include "WalkMovement.h"
#include "MovementComponent.h"

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
	HandleContact();

	//myVelocity.y += myData.myGravity * aDeltaTime;
	myVelocity.y = 0;

	Drag(aDeltaTime);
	Walk(aDeltaTime);

	Translate();

	myPreviousPosition = myOrientation.GetPos().GetVector2();
}

void WalkMovement::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myDirectionTarget = aDirection;
}

void WalkMovement::Impulse()
{
	//myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
	myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
	myMovementComponent.Impulse();
}

void WalkMovement::Activate()
{
	myVelocity.x = 0.f;
	myVelocity.y = 0.f;
	myHasContact = true;
}

void WalkMovement::DeActivate()
{
	int apa = 5;
	apa;
}

void WalkMovement::ReceiveNote(const ContactNote& aNote)
{
	//memcpy(&myPreviousContact, &myCurrentContact, sizeof(Contact));
	myPreviousContact.myContactNormal.x = myCurrentContact.myContactNormal.x;
	myPreviousContact.myContactNormal.y = myCurrentContact.myContactNormal.y;
	myPreviousContact.myContactPoint.x = myCurrentContact.myContactPoint.x;
	myPreviousContact.myContactPoint.y = myCurrentContact.myContactPoint.y;
	myPreviousContact.myLostTouch = myCurrentContact.myLostTouch;
	myPreviousContact.myOther = myCurrentContact.myOther;

	if (aNote.myHasEntered == false)
	{
		myCurrentContact.myOther = nullptr;
	}
	else
	{
		myCurrentContact.myOther = aNote.myOther;
	}

	myCurrentContact.myContactPoint.x = aNote.myContactPoint.x;
	myCurrentContact.myContactPoint.y = aNote.myContactPoint.y;
	myCurrentContact.myContactNormal.x = aNote.myContactNormal.x;
	myCurrentContact.myContactNormal.y = aNote.myContactNormal.y;
	myCurrentContact.myLostTouch = !aNote.myHasEntered;

	if (myCurrentContact.myOther == nullptr && myPreviousContact.myOther == nullptr)
	{
		myHasContact = false;
	}
}

void WalkMovement::HandleContact()
{
	//if (myCurrentContact.myOther == nullptr && myPreviousContact.myOther == nullptr)
	if (myHasContact == false)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
		myCurrentContact.myLostTouch = false;
	}
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