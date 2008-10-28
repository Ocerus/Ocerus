#include "ResourceGate.h"

namespace GUISystem {
	ResourceGate::ResourceGate();
	virtual ~ResourceGate::ResourceGate(void) {
	}

	virtual ResourceGate::getDefaultResourceGroup(void) const {
	}

	virtual void ResourceGate::loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup) {
	}

	virtual void ResourceGate::unloadRawDataContainer(RawDataContainer& data) {
	}
}