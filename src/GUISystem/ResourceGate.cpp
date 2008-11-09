#include "ResourceGate.h"
#include "CEGUIResource.h"

namespace GUISystem {

	void ResourceGate::loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
		const CEGUI::String& resourceGroup) {
		
		//ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(filename.c_str(), resourceGroup.c_str());

		CEGUIResourcePtr res = ResourceSystem::ResourceMgr::GetSingleton()
			.GetResource(resourceGroup.c_str(), filename.c_str());


		res->GetResource(output);
	}

	void ResourceGate::unloadRawDataContainer(CEGUI::RawDataContainer& data) {
		//ResourceSystem::ResourceMgr::GetSingleton().AddResourceFileToGroup(resourceGroup.c_str(), filename.c_str());		
	}
}