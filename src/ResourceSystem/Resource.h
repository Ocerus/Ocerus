/// @file
/// Generic representation of a resource in the ResourceSystem.

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Base.h"
#include "ResourceTypes.h"

namespace ResourceSystem
{
	/// @brief This abstract class represents 1 resource in the resource system. All concrete resources must derive from this 
	///	class and it's their responsibility to load necessary data when requested.
	/// @remarks The loading must happen inside LoadImpl method and unloading inside UnloadImpl method. There are
	/// helper methods OpenInputStream, CloseInputStream and GetRawInputData to make loading easier. Note that you
	/// should never work with Rersources via direct pointers. Always use ResourcePtr smart pointer.
	class Resource
	{
	public:

		/// State of a resource.
		enum eState 
		{ 
			/// The resource was not yet initialized in the ResourceSystem and it doesn't know about it.
			STATE_UNINITIALIZED=0, 
			/// The resource was initialized in the ResourceSystem and it knows about it. But it's not loaded yet.
			STATE_INITIALIZED, 
			/// The resource is being unloaded from memory, but it will remain initialized.
			STATE_UNLOADING, 
			/// The resource is loading to the memory.
			STATE_LOADING, 
			/// The resource is fully loaded and ready to use.
			STATE_LOADED 
		};

		/// @remarks No need to override.
		Resource(void);
		/// @remarks Don't forget to override!
		virtual ~Resource(void);

		/// Enforces this resource to be loaded. Do not override this!
		virtual bool Load(void);
		
		/// @brief Enforces this resource to be unloaded. Do not override this!
		///	@param allowManual If allowManual is true, the resource will be unloaded even if it was manually created.
		virtual bool Unload(bool allowManual = false);

		/// Returns the user type of this resource.
		inline eResourceType GetType(void) const { return mType; }

		/// Returns the current state of this resource.
		inline eState GetState(void) const { return mState; }

		/// Returns the string name of this resource. The name comes without the group the resource belongs to.
		inline string GetName(void) const { return mName; }

		/// Returns the path to the file this resource belongs to. An empty string is returned when no such exists.
		inline string GetFilePath(void) const { return mFilePath; }

		/// Returns true if this resource was manually created and is not managed by the ResourceMgr.
		inline bool IsManual(void) const { return mIsManual; }

	protected:

		/// @brief Called whenever the resource is to be loaded.
		/// @remarks Put your custom code inside the
		///	implementation. You can use the OpenInputStream, CloseInputStream and GetRawInputData methods from inside.
		virtual bool LoadImpl(void) = 0;

		/// @brief Called whenever the resource is to be unloaded.
		/// @remarks Put your custom code inside the
		///	implementation. You can use the OpenInputStream, CloseInputStream and GetRawInputData methods from inside them.
		virtual bool UnloadImpl(void) = 0;

		/// @brief Opens input stream from where raw data can be read.
		/// @remarks Valid only from within LoadImpl! Don't forget to close 
		///	the stream after you're done.
		InputStream& OpenInputStream(eInputStreamMode = ISM_BINARY );

		/// Closes the stream opened by OpenInputStream.
		void CloseInputStream(void);

		/// Returns raw data read from the input stream. The data is stored inside the outData container.
		void GetRawInputData(DataContainer& outData);

		/// Call this whenever you need to make sure the data is loaded.
		void EnsureLoaded(void);

		/// @brief Changes the state of the resource.
		/// @remarks Don't call this unless you are doing custom loading of the resource outside of LoadImpl.
		inline void SetState(const eState newState) { mState = newState; }

	private:

		friend class ResourceMgr;
		string mFilePath;
		bool mIsManual;
		eResourceType mType;
		string mName;
		eState mState;

		/// Used by the implementation of OpenInputStream.
		boost::filesystem::ifstream* mInputFileStream;

		/// For internal use by the resource manager.
		inline void SetName(const string& name) { mName = name; }
		inline void SetFilepath(const string& filepath) { mFilePath = filepath; }
		inline void SetManual(bool manual) { mIsManual = manual; }
		inline void SetType(const eResourceType newType) { mType = newType; }
	};

}

#endif