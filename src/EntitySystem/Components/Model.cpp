#include "Common.h"
#include "Model.h"
#include "GfxSystem/GfxRenderer.h"
#include "GfxSystem/GfxSceneMgr.h"

void EntityComponents::Model::Create( void )
{
	mTransparency = 0.0f;
	mYAngle = 0.0f;
}

void EntityComponents::Model::Destroy( void )
{
	gGfxRenderer.GetSceneManager()->RemoveDrawable(this);
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
				mMeshHandle = gResourceMgr.GetResource("General", ResourceSystem::RES_NULL_MODEL);
				OC_ASSERT(mMeshHandle.get());
			}

			if (!gEntityMgr.IsEntityPrototype(GetOwner()))
			{
				Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
				gGfxRenderer.GetSceneManager()->AddDrawable(this, transform);
			}

			return EntityMessage::RESULT_ERROR;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Model::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Mesh", &Model::GetMesh, &Model::SetMesh, PA_FULL_ACCESS, "Resource from which the model is grabbed.");
	RegisterProperty<float32>("Transparency", &Model::GetTransparency, &Model::SetTransparency, PA_FULL_ACCESS, "How transparent the model is. 0 is fully opaque while 1 is fully transparent.");
	RegisterProperty<float32>("YAngle", &Model::GetYAngle, &Model::SetYAngle, PA_FULL_ACCESS, "Rotation along the Y axis in radians.");

	// we need the transform to be able to have the position and angle ready while creating the Model
	AddComponentDependency(CT_Transform);
}

void EntityComponents::Model::SetMesh(ResourceSystem::ResourcePtr value) 
{ 
	if (value && value->GetType() == ResourceSystem::RESTYPE_MESH)
	{
		mMeshHandle = value; 
	}
}
