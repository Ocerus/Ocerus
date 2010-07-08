#include "Common.h"
#include "Project.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "Core/Game.h"
#include "Core/Application.h"

using namespace Core;

Project::Project(bool editorSupport): mProjectConfig(0), mSceneIndex(-1), mEditorSupport(editorSupport),
  mNeedCloseOpenedScene(false)
{
}

Project::~Project()
{
	CloseProject();
	delete mProjectConfig;
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
	mProjectPath = path;
	mProjectConfig = new Core::Config(configFile);
	SetDefaultProjectInfo();
	SaveProjectConfig();
	ocInfo << "New project at " << path << " created.";
	return true;
}

bool Project::OpenProject(const string& path)
{
	string configFile = path + "/project.ini";
	if (!boost::filesystem::is_directory(path) || !boost::filesystem::exists(configFile))
	{
		ocWarning << "Cannot open project " << path << ".";
		return false;
	}

	CloseProject();
	mProjectPath = path;
	mProjectConfig = new Core::Config(configFile);
	LoadProjectConfig();

	// Make sure basePath is terminated with slash.
	string basePath = path;
	if (!basePath.empty() && basePath.at(basePath.size() - 1) != '/')
		basePath.append("/");

	// Add project resources.
	gResourceMgr.SetBasePath(ResourceSystem::BPT_PROJECT, basePath);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_PROJECT, "", "Project", ".*", "", ResourceSystem::RESTYPE_AUTODETECT, mResourceTypeMap);

	if (mEditorSupport)
	{
		gEntityMgr.LoadPrototypes();
		gEditorMgr.RefreshPrototypeWindow();
		gEditorMgr.RefreshResourceWindow();
		gEditorMgr.UpdateSceneMenu();
	}

	ocInfo << "Project " << path << " loaded.";

	OpenDefaultScene();

	return true;
}

void Project::CloseProject()
{
	if (!IsProjectOpened())
		return;

	CloseOpenedScene();
	SaveProjectConfig();
	delete mProjectConfig;
	mProjectConfig = 0;
	mProjectPath = "";
	mResourceTypeMap.clear();
	mSceneList.clear();
	gResourceMgr.DeleteGroup("Project");

	if (mEditorSupport)
	{
		gEntityMgr.DestroyAllEntities(true, false);
		gEditorMgr.RefreshPrototypeWindow();
		gEditorMgr.RefreshResourceWindow();
		gEditorMgr.UpdateSceneMenu();
	}

	ocInfo << "Project closed.";
}

bool Project::IsProjectOpened() const
{
	return !mProjectPath.empty();
}

string Project::GetOpenedProjectPath() const
{
	return mProjectPath;
}

void Project::GetOpenedProjectInfo(ProjectInfo& projectInfo) const
{
	projectInfo = mProjectInfo;
}

void Project::SetOpenedProjectInfo(const Core::ProjectInfo& newProjectInfo)
{
	mProjectInfo = newProjectInfo;
}

void Project::SetDefaultProjectInfo()
{
	mProjectInfo.name = "Untitled";
	mProjectInfo.author = "";
	mProjectInfo.version = "1.0";
}

void Core::Project::LoadProjectConfig()
{
	OC_DASSERT(mProjectConfig != 0);
	OC_DASSERT(mSceneIndex == -1);

	// Load project info
	mProjectInfo.name = mProjectConfig->GetString("name");
	mProjectInfo.author = mProjectConfig->GetString("author");
	mProjectInfo.version = mProjectConfig->GetString("version");

	// Load resource types
	mResourceTypeMap.clear();
	vector<string> resourceNames;
	mProjectConfig->GetSectionKeys("resource-types", resourceNames);
	for (vector<string>::const_iterator it = resourceNames.begin(); it != resourceNames.end(); ++it)
	{
		mResourceTypeMap[*it] = ResourceSystem::GetResourceTypeFromString(mProjectConfig->GetString(*it, "", "resource-types"));
	}

	// Load scene list
	mSceneList.clear();
	vector<string> sceneFiles;
	mProjectConfig->GetSectionKeys("scenes", sceneFiles);
	for (vector<string>::const_iterator it = sceneFiles.begin(); it != sceneFiles.end(); ++it)
	{
		SceneInfo sceneInfo;
		sceneInfo.filename = *it;
		sceneInfo.name = mProjectConfig->GetString(*it, "", "scenes");
		mSceneList.push_back(sceneInfo);
	}
}

void Core::Project::SaveProjectConfig()
{
	OC_ASSERT(mProjectConfig != 0);

	// Save project info
	mProjectConfig->SetString("name", mProjectInfo.name);
	mProjectConfig->SetString("author", mProjectInfo.author);
	mProjectConfig->SetString("version", mProjectInfo.version);

	// Save resource types
	for (ResourceSystem::ResourceTypeMap::const_iterator it = mResourceTypeMap.begin(); it != mResourceTypeMap.end(); ++it)
	{
		mProjectConfig->SetString(it->first, ResourceSystem::GetResourceTypeName(it->second), "resource-types");
	}

	// Save scene list
	for (SceneInfoList::const_iterator it = mSceneList.begin(); it != mSceneList.end(); ++it)
	{
		mProjectConfig->SetString(it->filename, it->name, "scenes");
	}

	mProjectConfig->Save();
	ocInfo << "Project " << mProjectPath << " saved.";
}




bool Project::CreateScene(const string& sceneFilename, const string& sceneName)
{
	if (!IsProjectOpened() || IsSceneOpened()) return false;

	const string& filename = mProjectPath + "/" + sceneFilename;
	if (boost::filesystem::exists(filename))
	{
		ocWarning << "Cannot create scene '" << filename << "'. Already exists.";
		return false;
	}

	// Create empty scene file.
	{
		ResourceSystem::XMLOutput xmlOutput(filename);
		xmlOutput.BeginElement("Scene");
		xmlOutput.EndElement();
	}

	SceneInfo sceneInfo;
	sceneInfo.name = sceneName;
	sceneInfo.filename = sceneFilename;
	mSceneList.push_back(sceneInfo);
	mSceneIndex = mSceneList.size();

	if (mEditorSupport)
	{
		gEditorMgr.UpdateSceneMenu();
	}

	return true;
}

bool Project::OpenScene(const string& sceneFilename)
{
	if (IsSceneOpened()) return false;
	int sceneIndex = -1;
	for (SceneInfoList::const_iterator it = mSceneList.begin(); it != mSceneList.end(); ++it)
	{
		if (it->filename == sceneFilename)
		{
			sceneIndex = it - mSceneList.begin();
			break;
		}
	}
	return OpenSceneAtIndex(sceneIndex);
}

bool Project::OpenSceneAtIndex(int sceneIndex)
{
	if (IsSceneOpened()) return false;
	if (sceneIndex < 0 || sceneIndex >= (int)mSceneList.size())
		return false;
	mSceneIndex = sceneIndex;
	const string& sceneFilename = mSceneList[sceneIndex].filename;

	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Project", sceneFilename));
	GlobalProperties::Get<Core::Game>("Game").Init();
	if (gApp.IsEditMode()) gEditorMgr.GetEditorGui()->RefreshCameras();
	else GlobalProperties::Get<Core::Game>("Game").CreateDefaultRenderTarget();
	
	ocInfo << "Scene " << sceneFilename << " loaded.";
	return true;
}

bool Project::OpenDefaultScene()
{
	if (GetSceneCount() == 0)
	{
		ocWarning << "No default scene to open.";
		return false;
	}
	return OpenSceneAtIndex(0);
}

bool Project::SaveOpenedScene()
{
	if (mSceneIndex == -1) return false;
	OC_DASSERT(mSceneIndex >= 0 && mSceneIndex < (int)mSceneList.size());
	
	ResourceSystem::ResourcePtr sceneResource = gResourceMgr.GetResource("Project", mSceneList[mSceneIndex].filename);
	ResourceSystem::XMLOutput xmlOutput(sceneResource->GetFilePath());
	xmlOutput.BeginElement("Scene");
	if (!gEntityMgr.SaveEntitiesToStorage(xmlOutput))
	{
		ocError << "Unable to write entities to scene file.";
	}
	xmlOutput.EndElement();
	if (xmlOutput.CloseAndReport())
	{
		ocInfo << "Scene " << mSceneList[mSceneIndex].filename << " saved.";
		return true;
	}
	else
	{
		ocError << "Unable to save scene " << mSceneList[mSceneIndex].filename << ".";
		return false;
	}
}

void Project::CloseOpenedScene()
{
	gEntityMgr.DestroyAllEntities(false, true);
	if (mEditorSupport)
	{
		gEditorMgr.GetEditorGui()->DisableViewports();
	}
	mSceneIndex = -1;
}

void Project::GetSceneList(SceneInfoList& scenes) const
{
	scenes = mSceneList;
}

size_t Project::GetSceneCount() const
{
	return mSceneList.size();
}

string Project::GetOpenedSceneName() const
{
	OC_ASSERT(mSceneIndex >= 0 && mSceneIndex < (int)mSceneList.size());
	return mSceneList[mSceneIndex].name;
}

bool Core::Project::IsSceneOpened() const
{
	return mSceneIndex != -1;
}

void Core::Project::Update()
{
  if (mNeedCloseOpenedScene)
	{
	  if (gApp.IsEditMode())
	  { 
	    gEditorMgr.RestartAction();
	    gEditorMgr.SwitchActionTool(Editor::EditorMgr::AT_RESTART);
	  }
    else CloseOpenedScene();
    mNeedCloseOpenedScene = false;
	}
}
