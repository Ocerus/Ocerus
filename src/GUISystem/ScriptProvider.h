#ifndef _ScriptProvider_H_
#define _ScriptProvider_H_

#include "Base.h"
#include "CEGUIScriptModule.h"

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
    std::string mFunctionName;

  };
  
  /// Implements scripting in the CEGUI library.
  class ScriptProvider: public CEGUI::ScriptModule
  {
  public:
  
    virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target, 
      const CEGUI::String& name, const CEGUI::String& subscriber_name);
   
    virtual CEGUI::Event::Connection subscribeEvent(CEGUI::EventSet* target,
      const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name);
   
    virtual void executeScriptFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup="")
    {}
   
    virtual int executeScriptGlobal(const CEGUI::String& function_name)
    {
      return 0;
    }
   
    virtual void executeString(const CEGUI::String &str)
    {}
   
    virtual bool executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs &e)
    {
      return true;
    }
  };
}

#endif // _ScriptProvider_H_