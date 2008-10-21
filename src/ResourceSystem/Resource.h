#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <ios>
#include "../Common.h"
#include "../Utility/SmartPointer.h"

namespace ResourceSystem
{
	class Resource
	{
	public:
		enum eState { STATE_UNINITIALIZED, STATE_INITIALIZED, STATE_LOADING, STATE_LOADED, STATE_PREPARING, STATE_UNLOADING };
		enum eType { TYPE_TEXTURE=0, NUM_TYPES,	TYPE_AUTODETECT };

		Resource(): mState(STATE_UNINITIALIZED) {}
		virtual ~Resource();

		virtual istream& Open();

		virtual void Load() = 0;
		virtual void Unload() = 0;

		eType GetType() { return mType; }
		eState GetState() { return mState; }
		string GetName() { return mName; }

	protected:
		friend class ResourceMgr;

		eType mType;
		eState mState;
		string mName;
		string mFilePath;


		void SetName(const string& name) { mName = name; }
		void SetFilepath(const string& filepath) { mFilePath = filepath; }
	};

	typedef SmartPointer<Resource> ResourcePtr;
}

#endif