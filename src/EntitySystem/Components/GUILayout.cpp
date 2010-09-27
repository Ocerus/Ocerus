#include "Common.h"
#include "GUILayout.h"
#include "GUISystem/GUIMgr.h"
#include "Core/Game.h"
#include "CEGUI.h"
#include <angelscript.h>

void EntityComponents::GUILayout::Create(void)
{
	mRootWindow = 0;
	mScriptUpdateError = false;
}

void EntityComponents::GUILayout::Destroy(void)
{
	if (mRootWindow) gGUIMgr.DestroyWindowDirectly(mRootWindow);
}

void EntityComponents::GUILayout::ReloadWindow(void)
{
	if (mRootWindow) gGUIMgr.DestroyWindowDirectly(mRootWindow);
	if (mLayout)
	{
		mRootWindow = gGUIMgr.LoadProjectLayout(mLayout->GetName(), ScriptSystem::USER_GUI_WINDOWS_PREFIX);
		if (mRootWindow)
		{
			CEGUI::Window* parentWindow = GlobalProperties::Get<Core::Game>("Game").GetRootWindow();
			if (parentWindow)
			{
				parentWindow->addChildWindow(mRootWindow);
			}
			mRootWindow->setVisible(mVisible);
			mRootWindow->setUserData(GetOwnerPtr());
			mRootWindow->setUserString("RootWindow", "");
		}
	}
}

EntityMessage::eResult EntityComponents::GUILayout::HandleMessage(const EntityMessage& msg)
{
	if (gEntityMgr.IsEntityPrototype(GetOwner())) return EntityMessage::RESULT_IGNORED;

	// send the message to the script
	if (msg.type == EntityMessage::INIT || msg.type == EntityMessage::POST_INIT || msg.type == EntityMessage::UPDATE_LOGIC)
	{
		if (mCallback && !mScriptUpdateError && mVisible)
		{
			// Get function ID from module name and function declaration
			int32 funcId = gScriptMgr.GetFunctionID(mCallback->GetName().c_str(), 
				EntityMessage::GetHandlerDeclaration(msg.type));
			if (funcId >= 0) // Function is found
			{
				// Return new context prepared to call function from module
				AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
				if (ctx == 0) { mScriptUpdateError = true; return EntityMessage::RESULT_ERROR; }
				// Set parent entity handle as context user data
				ctx->SetUserData(GetOwnerPtr());
				// Execute script with time out
				if (!gScriptMgr.ExecuteContext(ctx, 1000)) { mScriptUpdateError = true; }
				// Release context
				ctx->Release();
			}
		}
	}

	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			ReloadWindow();

			return EntityMessage::RESULT_OK;
		}
	case EntityMessage::RESOURCE_UPDATE:
		{
			mScriptUpdateError = false;
			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::GUILayout::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Layout", &GUILayout::GetLayout, &GUILayout::SetLayout, PA_FULL_ACCESS, "Layout used for defining the GUI.");
	RegisterProperty<ResourceSystem::ResourcePtr>("Callback", &GUILayout::GetCallback, &GUILayout::SetCallback, PA_FULL_ACCESS, "Script module with functions used as a callback for GUI events.");
	RegisterProperty<bool>("Visible", &GUILayout::GetVisible, &GUILayout::SetVisible, PA_FULL_ACCESS, "Whether the GUI layout is visible.");
}

void EntityComponents::GUILayout::SetLayout(ResourceSystem::ResourcePtr value)
{ 
	static const string layoutExt = ".layout";
	if (value && value->GetType() == ResourceSystem::RESTYPE_CEGUIRESOURCE 
		&& value->GetName().rfind(layoutExt) == value->GetName().length() - layoutExt.length())
	{
		mLayout = value;
		if (GetOwner().IsInited() && !gEntityMgr.IsEntityPrototype(GetOwner())) ReloadWindow();
	}
}

void EntityComponents::GUILayout::SetCallback(ResourceSystem::ResourcePtr value)
{ 
	if (value && value->GetType() == ResourceSystem::RESTYPE_SCRIPTRESOURCE)
	{
		mCallback = value;
		mScriptUpdateError = false;
	}
}

void EntityComponents::GUILayout::SetVisible(bool value)
{ 
	mVisible = value;
	if (mRootWindow) mRootWindow->setVisible(mVisible);
}