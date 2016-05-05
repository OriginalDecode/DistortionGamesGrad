#pragma once
#include "Movement.h"

class FlyMovement : public Movement
{
public:
	FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation);
	~FlyMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;

	void ReceiveNote(const ContactNote& aNote) override;

private:
	void HandleContact();
	void Drag(float aDeltaTime);
	void Rotate(float aDeltaTime);
	void Translate();

	CU::Vector2<float> myPreviousPosition;

	struct Contact
	{
		Contact() : myActive(false), myOther(nullptr){};
		bool myActive;
		Entity* myOther;
		CU::Vector2<float> myContactPoint;
		CU::Vector2<float> myContactNormal;
	};
	volatile Contact myContact;
};
