#ifndef _CEGUIRESOURCE_H_
#define _CEGUIRESOURCE_H_

#include "Base.h"
#include "ResourcePointers.h"
#include "ResourceSystem/Resource.h"

namespace CEGUI { class RawDataContainer; }

namespace GUISystem {
	/**
	 * The CEGUIResource class is a resource wrapper that allows loading of CEGUI
	 * resources through Ocerus resource system. Unfortunately CEGUI is not designed
	 * to allow automatic resource unloading and reloading on demand, thus CEGUIResource
	 * is considered unloaded as soon as the resource is delivered to CEGUI.
	 * @warning: Again, CEGUI resources cannot be unloaded by resource manager because of
	 * simple CEGUI architecture.
	 */
	class CEGUIResource: public ResourceSystem::Resource
	{
	public:
		virtual ~CEGUIResource() {}

		/// Factory method.
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// Copies the resource data to outData and unloads the resource.
		void GetResource(CEGUI::RawDataContainer& outData);

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_CEGUIRESOURCE; }

	protected:
		friend class ResourceMgr;

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);

		DataContainer mData;
	};
}

#endif