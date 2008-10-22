#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "../Utility/SmartPointer.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

namespace ResourceSystem
{
	class Resource
	{
	public:
		enum eState { STATE_UNINITIALIZED=0, STATE_INITIALIZED, STATE_UNLOADING, STATE_LOADING, STATE_LOADED };
		enum eType { TYPE_TEXTURE=0, NUM_TYPES,	TYPE_AUTODETECT };

		Resource(void);
		virtual ~Resource(void);

		virtual bool Load(void);
		virtual bool Unload(void);

		inline eType GetType(void) const { return mType; }
		inline eState GetState(void) const { return mState; }
		inline string GetName(void) const { return mName; }

	protected:
		friend class ResourceMgr;

		eType mType;
		eState mState;
		string mName;
		string mFilePath;
		boost::filesystem::ifstream* mInputStream;

		virtual bool LoadImpl(void) = 0;
		virtual bool UnloadImpl(void) = 0;

		InputStream& OpenInputStream(void);
		void CloseInputStream(void);

		void SetName(const string& name) { mName = name; }
		void SetFilepath(const string& filepath) { mFilePath = filepath; }
	};

	typedef SmartPointer<Resource> ResourcePtr;
}

#endif