#include "stdafx.h"
#include "ContactNote.h"
#include "MovementComponent.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myData(aInputData)
	, myOrientation(anOrientation)
{
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Update(float aDeltaTime)
{
	myVelocity.y += myData.myGravity * aDeltaTime;

	Drag(aDeltaTime);
	Rotate(aDeltaTime);

	Translate();

	DEBUG_PRINT(myOrientation.GetPos());
}

void MovementComponent::ReceiveNote(const ContactNote& aNote)
{
	CU::Vector2<float> toContact(aNote.myContactPoint.GetVector2() - myOrientation.GetPos().GetVector2());

	if (fabs(toContact.x) > fabs(toContact.y))
	{
		myVelocity.x = 0;
	}
	else
	{
		myVelocity.y = 0;
	}
}

void MovementComponent::Impulse()
{
	myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
}

void MovementComponent::Drag(float aDeltaTime)
{
	if (myVelocity.x > 0)
	{
		myVelocity.x = fmaxf(myVelocity.x - myData.myDrag.x * aDeltaTime, 0);
	}
	else if (myVelocity.x < 0)
	{
		myVelocity.x = fminf(myVelocity.x + myData.myDrag.x * aDeltaTime, 0);
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

void MovementComponent::Rotate(float aDeltaTime)
{
	if (CU::Length2(myUpTarget) > myData.myDeadZone * myData.myDeadZone)
	{
		CU::Vector2<float> up(myOrientation.GetUp().GetVector2());
		CU::Vector2<float> target(CU::GetNormalized(myUpTarget));

		float dot = CU::Dot(up, target);
		if (acosf(dot) > myData.myAngleEpsilon)
		{
			float perpDot = up.x * target.y - up.y * target.x;

			float signedAngle = atan2f(perpDot, dot);

			if (signedAngle < 0)
			{
				myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(-myData.myRotationSpeed * aDeltaTime) * myOrientation;
			}
			else if (signedAngle > 0)
			{
				myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(myData.myRotationSpeed * aDeltaTime) * myOrientation;
			}
		}
	}
}

void MovementComponent::Translate()
{
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity, 0));

	myOrientation.SetPos(CU::Vector3<float>(myOrientation.GetPos().x, fmaxf(myOrientation.GetPos().y, 0), myOrientation.GetPos().z));

	if (myOrientation.GetPos().y == 0)
	{
		myVelocity.y = 0;
	}


	//only for debugging, keeping player inside screen:
	if (myOrientation.GetPos().x < -15.f)
	{
		myVelocity.x = fmaxf(myVelocity.x, 0);
	}
	else if (myOrientation.GetPos().x > 15.f)
	{
		myVelocity.x = fminf(myVelocity.x, 0);
	}
	//debugging out
}