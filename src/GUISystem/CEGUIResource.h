#ifndef _CEGUIRESOURCE_H_
#define _CEGUIRESOURCE_H_

#include "../Utility/Settings.h"
#include "../ResourceSystem/Resource.h"

namespace CEGUI { class RawDataContainer; }

namespace GUISystem {

	class CEGUIResource : public ResourceSystem::Resource
	{
	public:
		virtual ~CEGUIResource(void) {}

		static ResourceSystem::ResourcePtr CreateMe(void);

		void GetResource(CEGUI::RawDataContainer& outData);

	protected:
		friend class ResourceMgr;

		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);

		uint8* mDataBlock;
		uint32 mDataBlockSize;
	};


	typedef SmartPointer<CEGUIResource> CEGUIResourcePtr;
}

#endif