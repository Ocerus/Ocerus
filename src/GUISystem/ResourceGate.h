#ifndef _RESOURCEGATE_H_
#define _RESOURCEGATE_H_

#include "Base.h"
#include "CEGUIResourceProvider.h"

namespace GUISystem {
	class ResourceGate : public CEGUI::ResourceProvider
	{
		virtual void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
			const CEGUI::String& resourceGroup);
		virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);
	};
}

#endif