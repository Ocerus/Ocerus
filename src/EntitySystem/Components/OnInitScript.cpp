#include "Common.h"
#include "OnInitScript.h"
#include <angelscript.h>

using namespace EntityComponents;

void OnInitScript::Create(void)
{
	mOnInitScript.clear();
	mArrayTest = new Array<int32>(3);
	(*mArrayTest)[0] = 10;
	(*mArrayTest)[1] = 11;
	(*mArrayTest)[2] = 12;
}

void OnInitScript::Destroy(void)
{
	delete mArrayTest;
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
	RegisterProperty<string>("OnInitScript", &OnInitScript::GetOnInitScript, &OnInitScript::SetOnInitScript, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<uint32>("OnInitTimeOut", &OnInitScript::GetOnInitTimeOut, &OnInitScript::SetOnInitTimeOut, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<Array<int32>*>("ArrayTest", &OnInitScript::GetArrayTest, 0, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ | PA_SCRIPT_WRITE, "");
	RegisterFunction("TestFunction", &OnInitScript::TestFunction, PA_FULL_ACCESS, "");
}

bool OnInitScript::RunScript()
{
	gScriptMgr.DefineWord("TEST");
	// Get function ID
	int32 funcId = gScriptMgr.GetFunctionID(mOnInitScript.c_str(), "void OnInit(EntityHandle handle)");
	if (funcId < 0) return false;
	// Return new context prepared to call function from module
	AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
	if (ctx == 0) return false;
	// Set parent entity handle as first argument
	int32 r = ctx->SetArgObject(0, GetOwnerPtr());
	OC_ASSERT(r >= 0);
	// Execute script with time out
	bool res = gScriptMgr.ExecuteContext(ctx, mOnInitTimeOut);
	// Release context
	ctx->Release();
	ocInfo << "ArrayTest[2] is " << (*mArrayTest)[2] << ".";
	ocInfo << "ArrayTest[3] is " << (*mArrayTest)[3] << ".";
	return res;
}

void OnInitScript::TestRunTime()
{
	EntitySystem::EntityDescription entDesc;
	entDesc.Init();
	entDesc.SetName("Test entity");
	entDesc.AddComponent(CT_ON_INIT_SCRIPT);

	EntitySystem::EntityHandle handle = gEntityMgr.CreateEntity(entDesc);
	handle.GetProperty("OnInitScript").SetValue<string>("TestOnInitScript.as");
	handle.GetProperty("OnInitTimeOut").SetValue<uint32>(100000);
	// Calls script
	handle.FinishInit();
}
