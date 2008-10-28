#ifndef _RESOURCEGATE_H_
#define _RESOURCEGATE_H_

#include "../Utility/Settings.h"
// Common.h v headerech radsi neincludovat, pak vznikaji jeste vetsi zavislosti
#include "../Common.h"
#include "CEGUI.h"
//#include "CEGUIResourceProvider.h"
#include "CEGUIDataContainer.h"
//#include "CEGUIString.h"
//#include <string> misto toho naincludovat Settings.h
#include "../Utility/Settings.h"
//#pragma once

namespace GUISystem {
	class ResourceGate : public CEGUI::ResourceProvider
	{
		virtual void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
			const CEGUI::String& resourceGroup);
		virtual void unloadRawDataContainer(CEGUI::RawDataContainer& data);
	};
}

#endif