#include "Common.h"
#include "ScriptResource.h"

using namespace ScriptSystem;

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