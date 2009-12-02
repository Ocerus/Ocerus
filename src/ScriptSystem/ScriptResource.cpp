#include "Common.h"
#include "ScriptResource.h"

using namespace ScriptSystem;

ScriptResourceReloadCallback ScriptResource::mReloadCallback;

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
	return true;
}

const char* ScriptResource::GetScript()
{
	EnsureLoaded();
	return mScript.c_str();
}

void ScriptResource::SetReloadCallback(ScriptResourceReloadCallback callback)
{
	ScriptResource::mReloadCallback = callback;
}

void ScriptResource::Reload(void)
{
	if (ScriptResource::mReloadCallback != 0) ScriptResource::mReloadCallback(this);
	set<string>::iterator iter;
	while ((iter = mDependentModules.begin()) != mDependentModules.end())
	{
		gScriptMgr.ReloadModule(iter->c_str());	
	}
	ResourceSystem::Resource::Reload();
}