#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "../Utility/SmartPointer.h"
#include "../Utility/Settings.h"
#include "../Utility/ResourcePointers.h"
//TODO odstranit tenhle include!
#include <boost/filesystem/fstream.hpp>

/// @name Forward declaration.
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
		/// @name State of a resource.
		enum eState { STATE_UNINITIALIZED=0, STATE_INITIALIZED, STATE_UNLOADING, STATE_LOADING, STATE_LOADED };
		/// @name Type of a resource. Add your custom types here!
		enum eType { TYPE_TEXTURE=0, TYPE_CEGUIRESOURCE, TYPE_TEXTRESOURCE, TYPE_XMLRESOURCE, TYPE_PARTICLERESOURCE, NUM_TYPES, TYPE_AUTODETECT };

		/// @name No need to override.
		Resource(void);
		/// @name Don't forget to override!
		virtual ~Resource(void);

		/// @name Enforces this resource to be loaded. Do not override this!
		virtual bool Load(void);
		/** Enforces this resource to be unloaded. Do not override this!
			\param allowManual If allowManual is true, the resource will be unloaded even if it was manually created.
		*/
		virtual bool Unload(bool allowManual = false);

		/// @name Getters.
		//@{
		inline eType GetType(void) const { return mType; }
		inline eState GetState(void) const { return mState; }
		inline String GetName(void) const { return mName; }
		inline String GetFilePath(void) const { return mFilePath; }
		inline bool IsManual(void) const { return mIsManual; }
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
		/// @name Closes the stream opened by OpenInputStream.
		void CloseInputStream(void);
		/// @name Returns raw data read from the input stream. The data is stored inside the outData container.
		void GetRawInputData(DataContainer& outData);
		/// @name Call this whenever you need to make sure the data is loaded.
		void EnsureLoaded(void);
		/// @name Set the current state of the resource.
		inline void SetState(const eState newState) { mState = newState; }

	private:
		friend class ResourceMgr;
		String mFilePath;
		bool mIsManual;
		eType mType;
		String mName;
		/// @name Used by the impl of OpenInputStream
		//@{
		boost::filesystem::ifstream* mInputFileStream;
		//@}


		/// @name For internal use by the resource manager.
		//@{
		inline void SetName(const String& name) { mName = name; }
		inline void SetFilepath(const String& filepath) { mFilePath = filepath; }
		inline void SetManual(bool manual) { mIsManual = manual; }
		inline void SetType(const eType newType) { mType = newType; }
		//@}
	};

}

#endif