#include "Common.h"
#include "ProjectMgr.h"
#include "EditorMgr.h"

using namespace Editor;

ProjectMgr::ProjectMgr(): mIsOpened(false), mConfig(0)
{
}

ProjectMgr::~ProjectMgr()
{
	delete mConfig;
}

bool ProjectMgr::CreateProject(const string& path)
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
	return true;
}

bool ProjectMgr::OpenProject(const string& path)
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
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_PROJECT, "", "project", ".*", "", ResourceSystem::RESTYPE_AUTODETECT, mResourceTypeMap);
	gEditorMgr.RefreshResourceWindow();
	gEditorMgr.UpdateSceneMenu();
	return true;
}

void ProjectMgr::CloseProject()
{
	if (!IsProjectOpened())
		return;

	SaveProject();
	mPath = "";
	mIsOpened = false;
	mResourceTypeMap.clear();
	mScenes.clear();
	gResourceMgr.DeleteGroup("project");
}

void ProjectMgr::GetScenes(ProjectMgr::Scenes& scenes) const
{
	scenes = mScenes;
}

void ProjectMgr::OpenScene(const string& scene)
{
	mCurrentScene = scene;
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("project", scene));
}

void ProjectMgr::OpenSceneAtIndex(uint32 index)
{
	if (index >= mScenes.size()) return;
	OpenScene(mScenes[index].first);
}


void ProjectMgr::SetDefaultProjectInfo()
{
	mInfo.name = "Untitled";
	mInfo.author = "";
	mInfo.version = "1.0";
}

void ProjectMgr::SaveProject()
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
}

