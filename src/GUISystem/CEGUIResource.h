#ifndef _CEGUIRESOURCE_H_
#define _CEGUIRESOURCE_H_

#include "../Utility/Settings.h"
#include "../ResourceSystem/Resource.h"
#pragma warning(disable: 4717)

namespace GUISystem {

	class CEGUIResource : public ResourceSystem::Resource
	{
	public:
		virtual ~CEGUIResource(void) {}

		static ResourceSystem::ResourcePtr CreateMe(void);

		inline uint8* GetResource(uint32& Size) {
			EnsureLoaded();
			Size = mDataBlockSize;
			return mDataBlock;
		}

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