#ifndef _RESOURCEPROVIDER_H_
#define _RESOURCEPROVIDER_H_

#include "Base.h"
#include "CEGUIDefaultResourceProvider.h"

namespace GUISystem {

	 /// The ResourceProvider class allows CEGUI load resources through our ResourceSystem.
	class ResourceProvider: public CEGUI::DefaultResourceProvider
	{
		virtual void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

		virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);

		virtual size_t getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec, const CEGUI::String &file_pattern, const CEGUI::String &resource_group);
	};
}

#endif // _RESOURCEPROVIDER_H_