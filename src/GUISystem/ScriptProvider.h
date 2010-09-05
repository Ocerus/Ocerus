/// @file
/// Interconnects CEGUI with Ocerus script system.
#ifndef _GUISYSTEM_SCRIPTPROVIDER_H_
#define _GUISYSTEM_SCRIPTPROVIDER_H_

#include "Base.h"
#include "CEGUIForwards.h"
#include <CEGUIScriptModule.h>

namespace GUISystem
{
	/// Implements the callback of GUI events as a calling script function.
	class ScriptCallback
	{
	public:
		/// Constructor of the script callback class.
		/// @param functionName Name of the script function to call.
		ScriptCallback(const string& functionName) : mFunctionName(functionName) {}

		/// This method is called as a callback to a GUI event.
		bool operator()(const CEGUI::EventArgs& args) const;

	private:
		/// Name of the script function to call.
		string mFunctionName;

	};

	/// Implements scripting in the CEGUI library.
	class ScriptProvider: public CEGUI::ScriptModule
	{
	public:

		/// Subscribes the named Event to a scripted funtion. 
		virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, const CEGUI::String& subscriber_name);

		/// Subscribes the specified group of the named Event to a scripted funtion. 
		virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name);

		/// @name Unsupported methods
		/// Those methods are only present to satisfy the interface. This functionality is not necessary, so they have
		/// empty implementation.
		//@{
		virtual void executeScriptFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup = "")
		{
			OC_UNUSED(filename); OC_UNUSED(resourceGroup);
		}

		virtual int executeScriptGlobal(const CEGUI::String& function_name)
		{
			OC_UNUSED(function_name);
			return 0;
		}

		virtual void executeString(const CEGUI::String &str)
		{
			OC_UNUSED(str);
		}

		virtual bool executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs& e);
		//@}
	};
}

#endif // _GUISYSTEM_SCRIPTPROVIDER_H_
