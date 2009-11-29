#include "Common.h"
#include "Script.h"
#include <angelscript.h>

using namespace EntityComponents;
using namespace EntitySystem;

void Script::Create(void)
{
	mNeedUpdate = true;
}

void Script::Destroy(void)
{

}

void Script::UpdateMessageHandlers(void)
{
	mMessageHandlers.clear();
	for (int32 i=0; i<mModules.GetSize(); ++i) AnalyseModule(mModules[i]);
	mNeedUpdate = false;
}

void Script::AnalyseModule(const string& module)
{
	for (int32 type=0; type<EntityMessage::NUM_TYPES; ++type)
	{
		int32 funcId = gScriptMgr.GetFunctionID(module.c_str(),
			EntityMessage::GetHandleDeclaration(EntityMessage::eType(type)));
		if (funcId >= 0) mMessageHandlers[EntityMessage::eType(type)]=funcId;
	}
}

EntityMessage::eResult Script::HandleMessage(const EntityMessage& msg)
{
	if (mNeedUpdate) UpdateMessageHandlers();
	if (msg.type == EntityMessage::RESOURCE_UPDATE) mNeedUpdate = true;
	// Get function ID
	map<EntitySystem::EntityMessage::eType, int32>::const_iterator it = mMessageHandlers.find(msg.type);
	if (it == mMessageHandlers.end()) return EntityMessage::RESULT_IGNORED;
	int32 funcId = it->second;
	// Return new context prepared to call function from module
	AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
	if (ctx == 0) return EntityMessage::RESULT_IGNORED;
	// Set parent entity handle as context user data
	ctx->SetUserData(GetOwnerPtr());

	// Add additional parameters depended on message type
	switch(msg.type)
	{
	case EntityMessage::INIT: break;
	default: break;
	}

	// Execute script with time out
	bool res = gScriptMgr.ExecuteContext(ctx, mTimeOut);
	// Release context
	ctx->Release();
	return res ? EntityMessage::RESULT_OK : EntityMessage::RESULT_ERROR;
}

void Script::RegisterReflection()
{
	RegisterProperty<Array<string>*>("ScriptModules", &Script::GetModules, 0, PA_INIT | PA_EDIT_READ | 
		PA_EDIT_WRITE | PA_SCRIPT_READ, "Names of the script modules that are searched for script message handlers.");
	RegisterProperty<uint32>("ScriptTimeOut", &Script::GetTimeOut, &Script::SetTimeOut, 
		PA_INIT | PA_EDIT_READ | PA_EDIT_WRITE | PA_SCRIPT_READ, "Maximum time of execution the scripts in ms (0 means infinity).");
}

void Script::TestRunTime()
{
	EntitySystem::EntityDescription entDesc;
	entDesc.Init(ET_TEST);
	entDesc.SetName("Test script entity");
	entDesc.AddComponent(CT_SCRIPT);

	ocError << "testovaci error";

	EntitySystem::EntityHandle handle = gEntityMgr.CreateEntity(entDesc);
	Utils::Array<string>* a = handle.GetProperty("ScriptModules").GetValue<Utils::Array<string>*>();
	a->Resize(1);
	(*a)[0] = "TestScript.as";
	handle.GetProperty("ScriptTimeOut").SetValue<uint32>(1000);
	// Calls script
	handle.FinishInit();
}