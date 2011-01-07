#include "Common.h"
#include "ScriptProvider.h"
#include "CEGUICommon.h"
#include "Core/Game.h"
#include "Core/Application.h"
#include "Editor/EditorMgr.h"

#include <angelscript.h>

using namespace GUISystem;

bool ScriptCallback::operator()(const CEGUI::EventArgs &args) const
{
	if (gApp.IsEditMode() && gEditorMgr.WasActionRestarted()) return false;
	
	const CEGUI::WindowEventArgs* argument = static_cast<const CEGUI::WindowEventArgs*>(&args);

	CEGUI::Window* rootWindow = argument ? argument->window : 0;
	while (rootWindow != 0 && !rootWindow->isUserStringDefined("RootWindow")) rootWindow = rootWindow->getParent();

	string module = "GuiCallback.as";
	EntitySystem::EntityHandle* handle = 0;
	if (rootWindow)
	{
		handle = (EntitySystem::EntityHandle*)rootWindow->getUserData();
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

	string functionName = string("void ") + mFunctionName + string("(Window@)");
	int32 funcId = gScriptMgr.GetFunctionID(module.c_str(), functionName.c_str());
	if (funcId < 0)
	{
		ocWarning << "Cannot find function " << functionName << " in the GUI callback script file " << module << ".";
		return false;
	}

	AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
	if (ctx == 0) return false;

	if (handle) ctx->SetUserData(handle);
	ctx->SetArgObject(0, argument ? argument->window : 0);
	gScriptMgr.ExecuteContext(ctx, 1000);
	ctx->Release();

	return true;
}

CEGUI::Event::Connection ScriptProvider::subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, const CEGUI::String& subscriber_name)
{
	ScriptCallback callback(subscriber_name.c_str());
	return target->subscribeEvent(name, CEGUI::Event::Subscriber(callback));
}

CEGUI::Event::Connection ScriptProvider::subscribeEvent(CEGUI::EventSet* target, const CEGUI::String& name, CEGUI::Event::Group group, const CEGUI::String& subscriber_name)
{
	ScriptCallback callback(subscriber_name.c_str());
	return target->subscribeEvent(name, group, CEGUI::Event::Subscriber(callback));
}

bool GUISystem::ScriptProvider::executeScriptedEventHandler( const CEGUI::String& handler_name, const CEGUI::EventArgs& e )
{
	OC_UNUSED(handler_name); OC_UNUSED(e);
	return true;
}
