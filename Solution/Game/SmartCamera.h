#pragma once
#include "GrowingArray.h"
#include <Subscriber.h>
#include <bitset>
namespace Prism
{
	class Camera;
}

class SmartCamera : public Subscriber
{
public:
	SmartCamera(Prism::Camera& aCamera);
	~SmartCamera();

	void Update(float aDeltaTime);
	void AddPlayer(const CU::Matrix44f* aPlayerOrientation, const CU::Vector2<float>* aPlayerVelocity);
	void SetActivePlayerCount(int aPlayerCount);
	void SetStartPosition(const CU::Vector3f& aPosition);
	void ReceiveMessage(const PlayerActiveMessage& aMessage) override;
	void ReceiveMessage(const OnDeathMessage& aMessage) override;
	void ReceiveMessage(const OnPlayerLevelComplete& aMessage) override;

	const CU::Matrix44<float>& GetOrientation() const;

	int GetPlayerCount();
	int GetActivePlayerCount();

private:

	enum eActivePlayer
	{
		_NONE_DONT_USE_ME_I_AM_BAD,
		PLAYER_1,
		PLAYER_2,
		_COUNT
	};


	void operator=(SmartCamera&) = delete;
	CU::Vector3<float> CalcTargetPosition(int aPlayer) const;

	CU::Matrix44f myOrientation;
	Prism::Camera& myCamera;
	CU::GrowingArray<const CU::Matrix44f*> myPlayerOrientations;
	CU::GrowingArray<const CU::Vector2<float>*> myPlayerVelocities;
	CU::Vector3f myStartPosition;

	std::bitset<_COUNT> myActivePlayers;
	int myActivePlayerCount;

	float myMinZoom;
	float myMaxZoom;

	bool myShouldResetCameraPos;
	float myCameraAlpha;
	float myResponsiveness;
	float my2PlayersMoveBackMultiplier;
	CU::Vector3<float> myVelocityMultiplier;
};

