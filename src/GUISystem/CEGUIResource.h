#ifndef _CEGUIRESOURCE_H_
#define _CEGUIRESOURCE_H_

#include "../Utility/Settings.h"
#include "../ResourceSystem/Resource.h"

namespace GUISystem {
	struct DataBlock {
		DataBlock* prev;
		DataBlock* next;
		uint8* payload;
	};

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

	//typedef SmartPointer<CEGUIResource> CEGUIResourcePtr;

	class CEGUIResourcePtr : public SmartPointer<CEGUIResource>
	{
	public:
		explicit CEGUIResourcePtr(CEGUIResource* pointer): SmartPointer<CEGUIResource>(pointer) {}
		CEGUIResourcePtr(const CEGUIResourcePtr& r): SmartPointer<CEGUIResource>(r) {}
		CEGUIResourcePtr(const ResourceSystem::ResourcePtr& r): SmartPointer<CEGUIResource>() {	Recreate(r); }
		CEGUIResourcePtr& operator=(const ResourceSystem::ResourcePtr& r)
		{
			if (mPointer == static_cast<CEGUIResource*>(r.GetPointer()))
				return *this;
			Release();
			Recreate(r);
			return *this;
		}
	private:
		void Recreate(const CEGUIResourcePtr& r)
		{
			mPointer = static_cast<CEGUIResource*>(r.GetPointer());
			mUseCountPtr = r.GetUseCountPtr();
			if (mUseCountPtr)
				++(*mUseCountPtr);
		}
	};
}

#endif