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
		enum eState { STATE_UNINITIALIZED=0, STATE_INITIALIZED, STATE_UNLOADING, STATE_LOADING, STATE_LOADED, STATE_PREPARING, STATE_PREPARED };
		enum eType { TYPE_TEXTURE=0, NUM_TYPES,	TYPE_AUTODETECT };

		Resource(void): mState(STATE_UNINITIALIZED) {}
		virtual ~Resource(void);

		virtual istream& Open(void);

		virtual void Load(void) = 0;
		virtual void Unload(void) = 0;

		inline eType GetType(void) const { return mType; }
		inline eState GetState(void) const { return mState; }
		inline string GetName(void) const { return mName; }

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