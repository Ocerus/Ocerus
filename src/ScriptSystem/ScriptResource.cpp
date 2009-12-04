#include "Common.h"
#include "ScriptResource.h"

using namespace ScriptSystem;

ScriptResourceUnloadCallback ScriptResource::mUnloadCallback;

ResourceSystem::ResourcePtr ScriptResource::CreateMe(void)
{
	return ResourceSystem::ResourcePtr(new ScriptResource());
}

ScriptResource::~ScriptResource(void)
{
	UnloadImpl();
}

bool ScriptResource::LoadImpl()
{
	InputStream& is = OpenInputStream(ISM_TEXT);
	stringstream ss;
	ss << is.rdbuf();
	mScript = ss.str();
	CloseInputStream();
	return true;
}

bool ScriptResource::UnloadImpl()
{
	mScript.clear();
	if (ScriptResource::mUnloadCallback != 0) ScriptResource::mUnloadCallback(this);
	set<string>::iterator iter;
	while ((iter = mDependentModules.begin()) != mDependentModules.end())
	{
		string moduleName = (*iter); // it is important to copy string here because iter will be invalidated
		gScriptMgr.UnloadModule(moduleName.c_str());	
	}
	return true;
}

const char* ScriptResource::GetScript()
{
	EnsureLoaded();
	return mScript.c_str();
}

void ScriptResource::SetUnloadCallback(ScriptResourceUnloadCallback callback)
{
	ScriptResource::mUnloadCallback = callback;
}