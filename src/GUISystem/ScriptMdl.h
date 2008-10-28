#ifndef _SCRIPT_MDL_H
#define _SCRIPT_MDL_H

#define gScriptMdl GUISystem::GUIMgr::ScriptMdl::GetSingleton()

namespace ScriptMdl {
	class ScriptMdl : public Singleton<ScriptMdl>
	{
	public:
		virtual	void ExecuteScriptFile(const String& FileName, const String& ResourceGroup = "") = 0;
		virtual int ExecuteScriptGlobal(const String& FunctionName) = 0;
		virtual	bool ExecuteScriptedEventHandler(const String& HandlerName, const EventArgs& e) = 0;
		virtual void ExecuteString(const String& Str) = 0;
		virtual void CreateBindings() {}
		virtual void DestroyBindings() {}
		const String& GetIdentifierString() const;
		const String& GetLanguage() const;
		virtual Event::Connection SubscribeEvent(EventSet* Target, const String& Name, const String& SubscriberName) = 0;
		virtual Event::Connection SubscribeEvent(EventSet* Target, const String& Name, Event::Group Group, const String& SubscriberName) = 0;
		static void SetDefaultResourceGroup(const String& ResourceGroup);
		static const String& GetDefaultResourceGroup();

		ScriptMdl();
		virtual ~ScriptMdl();
	};

	class ScriptFunctor
	{
	public:
		ScriptFunctor(const String& functionName) : scriptFunctionName(functionName) {}
		ScriptFunctor(const ScriptFunctor& obj) : scriptFunctionName(obj.scriptFunctionName) {}
		bool operator()(const EventArgs& e) const;

	private:
		ScriptFunctor& operator=(const ScriptFunctor& rhs);
	};
}
#endif