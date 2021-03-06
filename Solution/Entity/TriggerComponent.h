#pragma once
#include "Component.h"
#include "GameEnum.h"

struct TriggerComponentData;

class TriggerComponent : public Component
{
public:
	TriggerComponent(Entity& anEntity, const TriggerComponentData& someData);
	~TriggerComponent();

	void Update(float aDelta) override;

	void ReceiveNote(const CollisionNote& aNote) override;

	void Activate();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	eTriggerType GetTriggerType() const;
	int GetLevelID() const;

	void SetLevelChangeID(int anID);

	float GetForce() const;
	void SetForce(float aForce);

private:

	const TriggerComponentData& myData;
	eTriggerType myTriggerType;
	int myLevelToChangeToID;
	float myForce;
};

inline eComponentType TriggerComponent::GetTypeStatic()
{
	return eComponentType::TRIGGER;
}

inline eComponentType TriggerComponent::GetType()
{
	return GetTypeStatic();
}

inline eTriggerType TriggerComponent::GetTriggerType() const
{
	return myTriggerType;
}

inline float TriggerComponent::GetForce() const
{
	return myForce;
}

inline void TriggerComponent::SetForce(float aForce)
{
	myForce = aForce;
}