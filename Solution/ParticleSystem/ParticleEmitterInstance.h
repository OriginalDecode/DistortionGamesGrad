#pragma once

#include <GrowingArray.h>
#include "ParticleEmitterData.h"
#include "ParticleData.h"
#include <bitset>

class Entity;

namespace Prism
{
	class Texture;
	class Camera;
	struct VertexBufferWrapper;

	class ParticleEmitterInstance
	{
	public:
		ParticleEmitterInstance();
		~ParticleEmitterInstance();
		void Initiate(ParticleEmitterData* aData, bool anAllowManyParticles);
		void ReleaseData();
		void Render(Prism::Texture* aSceneTexture);
		void Update(float aDeltaTime);
		void Activate(bool aShouldRender = true);
		bool IsActive();

		void SetPosition(const CU::Vector3f& aPosition);

		void SetEmitterLifeTime(float aLifeTime);
		void KillEmitter(float aKillTime);

		void SetEntity(Entity* anEntity);
		Entity* GetEntity();

		void SetRadius(float aRadius);
		void SetSize(const CU::Vector3f& aSize);
		void SetDirection(const CU::Vector3<float>& aDirection);
		void ToggleDebugLines();

		bool GetShouldRender();
		void SetShouldRender(bool aShouldRender);

		void SetRotation(const CU::Vector3<float>& aRotation);
		CU::Vector3<float> GetPosition() const;

		void SetRandomizeDirection(bool aShouldBeSet);
		void SetOtherOrientation(const CU::Matrix44f& aMatrix);
		void CalcRotation(const CU::Vector3f& aDirectionToCalcFrom);
	private:
		void CheckFlags();
		void Reset();
		void CreateVertexBuffer();

		void UpdateEmitter(float aDeltaTime);
		void UpdateParticle(float aDeltaTime);

		void EmitParticle(const CU::Matrix44f& aWorldMatrix);
		void Reflect(CU::Vector3<float>&  aOutputVector, const CU::Vector3<float>& aIncidentVector, const CU::Vector3<float>& aNormal);

		int UpdateVertexBuffer();

		CU::Vector3<float> CreateCirclePositions();
		CU::Vector3<float> CreateSpherePositions();
		CU::Vector3<float> CreateHollowSquare();
		CU::Matrix44f myOrientation;
		CU::Matrix44f myOtherOrientation;

		ID3DX11EffectShaderResourceVariable* myOffsetTexture = nullptr;
		ID3DX11EffectShaderResourceVariable* mySceneTexture = nullptr;


		CU::GrowingArray<GraphicalParticle> myParticleToGraphicsCard;

		CU::GrowingArray<LogicalParticle> myLogicalParticles;
		CU::GrowingArray<GraphicalParticle> myGraphicalParticles;

		CU::Vector3<float> myDiffColor;
		CU::Vector3<float> myDirection;
		CU::Vector3<float> myRotation;
		CU::Vector3<float> myPoints[8];
		
		Texture* myTexture;
		Texture* myEmissiveTexture;
		Texture* myOffset;

		ParticleEmitterData* myParticleEmitterData;
		VertexBufferWrapper* myVertexWrapper;

		float myEmissionTime;
		float myEmitterLife;
		float myParticleScaling;
		float myParticleSpeed;
		float myRotationToOverrideWith;

		int myParticleIndex;
		int myLiveParticleCount;

		bool myOverrideDirection;
		bool myRandomizeDirection = false;
		bool myIsAffectedByGravity = false;
		bool myOverrideRotation = false;
		Entity* myEntity;

		enum eEmitterStates
		{
			HOLLOW,
			ACTIVE,
			CIRCLE,
			SPHERE,
			EMITTERLIFE,
			USE_ALPHA_DELTA,
			AFFECTED_BY_GRAVITY,
			IS_HAT,
			_COUNT
		};

		std::bitset<_COUNT> myStates;
		std::string myEmitterPath;
	};

	inline Entity* ParticleEmitterInstance::GetEntity()
	{
		return myEntity;
	}

	inline void ParticleEmitterInstance::SetDirection(const CU::Vector3<float>& aDirection)
	{
		myOverrideDirection = true;
		myDirection = aDirection;
	}

	inline void ParticleEmitterInstance::SetSize(const CU::Vector3f& aSize)
	{
		myParticleEmitterData->myEmitterSize = aSize * 0.5f;
	}

	inline bool ParticleEmitterInstance::IsActive()
	{
		return myStates[ACTIVE];
	}

	inline void ParticleEmitterInstance::SetRadius(float aRadius)
	{
		myParticleEmitterData->myEmitterSize.x = aRadius;
		myParticleEmitterData->myEmitterSize.z = aRadius;
		myParticleEmitterData->myEmitterSize.y = 0.f;
	}

	inline void ParticleEmitterInstance::SetPosition(const CU::Vector3f& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	inline void ParticleEmitterInstance::SetEmitterLifeTime(float aLifeTime)
	{
		myEmitterLife = aLifeTime;
		myStates[EMITTERLIFE] = TRUE;
	}

	inline void ParticleEmitterInstance::SetRotation(const CU::Vector3<float>& aRotation)
	{
		myRotation = aRotation;
	}

}
