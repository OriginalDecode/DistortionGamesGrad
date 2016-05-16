#include "stdafx.h"

#include "Camera.h"
#include "DecalPass.h"
#include "Instance.h"
#include "ModelLoader.h"
#include "Texture.h"

namespace Prism
{
	DecalPass::DecalPass()
		: myDecals(16)
	{
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		myInstance = new Instance(*model, myOrientation);;
	}


	DecalPass::~DecalPass()
	{
		SAFE_DELETE(myInstance);
	}

	void DecalPass::AddDecal(const CU::Vector3<float>& aPosition, const std::string& aTexturePath)
	{
		DecalInfo info;
		info.myPosition = aPosition;
		info.myTexture = TextureContainer::GetInstance()->GetTexture(aTexturePath);
		myDecals.Add(info);
	}

	void DecalPass::Render(const Camera& aCamera, Texture* aDepthTexture)
	{
		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_DISABLED);
		
		if (myInstance->GetModel().IsLoaded())
		{
			Effect* effect = myInstance->GetModel().GetEffect();
			
			effect->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
			effect->SetViewMatrixNotInverted(aCamera.GetOrientation());
			effect->SetProjectionMatrix(aCamera.GetProjection());
			effect->SetProjectionMatrixInverted(CU::InverseReal(aCamera.GetProjection()));
			effect->SetDepthTexture(aDepthTexture);

			for each (const DecalInfo& info in myDecals)
			{
				myOrientation.SetPos(info.myPosition);
				effect->SetWorldMatrixInverted(CU::InverseSimple(myOrientation));
				effect->SetTexture(info.myTexture);
				myInstance->Render(aCamera);
			}
			

		}

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);
	}

}