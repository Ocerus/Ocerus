#ifndef _TEXTRESOURCE_H_
#define _TEXTRESOURCE_H_

#include "../ResourceSystem/Resource.h"
#include "../Utility/Settings.h"
//#include "StringKey.h"
#include <map>

namespace StringSystem 
{
	/// define type used for storing strings
	typedef string TextData;
	/// define a container of <key, textdata>
	typedef std::map<string, TextData> TextDataMap;

	class TextResource : public ResourceSystem::Resource
	{
	public:
		virtual ~TextResource(void) {}
		static ResourceSystem::ResourcePtr CreateMe(void);

		TextData GetTextData(const string& key);
		TextDataMap GetTextDataMap();

	protected:	
		TextDataMap mTextDataMap;

		/// we parse the file as we load it and store the items into TextDataMap
		virtual bool LoadImpl(void);
		virtual bool UnloadImpl(void);	
		friend class StringMgr;
	};

	typedef SmartPointer<TextResource> TextResourcePtr;
}

#endif