#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "../Utility/SmartPointer.h"
#include "../Utility/Settings.h"
#include <boost/filesystem/fstream.hpp>

/// Forward declaration.
class DataContainer;

namespace ResourceSystem
{
	/** This abstract class represents 1 resource in the resource system. All concrete resources must derive from this 
		class and it's their responsibility to load necessary data when requested. The loading must happen inside
		LoadImpl method and unloading inside UnloadImpl method. There are helper methods OpenInputStream, 
		CloseInputStream and GetRawInputData to make loading easier.
		Note that you should never work with Rersources via direct pointers. Always use ResourcePtr smart pointer.
	*/
	class Resource
	{
	public:
		/// State of a resource.
		enum eState { STATE_UNINITIALIZED=0, STATE_INITIALIZED, STATE_UNLOADING, STATE_LOADING, STATE_LOADED };
		/// Type of a resource. Add your custom types here!
		enum eType { TYPE_TEXTURE=0, TYPE_CEGUIRESOURCE, NUM_TYPES,	TYPE_AUTODETECT };

		/// No need to override.
		Resource(void);
		/// Don't forget to override!
		virtual ~Resource(void);

		/// Enforces this resource to be loaded. Do not override this!
		virtual bool Load(void);
		/// Enforces this resource to be unloaded. Do not override this!
		virtual bool Unload(void);

		/// Getters.
		//@{
		inline eType GetType(void) const { return mType; }
		inline eState GetState(void) const { return mState; }
		inline string GetName(void) const { return mName; }
		//@}

	protected:
		eState mState;

		/** These methods are called whenever the resource is to be loaded or unloaded. Put your custom code inside their
			implementation. You can use OpenInputStream, CloseInputStream and GetRawInputData from inside them.
		*/
		//@{
		virtual bool LoadImpl(void) = 0;
		virtual bool UnloadImpl(void) = 0;
		//@}

		/** Open input stream from where raw data can be read. Valid only from within LoadImpl! Don't forget to close 
			the stream after you're done.
		*/
		InputStream& OpenInputStream(eInputStreamMode = ISM_BINARY );
		/// Closes the stream opened by OpenInputStream.
		void CloseInputStream(void);
		/// Returns raw data read from the input stream. The data is stored inside the outData container.
		void GetRawInputData(DataContainer& outData);
		/// Call this whenever you need to make sure the data is loaded.
		void EnsureLoaded(void);

	private:
		friend class ResourceMgr;
		string mFilePath;
		boost::filesystem::ifstream* mInputStream;
		eType mType;
		string mName;

		/// For internal use by the resource manager.
		//@{
		inline void SetName(const string& name) { mName = name; }
		inline void SetFilepath(const string& filepath) { mFilePath = filepath; }
		inline void SetState(const eState newState) { mState = newState; }
		inline void SetType(const eType newType) { mType = newType; }
		//@}
	};

	/// Use this instead of a direct pointer to Resource.
	typedef SmartPointer<Resource> ResourcePtr;
}

#endif