#ifndef _SCHEME_MGR_H
#define _SCHEME_MGR_H

#include "../ResourceSystem/ResourceMgr.h"

#define gSchemeMgr GUISystem::GUIMgr::SchemeMgr::GetSingleton()

namespace GUIMgr {
	class SchemeMgr : public Singleton<SchemeMgr>
	{
	public:
		Scheme*	LoadScheme(const String& SchemeFilename, const String& ResourceGroup = "");
		void UnloadScheme(const String& SchemeName);
		bool IsSchemePresent(const String& SchemeName) const;
		Scheme*	GetScheme(const String& Name) const;
		void UnloadAllSchemes();

		typedef	ConstBaseIterator<SchemeRegistry> SchemeIterator;
		SchemeIterator GetIterator(void) const;

		SchemeMgr();
		virtual ~SchemeMgr();
	};
}
#endif