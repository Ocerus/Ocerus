#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "../Utility/SmartPointer.h"

namespace ResourceSystem
{
	typedef SmartPointer<Resource> ResourcePtr;

	class Resource
	{
	public:
		Resource() {}
		virtual ~Resource();

		virtual std::istream& Open();

		virtual void Load() = 0;
		virtual void Unload() = 0;

		eResourceType GetType() { return mType; }
		eResourceState GetState() { return mState; }
		string GetName() { return mName; }

		enum eResourceState { RSTATE_UNINITIALIZED, RSTATE_INITIALIZED, RSTATE_LOADING, RSTATE_LOADED, RSTATE_PREPARING, RSTATE_UNLOADING };
	protected:
		friend ResourceMgr;

		eResourceType mType;
		eResourceState mState;
		string mName;
		string mFilePath;

		void SetName(const string& name) { mName = name; }
		void SetFilepath(const string& filepath) { mFilePath = filepath; }
	};
}

#endif