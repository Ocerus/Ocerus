/// @file
/// Implementation of a resource able to read files containing text data.

#ifndef _TEXTRESOURCE_H_
#define _TEXTRESOURCE_H_

#include "Base.h"
#include "../ResourceSystem/Resource.h"
#include "TextData.h"

namespace StringSystem
{
	/// Callback invoked when a text resource is to be unloaded.
	typedef void (*TextResourceUnloadCallback)(TextResource* resource);
	
	/// This class represents a single text resource.
	/// Main purpose of the class is to load and parse a .str file, not to store the actual data. You should
	///	only use StringMgr to load text resources (although its possible to load it directly with ResourceMgr,
	///	IT IS NOT recommended). Also, use StringMgr to access the actual text data, because TextResource is normally discarded
	///	after loading (StringMgr calls GetTextDataMap() and Unload()), thus it may be required to load it
	///	again, which would be slow.
	class TextResource : public ResourceSystem::Resource
	{
	public:

		virtual ~TextResource(void);

		/// Factory method.
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// Returns pointer to parsed the text data.
		const TextData* GetTextDataPtr(const StringKey& key);

		/// Returns the text data.
		/// Note that this may be slow if strings are long. Returning a ptr should be preffered.
		const TextData GetTextData(const StringKey& key);

		/// Returns the text data container used by this resource.
		const TextDataMap* GetTextDataMap(void);
		
		/// Returns the name of group to which the text data belongs.
		inline const string& GetGroupName(void) { return mGroupName; }

		/// Returns the resource type associated with this class.
		static ResourceSystem::eResourceType GetResourceType() { return ResourceSystem::RESTYPE_TEXTRESOURCE; }
		
		/// Text item with this key which is the first in the file contains the group name.
		static const string GroupNameKey;
		
		/// Register the callback for reloading the text resource
		static void SetUnloadCallback(TextResourceUnloadCallback callback);

	protected:

		virtual size_t LoadImpl(void);
		virtual bool UnloadImpl(void);

	private:

		/// Container used to store the parsed data.
		TextDataMap mTextDataMap;
		
		/// Name of group to which the text data belongs.
		string mGroupName;
		
		/// Callback for reloading the text resource
		static TextResourceUnloadCallback mUnloadCallback;

	};
}

#endif