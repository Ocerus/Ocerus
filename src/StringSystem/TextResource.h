#ifndef _TEXTRESOURCE_H_
#define _TEXTRESOURCE_H_

#include "../ResourceSystem/Resource.h"
#include "../Utility/Settings.h"
#include "../Utility/ResourcePointers.h"
#include "../Utility/StringKey.h"
#include <map>

namespace StringSystem 
{
	/// @name define type used for storing strings
	typedef String TextData;
	/// @name define a container for textdata
	typedef Map<StringKey, TextData> TextDataMap;

	/** This class represents single text resource.
		Main purpose of the class is to load and parse a .str file, not to store actual data. You should
		only use StringMgr to load TextResources (however its possible to load it directly with ResourceMgr,
		IT IS NOT recommended). Also, use StringMgr to access data, because TextRes is normally discarded 
		after loading (StringMgr calls GetTextDataMap() and Unload()), thus it may be required to load it
		again, which is slow.
	*/
	class TextResource : public ResourceSystem::Resource
	{
	public:
		virtual ~TextResource(void);
		static ResourceSystem::ResourcePtr CreateMe(void);

		/// @name return pointer to text data
		const TextData* GetTextDataPtr(const StringKey& key);
		/** Return text data. Note that this may be slow if strings are long. (It has to copy
		    whole TextData structure). Returning a ptr should be preffered way to access data.
		*/
		const TextData GetTextData(const StringKey& key);
		/// @name return textdata map
		const TextDataMap* GetTextDataMap(void);

	protected:	
		/// @name container used to store data
		TextDataMap mTextDataMap;

		/// @name we parse the file as we load it and store the items into TextDataMap
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	
		//friend class StringMgr; // dunno if this is actually necessary
	};
}

#endif