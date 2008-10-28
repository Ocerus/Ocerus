#ifndef _WINDOW_MGR_H
#define _WINDOW_MGR_H

#define gWindowMgr GUISystem::GUIMgr::WindowMgr::GetSingleton()

namespace GUIMgr {
	class WindowMgr : public Singleton<WindowMgr>
	{
	public:
		static const String GeneratedWindowNameBase;      //!< Base name to use for generated window names.
		typedef bool PropertyCallback(Window* Window, String& PropName, String& PropValue, void* UserData);
		Window* CreateWindow(const String& Type, const String& Name = "", const String& Prefix = "");
		void DestroyWindow(Window* Window);
		void DestroyWindow(const String& Window);
		Window*	GetWindow(const String& Name) const;
		bool IsWindowPresent(const String& Name) const;
		void DestroyAllWindows();
		Window*	LoadWindowLayout(const String& FileName, const String& NamePrefix = "", const String& ResourceGroup = "",
			PropertyCallback* Callback = 0, void* UserData = 0);
		Window*	LoadWindowLayout(const String& FileName, bool GenerateRandomPrefix);
		bool IsDeadPoolEmpty() const;
		void CleanDeadPool();
		void WriteWindowLayoutToStream(const Window& Window, OutStream& OStream, bool WriteParent = false) const;
		void WriteWindowLayoutToStream(const String& window, OutStream& OStream, bool WriteParent = false) const;
		void RenameWindow(const String& Window, const String& NewName);
		void RenameWindow(Window* Window, const String& NewName);
		static const String& GetDefaultResourceGroup();
		static void SetDefaultResourceGroup(const String& ResourceGroup);

		typedef	ConstBaseIterator<WindowRegistry> WindowIterator;

		WindowIterator GetIterator() const;

		void DEBUG_dumpWindowNames(String zone); 

		WindowMgr();
		virtual ~WindowMgr();
	};
}
#endif
