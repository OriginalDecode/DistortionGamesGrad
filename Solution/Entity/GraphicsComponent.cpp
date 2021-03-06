#include "stdafx.h"

#include "GraphicsComponent.h"
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Texture.h>

GraphicsComponent::GraphicsComponent(Entity& aEntity, const GraphicsComponentData& aComponentData)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.GetOrientation());
}

GraphicsComponent::~GraphicsComponent()
{
	//myEntity.GetScene()->RemoveInstance(myInstance);
	SAFE_DELETE(myInstance);
}

void GraphicsComponent::InitDLL(const char* aModelPath, const char* aEffectPath)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aModelPath
		, aEffectPath);

	Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath);
	model->SetEffect(Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath));

	myInstance = new Prism::Instance(*model, myEntity.GetOrientation());
}

void GraphicsComponent::InitCube(float aWidth, float aHeight, float aDepth)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadCube(aWidth, aHeight, aDepth);

	myInstance = new Prism::Instance(*model, myEntity.GetOrientation());
}

void GraphicsComponent::Update(float aDeltaTime)
{
	if (myEntity.GetType() == eEntityType::SAW_BLADE)
	{
		myEntity.SetRotation({ 0, 0, 15.f * aDeltaTime });
	}
}

void GraphicsComponent::SetScale(const CU::Vector3<float>& aScale)
{
	myInstance->SetScale(aScale);
}