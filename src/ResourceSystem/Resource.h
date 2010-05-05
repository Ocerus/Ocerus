/// @file
/// Generic representation of a resource in the ResourceSystem.

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Base.h"
#include "ResourceTypes.h"
#include "boost/filesystem/fstream.hpp"
#include "Utils/StringKey.h"

namespace ResourceSystem
{
	/// This abstract class represents 1 resource in the resource system. All concrete resources must derive from this
	///	class and it's their responsibility to load necessary data when requested.
	/// The loading must happen inside LoadImpl method and unloading inside UnloadImpl method. There are
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

		/// No need to override.
		Resource(void);

		/// Don't forget to override!
		virtual ~Resource(void);

		/// Enforces this resource to be loaded. Do not override this!
		virtual bool Load(void);

		/// Enforces this resource to be unloaded. Do not override this!
		///	@param allowManual If allowManual is true, the resource will be unloaded even if it was manually created.
		virtual bool Unload(bool allowManual = false);

		/// Reloads the resource
		virtual void Reload(void);

		/// If the resource is out of date (with respect to the source of its data), it will be reloaded.
		virtual void Refresh(void);

		/// Returns the user type of this resource.
		inline eResourceType GetType(void) const { return mType; }

		/// Returns the current state of this resource.
		inline eState GetState(void) const { return mState; }

		/// Returns the string name of this resource. The name comes without the group the resource belongs to.
		inline string GetName(void) const { return mName; }

		/// Returns the first group of this resource.
		inline StringKey GetGroup(void) const { return mGroup; }

		/// Returns the path to the file this resource belongs to. An empty string is returned when no such exists.
		inline string GetFilePath(void) const { return mFilePath; }

		/// Returns the path to the directory containing the file.
		string GetFileDir(void) const;

		/// Returns the relative path to the directory containing the file. The path is relative to appropriate base prefix.
		string GetRelativeFileDir(void) const;

		/// Returns true if this resource was manually created and is not managed by the ResourceMgr.
		inline bool IsManual(void) const { return mIsManual; }

		/// Returns the size in bytes of the loaded resource. Null if it is not loaded.
		size_t GetSize(void) const;

		/// Returns the last time (counter based) this resource was used.
		inline uint64 GetLastUsedTime(void) const { return mLastUsedTime; }
		
		/// Returns base path type of this resource.
		inline eBasePathType GetBasePathType(void) const { return mBasePathType; }


	protected:

		/// Called whenever the resource is to be loaded.
		/// Put your custom code inside the implementation. You can use the OpenInputStream, CloseInputStream and
		/// GetRawInputData methods from inside.
		/// @returns The size of the data kept in memory for this resource after it was loaded. Null is a valid value
		/// indicating there was a problem with the load.
		virtual size_t LoadImpl(void) = 0;

		/// Called whenever the resource is to be unloaded.
		/// Put your custom code inside the	implementation. You can use the OpenInputStream, CloseInputStream and
		/// GetRawInputData methods from inside them.
		/// @returns True if successful, false otherwise.
		virtual bool UnloadImpl(void) = 0;

		/// Opens input stream from where raw data can be read.
		/// Valid only from within LoadImpl! Don't forget to close
		///	the stream after you're done.
		InputStream& OpenInputStream(eInputStreamMode = ISM_BINARY );

		/// Closes the stream opened by OpenInputStream.
		void CloseInputStream(void);

		/// Returns raw data read from the input stream. The data is stored inside the outData container.
		void GetRawInputData(DataContainer& outData);

		/// Call this whenever you need to make sure the data is loaded.
		void EnsureLoaded(void);

		/// Changes the state of the resource.
		/// Don't call this unless you are doing custom loading of the resource outside of LoadImpl.
		inline void SetState(const eState newState) { mState = newState; }

	private:

		friend class ResourceMgr;
		string mFilePath;
		string mName;
		StringKey mGroup;
		int64 mLastWriteTime;
		uint64 mLastUsedTime;
		bool mIsManual;
		eResourceType mType;
		eState mState;
		size_t mSizeInBytes;
		eBasePathType mBasePathType;

		/// Used by the implementation of OpenInputStream.
		boost::filesystem::ifstream* mInputFileStream;

		/// For internal use by the resource manager.
		inline void SetName(const string& name) { mName = name; }
		inline void SetGroup(const StringKey& group) { mGroup = group; }
		inline void SetFilePath(const string& filepath) { mFilePath = filepath; }
		inline void SetManual(bool manual) { mIsManual = manual; }
		inline void SetType(const eResourceType newType) { mType = newType; }
		inline void SetBasePathType(const eBasePathType newBasePathType) { mBasePathType = newBasePathType; }
		void RefreshResourceInfo(void);

	private:

		static uint64 sLastUsedTime;
		inline static void ResetLastUsedTime(void) { sLastUsedTime = 0; }
	};

}

#endif