#ifndef _WIDGETLOOK_MGR_H
#define _WIDGETLOOK_MGR_H

#define gWidgetLookMgr GUISystem::GUIMgr::WidgetLookMgr::GetSingleton()

namespace GUIMgr {
	class WidgetLookMgr : public Singleton<WidgetLookMgr>
	{
	public:
		static WidgetLookManager& GetSingleton();
		static WidgetLookManager* GetSingletonPtr();
		void ParseLookNFeelSpecification(const String& FileName, const String& ResourceGroup = "");
		bool IsWidgetLookAvailable(const String& Widget) const;
		const WidgetLookFeel& GetWidgetLook(const String& Widget) const;
		void EraseWidgetLook(const String& Widget);
		void AddWidgetLook(const WidgetLookFeel& Look);
		void WriteWidgetLookToStream(const String& Name, OutStream& OStream) const;
		void WriteWidgetLookSeriesToStream(const String& Prefix, OutStream& OStream) const;
		static const String& GetDefaultResourceGroup();
		static void setDefaultResourceGroup(const String& ResourceGroup);
		
		WidgetLookMgr();
		virtual ~WidgetLookMgr();
	};
}
#endif