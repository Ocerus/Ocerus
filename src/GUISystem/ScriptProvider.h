#ifndef _ScriptProvider_H_
#define _ScriptProvider_H_

#include "Base.h"
#include "CEGUIScriptModule.h"

namespace GUISystem
{
  class ScriptCallback
  {
  public:
    
    ScriptCallback(const string& name) : mName(name) {}
    
    bool operator()(const CEGUI::EventArgs& args) const;
   
  private:
    std::string mName;

  };
  
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