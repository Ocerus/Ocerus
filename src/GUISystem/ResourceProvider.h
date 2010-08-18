/// @file
/// Interconnects CEGUI with Ocerus resource system.
#ifndef _GUISYSTEM_RESOURCEPROVIDER_H_
#define _GUISYSTEM_RESOURCEPROVIDER_H_

#include "Base.h"
#include "CEGUIDefaultResourceProvider.h"

namespace GUISystem {

	/// The ResourceProvider class allows CEGUI load resources through our ResourceSystem.
	class ResourceProvider: public CEGUI::DefaultResourceProvider
	{
		/// Loads a resource from given filename and group and stores it to output container.
		virtual void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output, const CEGUI::String& resourceGroup);

		/// Unloads the resource in given container.
		virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);

		/// This method is not implemented in this implementation.
		virtual size_t getResourceGroupFileNames(std::vector<CEGUI::String> &out_vec, const CEGUI::String &file_pattern, const CEGUI::String &resource_group);
	};
}

#endif // _RESOURCEPROVIDER_H_