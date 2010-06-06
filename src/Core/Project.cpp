#include "Common.h"
#include "Project.h"
#include "Editor/EditorMgr.h"

using namespace Core;

Project::Project(): mIsOpened(false), mConfig(0)
{
}

Project::~Project()
{
	CloseProject();
	delete mConfig;
}

bool Project::CreateProject(const string& path)
{
	string configFile = path + "/project.ini";
	if (!boost::filesystem::is_directory(path) && !boost::filesystem::create_directory(path))
	{
		ocWarning << "Cannot create project directory " << path << ".";
		return false;
	}

	if (boost::filesystem::exists(configFile))
	{
		ocWarning << "Cannot create project " << path << ". Already exists!";
		return false;
	}

	CloseProject();
	mPath = path;
	mIsOpened = true;
	mConfig = new Core::Config(configFile);
	SetDefaultProjectInfo();
	SaveProject();

	ocInfo << "Created new project at " << path;
	return true;
}

bool Project::OpenProject(const string& path, bool editorSupport)
{
	string configFile = path + "/project.ini";
	if (!boost::filesystem::is_directory(path) || !boost::filesystem::exists(configFile))
	{
		ocWarning << "Cannot open project " << path << ".";
		return false;
	}

	CloseProject();
	mPath = path;
	mIsOpened = true;
	mConfig = new Core::Config(configFile);

	// Load resource types
	vector<string> resourceNames;
	mConfig->GetSectionKeys("resource-types", resourceNames);
	for (vector<string>::const_iterator it = resourceNames.begin(); it != resourceNames.end(); ++it)
	{
		mResourceTypeMap[*it] = ResourceSystem::GetResourceTypeFromString(mConfig->GetString(*it, "", "resource-types"));
	}

	// Load scenes
	vector<string> sceneFiles;
	mConfig->GetSectionKeys("scenes", sceneFiles);
	for (vector<string>::const_iterator it = sceneFiles.begin(); it != sceneFiles.end(); ++it)
	{
		mScenes.push_back(Containers::make_pair(*it, mConfig->GetString(*it, "", "scenes")));
	}

	// Make sure basePath is terminated with slash.
	string basePath = path;
	if (!basePath.empty() && basePath.at(basePath.size() - 1) != '/')
		basePath.append("/");

	gResourceMgr.SetBasePath(ResourceSystem::BPT_PROJECT, basePath);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_PROJECT, "", "Project", ".*", "", ResourceSystem::RESTYPE_AUTODETECT, mResourceTypeMap);

	if (editorSupport)
	{
		gEntityMgr.LoadPrototypes();
		gEditorMgr.RefreshPrototypeWindow();
		gEditorMgr.RefreshResourceWindow();
		gEditorMgr.UpdateSceneMenu();
	}

	ocInfo << "Loaded project from " << path;

	OpenDefaultScene();

	return true;
}

void Project::CloseProject()
{
	if (!IsProjectOpened())
		return;

	SaveProject();
	mPath = "";
	mIsOpened = false;
	mResourceTypeMap.clear();
	mScenes.clear();
	gResourceMgr.DeleteGroup("Project");

	ocInfo << "Closed project";
}

void Project::GetScenes(Project::Scenes& scenes) const
{
	scenes = mScenes;
}

void Project::OpenScene(const string& scene)
{
	mCurrentScene = scene;
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Project", scene));
}

void Project::OpenSceneAtIndex(uint32 index)
{
	if (index >= mScenes.size())
	{
		ocError << "Invalid scene at " << index;
		return;
	}
	OpenScene(mScenes[index].first);
}


void Project::SetDefaultProjectInfo()
{
	mInfo.name = "Untitled";
	mInfo.author = "";
	mInfo.version = "1.0";
}

void Project::SaveProject()
{
	OC_ASSERT(mConfig);
	mConfig->SetString("name", mInfo.name);
	mConfig->SetString("author", mInfo.author);
	mConfig->SetString("version", mInfo.version);
	for (ResourceSystem::ResourceTypeMap::const_iterator it = mResourceTypeMap.begin(); it != mResourceTypeMap.end(); ++it)
	{
		mConfig->SetString(it->first, ResourceSystem::GetResourceTypeName(it->second), "resource-types");
	}

	for (Scenes::const_iterator it = mScenes.begin(); it != mScenes.end(); ++it)
	{
		mConfig->SetString(it->first, it->second, "scenes");
	}

	mConfig->Save();

	ocInfo << "Project was saved to " << mPath;
}


string Core::Project::GetDefaultSceneName() const
{
	OC_ASSERT(mScenes.size() > 0);
	return mScenes.front().first;
}

void Core::Project::OpenDefaultScene()
{
	if (GetScenesCount() == 0)
	{
		ocWarning << "No default scene to open";
		return;
	}
	OpenSceneAtIndex(0);
}

uint32 Core::Project::GetScenesCount() const
{
	return mScenes.size();
}