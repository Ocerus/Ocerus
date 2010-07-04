#include "Common.h"
#include "ScriptProvider.h"
#include "CEGUIWindow.h"
#include <angelscript.h>

using namespace GUISystem;

bool ScriptCallback::operator()(const CEGUI::EventArgs &args) const
{
  const CEGUI::WindowEventArgs* argument = static_cast<const CEGUI::WindowEventArgs*>(&args);
  /*string windowName;
  
  if (argument)
  {
    windowName = argument->window->getName().c_str();
  }*/
  
  int32 funcId = gScriptMgr.GetFunctionID("GuiCallback.as", (string("void ")
    + mName + "(Window@)").c_str());
  if (funcId < 0) return false;
  
  AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
  if (ctx == 0) return false;
  
  ctx->SetArgObject(0, argument ? argument->window : 0);
  //gScriptMgr.SetFunctionArgument(ctx, 0, Reflection::PropertyFunctionParameter(windowName));
  gScriptMgr.ExecuteContext(ctx, 1000);
  ctx->Release();
  
  return true;
}

CEGUI::Event::Connection ScriptProvider::subscribeEvent(CEGUI::EventSet* target, 
  const CEGUI::String& name, const CEGUI::String& subscriber_name)
{
  ScriptCallback callback(subscriber_name.c_str());
  return target->subscribeEvent(name, CEGUI::Event::Subscriber(callback));
}

CEGUI::Event::Connection ScriptProvider::subscribeEvent(CEGUI::EventSet* target,
  const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name)
{
  ScriptCallback callback(subscriber_name.c_str());
  return target->subscribeEvent(name, group, CEGUI::Event::Subscriber(callback));
}