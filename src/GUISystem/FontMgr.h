#ifndef _FONT_MGR_H
#define _FONT_MGR_H

#include "../ResourceSystem/ResourceMgr.h"

#define gFontMgr GUISystem::GUIMgr::FontMgr::GetSingleton()

namespace GUIMgr {
	class FontMgr : public Singleton<FontMgr>
	{
	public:
		Font *CreateFont(const String& FileName, const String& ResourceGroup = "");
		Font *CreateFont (const String &Type, const String& Name, const String& FontName, const String& ResourceGroup = "");
		Font *CreateFont (const String &Type, const XMLAttributes& Attributes);
		void DestroyFont(const String& Name);
		void DestroyFont(Font* Font);
		void DestroyAllFonts();
		bool IsFontPresent(const String& Name) const;
		Font* GetFont(const String& Name) const;
		void NotifyScreenResolution(const Size& Size);
		void WriteFontToStream(const String& Name, OutStream& OStream) const;

		typedef ConstBaseIterator<FontRegistry> FontIterator;
		FontIterator GetIterator () const;

		FontMgr();
		virtual ~FontMgr();
	};
}
#endif
