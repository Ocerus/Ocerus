#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "../Utility/SmartPointer.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

class DataContainer;

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
		eState mState;

		virtual bool LoadImpl(void) = 0;
		virtual bool UnloadImpl(void) = 0;

		InputStream& OpenInputStream(eInputStreamMode = ISM_BINARY );
		void CloseInputStream(void);
		void GetRawInputData(DataContainer& outData);
		void EnsureLoaded(void); // to be called in Get method

	private:
		friend class ResourceMgr;
		string mFilePath;
		boost::filesystem::ifstream* mInputStream;
		eType mType;
		string mName;

		inline void SetName(const string& name) { mName = name; }
		inline void SetFilepath(const string& filepath) { mFilePath = filepath; }
		inline void SetState(const eState newState) { mState = newState; }
		inline void SetType(const eType newType) { mType = newType; }
	};

	typedef SmartPointer<Resource> ResourcePtr;
}

#endif