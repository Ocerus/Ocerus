#include "Common.h"
#include "StringValueEditorModel.h"
#include "Utils/ResourcePointers.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

namespace Editor
{
	/// Template specialization of GetValue() for resources.
	/*template<>
	virtual string StringValueEditorModel<ResourceSystem::ResourcePtr>::GetValue() const
	{
		ResourceSystem::ResourcePtr resource = mWrappedModel->GetValue(); 
		return resource ? resource->GetName() : "";
	}*/
}
