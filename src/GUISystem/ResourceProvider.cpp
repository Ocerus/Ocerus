#include "Common.h"
#include "ResourceProvider.h"
#include "CEGUIResource.h"

namespace GUISystem {

	void ResourceProvider::loadRawDataContainer(const CEGUI::String& filename,
		CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup)
	{
		CEGUIResourcePtr res = (CEGUIResourcePtr)(gResourceMgr.GetResource(resourceGroup.c_str(), filename.c_str()));
		res->GetResource(output);
	}

	void ResourceProvider::unloadRawDataContainer(CEGUI::RawDataContainer& data)
	{
		delete[] data.getDataPtr();
		data.setData(0);
		data.setSize(0);
	}

	size_t ResourceProvider::getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec,
		const CEGUI::String &file_pattern, const CEGUI::String &resource_group)
	{
		///@todo implement this;
		return 0;
	}
}