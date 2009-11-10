#include "Common.h"
#include "OnInitScript.h"
#include <angelscript.h>

using namespace EntityComponents;

void OnInitScript::Create(void)
{
	mOnInitScript.clear();
}

void OnInitScript::Destroy(void)
{

}

EntityMessage::eResult OnInitScript::HandleMessage(const EntityMessage& msg)
{
	switch(msg.type)
	{
	case EntityMessage::POST_INIT:
		return RunScript() ? EntityMessage::RESULT_OK : EntityMessage::RESULT_ERROR;
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void OnInitScript::RegisterReflection()
{
	RegisterProperty<string>("OnInitScript", &OnInitScript::GetOnInitScript, &OnInitScript::SetOnInitScript, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("OnInitTimeOut", &OnInitScript::GetOnInitTimeOut, &OnInitScript::SetOnInitTimeOut, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
}

bool OnInitScript::RunScript()
{
	gScriptMgr.DefineWord("TEST");
	// Return new context prepared to call function from module
	AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(mOnInitScript.c_str(), "void OnInit(EntityHandle handle)");
	if (ctx == 0) return false;
	// Set parent entity handle as first argument
	int r = ctx->SetArgObject(0, GetOwnerPtr());
	OC_ASSERT(r >= 0);
	// Execute script with time out
	bool res = gScriptMgr.ExecuteContext(ctx, mOnInitTimeOut);
	// Release context
	ctx->Release();
	return res;
}

void OnInitScript::TestRunTime()
{
	EntitySystem::EntityDescription entDesc;
	entDesc.Init(ET_TEST);
	entDesc.SetName("Test entity");
	entDesc.AddComponent(CT_ON_INIT_SCRIPT);

	Reflection::PropertyList propList;
	EntitySystem::EntityHandle handle = gEntityMgr.CreateEntity(entDesc, propList);
	handle.GetProperty("OnInitScript").SetValue<string>("TestOnInitScript.as");
	handle.GetProperty("OnInitTimeOut").SetValue<uint32>(1000);
	// Calls script
	handle.FinishInit();
}
