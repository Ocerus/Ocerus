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
	if (mRootWindow) gGUIMgr.DestroyWindow(mRootWindow);
}

void EntityComponents::GUILayout::ReloadWindow(void)
{  
	if (mRootWindow) gGUIMgr.DestroyWindow(mRootWindow);
	if (mLayout)
	{
		if (mScheme) gGUIMgr.LoadProjectScheme(mScheme->GetName());
		mRootWindow = gGUIMgr.LoadProjectLayout(mLayout->GetName(), ScriptSystem::USER_GUI_WINDOWS_PREFIX);
		if (mRootWindow)
		{
			CEGUI::Window* parentWindow = GlobalProperties::Get<Core::Game>("Game").GetRootWindow();
			if (parentWindow)
			{
				parentWindow->addChildWindow(mRootWindow);
			}
			mRootWindow->setVisible(mVisible);
			mRootWindow->setEnabled(mEnabled); 
			mRootWindow->setUserData(GetOwnerPtr());
			mRootWindow->setUserString("RootWindow", "");
		}
	}
}

EntityMessage::eResult EntityComponents::GUILayout::HandleMessage(const EntityMessage& msg)
{
	if (mCallback && (((msg.type == EntityMessage::INIT || msg.type == EntityMessage::POST_INIT) 
		&& !gEntityMgr.IsEntityPrototype(GetOwner())) 
		|| (msg.type == EntityMessage::UPDATE_LOGIC && !mScriptUpdateError && mVisible)
		|| ((msg.type == EntityMessage::KEY_PRESSED || msg.type == EntityMessage::KEY_RELEASED) && mEnabled)))
	{
		// Get function ID from module name and function declaration
		int32 funcId = gScriptMgr.GetFunctionID(mCallback->GetName().c_str(),
			EntityMessage::GetHandlerDeclaration(msg.type));
		if (funcId >= 0) // Function is found
		{
			// Return new context prepared to call function from module
			AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
			if (ctx != 0)
			{
				// Set parent entity handle as context user data
				ctx->SetUserData(GetOwnerPtr());
				// Add additional parameters depended on message type
				for (uint32 i = 0; i < msg.parameters.GetParametersCount(); ++i)
				{
					bool result = gScriptMgr.SetFunctionArgument(ctx, i, msg.parameters.GetParameter(i));
					OC_ASSERT_MSG(result, "Can't set script function argument; check EntityMessageTypes.h");
				}
				// Execute script with time out
				if (!gScriptMgr.ExecuteContext(ctx, 1000) && msg.type == EntityMessage::UPDATE_LOGIC) { mScriptUpdateError = true; }
				// Release context
				ctx->Release();
			} else if (msg.type == EntityMessage::UPDATE_LOGIC) { mScriptUpdateError = true; }
		}
	}
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if (!gEntityMgr.IsEntityPrototype(GetOwner())) ReloadWindow();	
			return EntityMessage::RESULT_OK;
		}
	case EntityMessage::POST_INIT:
	case EntityMessage::UPDATE_LOGIC:
	case EntityMessage::KEY_PRESSED:
	case EntityMessage::KEY_RELEASED:
		{
			return EntityMessage::RESULT_OK;
		}
	case EntityMessage::RESOURCE_UPDATE:
		{
			mScriptUpdateError = false;
			HandleMessage(EntityMessage(EntityMessage::INIT));
			HandleMessage(EntityMessage(EntityMessage::POST_INIT));
			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::GUILayout::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Layout", &GUILayout::GetLayout, &GUILayout::SetLayout, PA_FULL_ACCESS, "Layout used for defining the GUI.");
	RegisterProperty<ResourceSystem::ResourcePtr>("Scheme", &GUILayout::GetScheme, &GUILayout::SetScheme, PA_FULL_ACCESS, "Scheme used for defining the GUI.");
	RegisterProperty<ResourceSystem::ResourcePtr>("Callback", &GUILayout::GetCallback, &GUILayout::SetCallback, PA_FULL_ACCESS, "Script module with functions used as a callback for GUI events.");
	RegisterProperty<bool>("Visible", &GUILayout::GetVisible, &GUILayout::SetVisible, PA_FULL_ACCESS, "Whether the GUI layout is visible.");
	RegisterProperty<bool>("Enabled", &GUILayout::GetEnabled, &GUILayout::SetEnabled, PA_FULL_ACCESS, "Whether the GUI layout reacts on an user input.");
}

void EntityComponents::GUILayout::SetLayout(ResourceSystem::ResourcePtr value)
{ 
	static const string layoutExt = ".layout";
	if (value && value->GetType() == ResourceSystem::RESTYPE_CEGUIRESOURCE 
		&& value->GetName().rfind(layoutExt) == value->GetName().length() - layoutExt.length())
	{
		mLayout = value;
		if (GetOwner().IsInited() && !gEntityMgr.IsEntityPrototype(GetOwner()))
		{
			HandleMessage(EntityMessage(EntityMessage::INIT));
			HandleMessage(EntityMessage(EntityMessage::POST_INIT));
		}
	}
}

void EntityComponents::GUILayout::SetScheme(ResourceSystem::ResourcePtr value)
{ 
	static const string layoutExt = ".scheme";
	if (value && value->GetType() == ResourceSystem::RESTYPE_CEGUIRESOURCE 
		&& value->GetName().rfind(layoutExt) == value->GetName().length() - layoutExt.length())
	{
		mScheme = value;
		if (GetOwner().IsInited() && !gEntityMgr.IsEntityPrototype(GetOwner()))
		{
			HandleMessage(EntityMessage(EntityMessage::INIT));
			HandleMessage(EntityMessage(EntityMessage::POST_INIT));
		}
	}
}

void EntityComponents::GUILayout::SetCallback(ResourceSystem::ResourcePtr value)
{ 
	if (value && value->GetType() == ResourceSystem::RESTYPE_SCRIPTRESOURCE)
	{
		mCallback = value;
		mScriptUpdateError = false;
		if (GetOwner().IsInited() && !gEntityMgr.IsEntityPrototype(GetOwner()))
		{
			HandleMessage(EntityMessage(EntityMessage::INIT));
			HandleMessage(EntityMessage(EntityMessage::POST_INIT));
		}
	}
}

void EntityComponents::GUILayout::SetVisible(bool value)
{ 
	mVisible = value;
	if (mRootWindow)
	{
		mRootWindow->setVisible(mVisible);
		mRootWindow->moveToFront();
	}
}

void EntityComponents::GUILayout::SetEnabled(bool value)
{ 
	mEnabled = value;
	if (mRootWindow)
	{
		mRootWindow->setEnabled(mEnabled);
	}
}