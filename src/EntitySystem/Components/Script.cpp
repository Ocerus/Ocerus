#include "Common.h"
#include "Script.h"
#include <angelscript.h>
#include "Core/Game.h"

using namespace EntityComponents;
using namespace EntitySystem;


void Script::Create(void)
{
	mNeedUpdate = true;
	mIsUpdating = false;
	mTimeOut = 1000;
}

void Script::Destroy(void)
{

}

bool Script::ExecuteScriptFunction(int32 funcId)
{
  bool res = true;
  // Return new context prepared to call function from module
  AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
  if (ctx == 0) { return false; }
  // Set parent entity handle as context user data
  ctx->SetUserData(GetOwnerPtr());
  // Execute script with time out
  if (!gScriptMgr.ExecuteContext(ctx, mTimeOut)) { res = false; }
  // Release context
  ctx->Release();
  return res;
}

void Script::UpdateMessageHandlers(void)
{
	mIsUpdating = true;
	// Get a set of new and removed modules
	set<string> newModules;
	for (int32 i = 0; i < mModules.GetSize(); ++i) { newModules.insert(mModules[i]); }
	set<string> removedModules;
	for (set<string>::iterator it = mOldModules.begin(); it != mOldModules.end(); ++it)
	{
	  if (newModules.find(*it) == newModules.end()) { removedModules.insert(*it); }
	}
	
	// Call OnDestroy() function on removed modules
	multimap<EntitySystem::EntityMessage::eType, int32>::const_iterator destroyIt
	  = mMessageHandlers.find(EntitySystem::EntityMessage::DESTROY);
	for (; destroyIt != mMessageHandlers.end() && destroyIt->first == EntitySystem::EntityMessage::DESTROY; ++destroyIt)
	{
	  string moduleName = gScriptMgr.GetFunctionModuleName(destroyIt->second);
	  if (removedModules.find(moduleName) != removedModules.end()) { ExecuteScriptFunction(destroyIt->second); }
	}
	
	mMessageHandlers.clear();

	// Variables for refreshing mStates, mTimes, mModuleToFuncID and mFuncIDToArrayIndex
	Utils::Array<int32> newStates;
	Utils::Array<uint64> newTimes;
  map<string, int32> newModuleToFuncID;
	map<int32, int32> newFuncIDToArrayIndex;

	for (int32 i = 0; i < mModules.GetSize(); ++i) // For each module in mModules
	{
		for (int32 type = 0; type < EntityMessage::NUM_TYPES; ++type) // For each type of message
		{
			// Get function ID from module name and function declaration
			int32 funcId = gScriptMgr.GetFunctionID(mModules[i].c_str(),
				EntityMessage::GetHandlerDeclaration(EntityMessage::eType(type)));
			if (funcId >= 0) // Function is found
			{
				// Map message type to found function ID
				mMessageHandlers.insert(pair<EntityMessage::eType, int32>
					(EntityMessage::eType(type), funcId));
				// Special action for CHECK_ACTION messages
				if (type == EntityMessage::CHECK_ACTION)
				{
					// Find index of member of mStates and mTimes which holds data for function in module
					int32 copyFromIndex = -1;
					map<string, int32>::iterator mfit = mModuleToFuncID.find(mModules[i]);
					if (mfit != mModuleToFuncID.end())
					{
						map<int32, int32>::iterator fait = mFuncIDToArrayIndex.find(mfit->second);
						if (fait != mFuncIDToArrayIndex.end())
						{
							copyFromIndex = fait->second;
						}
					}
					// Create new data for found function and map module to function ID and function ID to array index
					int32 newIndex = newStates.GetSize();
					newModuleToFuncID[mModules[i]] = funcId;
					newFuncIDToArrayIndex[funcId] = newIndex;
					newStates.Resize(newIndex + 1);
					newTimes.Resize(newIndex + 1);
					// Copy data from old arrays or initialize it
					newStates[newIndex] = (copyFromIndex != -1) ? mStates[copyFromIndex] : 0;
					newTimes[newIndex] = (copyFromIndex != -1) ? mTimes[copyFromIndex] : 0;
				}
			}
		}
	}

	// Refresh mStates, mTimes, mModuleToFuncID and mFuncIDToArrayIndex with new values
	mStates.CopyFrom(newStates);
	mTimes.CopyFrom(newTimes);
	mModuleToFuncID = newModuleToFuncID;
	mFuncIDToArrayIndex = newFuncIDToArrayIndex;
	
	// Get a set of added modules
	set<string> addedModules;
	for (int32 i = 0; i < mModules.GetSize(); ++i)
	{ 
	  if (mOldModules.find(mModules[i]) == mOldModules.end()) { addedModules.insert(mModules[i]); }
	}
	
	// Call OnInit() and OnPostInit() functions on added modules if the entity is already fully initialized
	if (GetOwner().IsInited())
	{
	  multimap<EntitySystem::EntityMessage::eType, int32>::const_iterator initIt
	    = mMessageHandlers.find(EntitySystem::EntityMessage::INIT);
	  for (; initIt != mMessageHandlers.end() && initIt->first == EntitySystem::EntityMessage::INIT; ++initIt)
	  {
	    string moduleName = gScriptMgr.GetFunctionModuleName(initIt->second);
	    if (addedModules.find(moduleName) != addedModules.end()) { ExecuteScriptFunction(initIt->second); }
	  }
	  
	  initIt = mMessageHandlers.find(EntitySystem::EntityMessage::POST_INIT);
	  for (; initIt != mMessageHandlers.end() && initIt->first == EntitySystem::EntityMessage::POST_INIT; ++initIt)
	  {
	    string moduleName = gScriptMgr.GetFunctionModuleName(initIt->second);
	    if (addedModules.find(moduleName) != addedModules.end()) { ExecuteScriptFunction(initIt->second); }
	  }
	}
	
	// Refresh a module history
	mOldModules = newModules;
	
	mNeedUpdate = false;
	mIsUpdating = false;
}

EntityMessage::eResult Script::HandleMessage(const EntityMessage& msg)
{
	// When the script component is being destroyed we must call OnDestroy() in all modules.
	if (msg.type == EntityMessage::COMPONENT_DESTROYED && msg.parameters.GetParametersCount() == 1
	  && (*msg.parameters.GetParameter(0).GetData<uint32>()) == (uint32)EntitySystem::CT_Script)
	{
	  mModules.Clear();
	  if (!mIsUpdating) { UpdateMessageHandlers(); }
	  return EntityMessage::RESULT_OK;
	}
	
	// Updating message handlers if necessary
	if (msg.type == EntityMessage::RESOURCE_UPDATE)
	{
		mNeedUpdate = true;
		return EntityMessage::RESULT_OK;
	} else if (mNeedUpdate && !mIsUpdating) { UpdateMessageHandlers(); }
	
	// Find all associated functions and execute it
	multimap<EntitySystem::EntityMessage::eType, int32>::const_iterator it = mMessageHandlers.find(msg.type);
	if (it == mMessageHandlers.end()) return EntityMessage::RESULT_IGNORED;
	EntityMessage::eResult res = EntityMessage::RESULT_OK;
	for (; it != mMessageHandlers.end() && it->first == msg.type; ++it)
	{
		// Get function ID
		int32 funcId = it->second;
		// Special action for CHECK_ACTION messages 
		if (msg.type == EntityMessage::CHECK_ACTION)
		{
			mCurrentArrayIndex = mFuncIDToArrayIndex[funcId];
			// Check whether is time to run OnAction script
			if (mTimes[mCurrentArrayIndex] > gScriptMgr.GetTime()) { continue; }
		} else mCurrentArrayIndex = -1;
		// Return new context prepared to call function from module
		AngelScript::asIScriptContext* ctx = gScriptMgr.PrepareContext(funcId);
		if (ctx == 0) { continue; }
		// Set parent entity handle as context user data
		ctx->SetUserData(GetOwnerPtr());

		// Add additional parameters depended on message type
		for (uint32 i=0; i<msg.parameters.GetParametersCount(); ++i)
		{
			bool result = gScriptMgr.SetFunctionArgument(ctx, i, msg.parameters.GetParameter(i));
			OC_ASSERT_MSG(result, "Can't set script function argument; check EntityMessageTypes.h");
		}

		// Execute script with time out
		if (!gScriptMgr.ExecuteContext(ctx, mTimeOut)) { res = EntityMessage::RESULT_ERROR; }
		// Release context
		ctx->Release();
	}
	return res;
}

void Script::RegisterReflection()
{
	RegisterProperty<Array<string>*>("ScriptModules", &Script::GetModules, &Script::SetModules, PA_INIT | PA_EDIT_READ | 
		PA_EDIT_WRITE | PA_SCRIPT_READ, "Names of the script modules that are searched for script message handlers.");
	RegisterProperty<uint32>("ScriptTimeOut", &Script::GetTimeOut, &Script::SetTimeOut, 
		PA_INIT | PA_EDIT_READ | PA_EDIT_WRITE | PA_SCRIPT_READ, "Maximum time of execution the scripts in ms (0 means infinity).");
	RegisterProperty<Array<int32>*>("ScriptStates", &Script::GetStates, &Script::SetStates, PA_INIT | PA_EDIT_READ | 
		PA_EDIT_WRITE, "States of Action handlers");
	RegisterProperty<Array<uint64>*>("ScriptTimes", &Script::GetTimes, &Script::SetTimes, PA_INIT | PA_EDIT_READ | 
		PA_EDIT_WRITE, "Times of execution of Action handlers");
	RegisterProperty<int32>("ScriptCurrentArrayIndex", &Script::GetCurrentArrayIndex, 0, 
		PA_INIT, "Current index of ScriptStates and ScriptTimes");
}
