#include "Common.h"
#include "Model.h"

void EntityComponents::Model::Create( void )
{
	mTransparency = 0.0f;
}

void EntityComponents::Model::Destroy( void )
{
	//gGfxRenderer.GetSceneManager()->RemoveModel(this);
}

EntityMessage::eResult EntityComponents::Model::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			//Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
			//gGfxRenderer.GetSceneManager()->AddModel(this, transform);

			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Model::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Mesh", &Model::GetMesh, &Model::SetMesh, PA_FULL_ACCESS | PA_TRANSIENT, "");
	RegisterProperty<float32>("Transparency", &Model::GetTransparency, &Model::SetTransparency, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the Model
	AddComponentDependency(CT_Transform);
}
