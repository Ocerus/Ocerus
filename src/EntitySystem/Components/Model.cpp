#include "Common.h"
#include "Model.h"
#include "GfxSystem/GfxRenderer.h"
#include "GfxSystem/GfxSceneMgr.h"

void EntityComponents::Model::Create( void )
{
	mTransparency = 0.0f;
	mZAngle = 0.0f;
}

void EntityComponents::Model::Destroy( void )
{
	gGfxRenderer.GetSceneManager()->RemoveModel(this);
}

EntityMessage::eResult EntityComponents::Model::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if (!mMeshHandle)
			{
				ocWarning << "Initing model with null value";
				///@TODO add null model
				//mMeshHandle = gResourceMgr.GetResource("General", "NullTexture");
				return EntityMessage::RESULT_ERROR;
			}

			Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
			gGfxRenderer.GetSceneManager()->AddModel(this, transform);

			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Model::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Mesh", &Model::GetMesh, &Model::SetMesh, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Transparency", &Model::GetTransparency, &Model::SetTransparency, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("ZAngle", &Model::GetZAngle, &Model::SetZAngle, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the Model
	AddComponentDependency(CT_Transform);
}

void EntityComponents::Model::SetMesh(ResourceSystem::ResourcePtr value) 
{ 
	if (value && value->GetType() == ResourceSystem::RESTYPE_MESH)
		mMeshHandle = value; 
}