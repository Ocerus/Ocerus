#ifndef _RESOURCEGATE_H_
#define _RESOURCEGATE_H_

#include "../Utility/Settings.h"
#include "../Common.h"
#include "CEGUISystem.h"
//#pragma once

namespace GUISystem {
	class ResourceGate : public ResourceProvider
	{
		ResourceGate();
		virtual ~ResourceGate(void);
		virtual getDefaultResourceGroup(void) const;
		virtual void loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup);
		virtual void unloadRawDataContainer(RawDataContainer& data);
	}
}

#endif