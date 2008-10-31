#include "ResourceGate.h"
#include "CEGUIResource.h"

namespace GUISystem {

	void ResourceGate::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
		const CEGUI::String& resourceGroup) {
		
		uint32 res_size;

		assert(ResourceSystem::ResourceMgr::GetSingleton());

		ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(resourceGroup.c_str(), filename.c_str());

		CEGUIResourcePtr res = (CEGUIResourcePtr)ResourceSystem::ResourceMgr::GetSingleton()
			.GetResource(resourceGroup.c_str(), filename.c_str());


		output.setData((uint8 *)(res->GetResource(res_size)));
		output.setSize((size_t)res_size);
	}

	void ResourceGate::unloadRawDataContainer(CEGUI::RawDataContainer& data) {
		//ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(resourceGroup.c_str(), filename.c_str());		
	}
}