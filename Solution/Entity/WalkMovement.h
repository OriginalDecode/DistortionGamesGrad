#pragma once
#include "Movement.h"

class WalkMovement : public Movement
{
public:
	WalkMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent);
	~WalkMovement();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void SetDirectionTarget(const CU::Vector2<float>& aDirection) override;
	void Impulse() override;
	void Impulse(const CU::Vector2<float>& aVelocity) override;
	void Activate() override;
	void DeActivate() override;
	void SetVelocity(const CU::Vector2<float>& aVelocity) override;

	void ReceiveNote(const ContactNote& aNote) override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal) override;

private:
	CU::Vector3<float> myPreviousPosition;
	void HandleContact();
	void Drag(float aDeltaTime);
	void Walk(float aDeltaTime);
	void Translate();

	bool myHasContact;
};

