#include "Common.h"
#include "Core/Application.h"
#include "Editor/EditorMgr.h"
#include "ResourceProvider.h"

#include "CEGUIResource.h"

namespace GUISystem {

	void ResourceProvider::loadRawDataContainer(const CEGUI::String& filename,
		CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup)
	{
		ResourceSystem::ResourcePtr rawRes = gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());
		if (!rawRes)
		{
			output.setData(0);
			output.setSize(0);
			return;
		}
		CEGUIResourcePtr res = rawRes;
		if (!res)
		{
			// seems like the conversion to CEGUIResource failed, so let's try to fix the resource type
			if (gApp.IsDevelopMode())
				gEditorMgr.ChangeResourceType(rawRes, ResourceSystem::RESTYPE_CEGUIRESOURCE);
			else
				gResourceMgr.ChangeResourceType(rawRes, ResourceSystem::RESTYPE_CEGUIRESOURCE);

			rawRes = gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str());
			res = (CEGUIResourcePtr)rawRes;
		}
		res->GetResource(output);
	}

	void ResourceProvider::unloadRawDataContainer(CEGUI::RawDataContainer& data)
	{
		if (data.getDataPtr())
		{
			delete[] data.getDataPtr();
			data.setData(0);
			data.setSize(0);
		}
	}

	size_t ResourceProvider::getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec,
		const CEGUI::String &file_pattern, const CEGUI::String &resource_group)
	{
		OC_UNUSED(out_vec);
		OC_UNUSED(file_pattern);
		OC_UNUSED(resource_group);
		return 0;
	}
}