#include "Common.h"
#include "ScriptProvider.h"

#include "CEGUIWindow.h"
#include <angelscript.h>

using namespace GUISystem;

bool ScriptCallback::operator()(const CEGUI::EventArgs &args) const
{
  const CEGUI::WindowEventArgs* argument = static_cast<const CEGUI::WindowEventArgs*>(&args);
  
  CEGUI::Window* rootWindow = argument ? argument->window : 0;
  while (rootWindow != 0 && rootWindow->isUserStringDefined("RootWindow")) rootWindow = rootWindow->getParent();
  
  string module = "GuiCallback.as";
  if (rootWindow)
  {
    EntityHandle* handle = (EntityHandle*)rootWindow->getUserData();
    if (handle != 0 && handle->Exists())
    {
      EntitySystem::ComponentID cmpId = gEntityMgr.GetEntityComponent(*handle, EntitySystem::CT_GUILayout);
      if (cmpId != -1)
      {
        ResourceSystem::ResourcePtr resource = handle->GetComponentProperty(cmpId, "Callback").GetValue<ResourceSystem::ResourcePtr>();
        if (resource)
        {
          module = resource->GetName();
        }
      }
    }
  }
  
  int32 funcId = gScriptMgr.GetFunctionID(module.c_str(), (string("void ")
    + mFunctionName + "(Window@)").c_str());
  if (funcId < 0) return false;
  
  AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
  if (ctx == 0) return false;
  
  ctx->SetArgObject(0, argument ? argument->window : 0);
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

bool ScriptProvider::executeScriptedEventHandler(const CEGUI::String& handler_name, const CEGUI::EventArgs &e)
{
  OC_UNUSED(handler_name);
	OC_UNUSED(e);
	return true;
}