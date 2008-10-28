#include "ResourceGate.h"

namespace GUISystem {

	void ResourceGate::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
		const CEGUI::String& resourceGroup) {
		
		ResourceSystem::ResourcePtr res;

		ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(resourceGroup.c_str(), filename.c_str());

		res = ResourceSystem::ResourceMgr::GetSingleton().GetResource(resourceGroup.c_str(), filename.c_str());
		output.setData((uint8 *)res.GetPointer());
	}

	void ResourceGate::unloadRawDataContainer(CEGUI::RawDataContainer& data) {
		//ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(resourceGroup.c_str(), filename.c_str());		
	}
}