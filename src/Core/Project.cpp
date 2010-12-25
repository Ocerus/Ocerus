#include "Common.h"
#include "Project.h"
#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/EntityWindow.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/LayerWindow.h"
#include "Editor/ResourceWindow.h"
#include "Core/Game.h"
#include "Core/Application.h"
#include "GUISystem/ViewportWindow.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "Utils/FilesystemUtils.h"

using namespace Core;

const char* Project::PROJECT_FILE_NAME = "project.ini";


Project::Project(bool editorSupport): mProjectConfig(0), mSceneIndex(-1), mRequestSceneIndex(-1), mEditorSupport(editorSupport)
{
}

Project::~Project()
{
	if (IsProjectOpened())
	{
		SaveProjectConfig();
		GlobalProperties::RemovePointer("ProjectConfig");
		delete mProjectConfig;
	}
}

bool Project::CreateProject(const string& name, const string& path)
{
	string configFile = path + "/" + PROJECT_FILE_NAME;
	bool destinationOk = false;
	try
	{
		destinationOk = boost::filesystem::is_directory(path) || boost::filesystem::create_directory(path);
	}
	catch (boost::exception&){}
	if (!destinationOk)
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
	mProjectInfo.name = name;
	CreateDefaultProjectStructure();
	SaveProjectConfig();
	ocInfo << "New project at " << path << " created.";
	return OpenProject(path);
}

bool Project::OpenProject(const string& path)
{
	string configFile = path + "/" + PROJECT_FILE_NAME;
	bool projectOk = false;
	try
	{
		projectOk = boost::filesystem::is_directory(path) && boost::filesystem::exists(configFile);
	}
	catch (boost::exception&)	{}
	if (!projectOk)
	{
		ocWarning << "Cannot open project " << path << ".";
		return false;
	}

	CloseProject();
	mProjectPath = path;
	mProjectConfig = new Core::Config(configFile);
	GlobalProperties::SetPointer("ProjectConfig", mProjectConfig);
	LoadProjectConfig();

	// Make sure basePath is terminated with slash.
	string basePath = path;
	if (!basePath.empty() && basePath.at(basePath.size() - 1) != '/') basePath.append("/");

	// Add project resources.
	gResourceMgr.SetBasePath(ResourceSystem::BPT_PROJECT, basePath);
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_PROJECT, "", "Project", ".*", "", ResourceSystem::RESTYPE_AUTODETECT, mResourceTypeMap);
	gStringMgrProject.LoadLanguagePack();

	// Make sure scene resources are all XML.
	for (size_t i=0; i<GetSceneCount(); ++i)
	{
		ResourceSystem::ResourcePtr res = gResourceMgr.GetResource("Project", mSceneList[i].filename);
		if (res && res->GetType() != ResourceSystem::RESTYPE_XMLRESOURCE)
		{
			SetResourceType(res, ResourceSystem::RESTYPE_XMLRESOURCE);
			gResourceMgr.ChangeResourceType(res, ResourceSystem::RESTYPE_XMLRESOURCE);
		}
	}

	// Load prototypes.
	gEntityMgr.LoadPrototypes();

	gGfxWindow.SetWindowCaption(mProjectInfo.name);

	ocInfo << "Project " << path << " loaded.";

	OpenDefaultScene();

	if (mEditorSupport)
	{
		gEditorMgr.OnProjectOpened();
	}

	return true;
}

void Project::CloseProject()
{
	if (!IsProjectOpened())
		return;

	CloseOpenedScene();
	SaveProjectConfig();
	GlobalProperties::RemovePointer("ProjectConfig");
	delete mProjectConfig;
	mProjectConfig = 0;
	mProjectPath = "";
	mResourceTypeMap.clear();
	mSceneList.clear();
	gResourceMgr.DeleteProjectResources();
	gEntityMgr.DestroyAllEntities(true, false); // Is this OK?
	
	if (mEditorSupport)
	{
		gEditorMgr.OnProjectClosed();
	}

	gGfxWindow.SetWindowCaption("");

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

string Project::GetAbsoluteOpenedProjectPath() const
{
	return boost::filesystem::complete(boost::filesystem::path(mProjectPath)).directory_string();
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
		sceneInfo.name = sceneInfo.filename;
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
		mProjectConfig->SetString(it->filename, "", "scenes");
	}

	mProjectConfig->Save();
	ocInfo << "Project " << mProjectPath << " saved.";
}

void Project::CreateDefaultProjectStructure()
{
	try
	{
		FilesystemUtils::CopyDirectory(gApp.GetDataDirectory() + "dummy", mProjectPath, ".*", "\\.svn");

		// the scene file was copied to the project directory by the command above
		SceneInfo sceneInfo;
		sceneInfo.filename = "defaultScene.xml";
		sceneInfo.name = sceneInfo.filename;
		mSceneList.push_back(sceneInfo);
		mSceneIndex = mSceneList.size() - 1;
	}
	catch (const boost::system::system_error& e)
	{
		ocWarning << "System exception: " << e.what();
	}
}

bool Project::CreateScene(string sceneFilename, const string& sceneName)
{
	if (!IsProjectOpened()) return false;
	if (IsSceneOpened())
		CloseOpenedScene();

	if (sceneFilename.at(0) == '/')
		sceneFilename = sceneFilename.substr(1);

	string filename = mProjectPath + "/" + sceneFilename;
	if (boost::filesystem::exists(filename))
	{
		ocWarning << "Cannot create scene '" << filename << "'. Already exists.";
		return false;
	}

	// Create empty scene file.
	{
		ResourceSystem::XMLOutput xmlOutput(filename);
		xmlOutput.BeginElement("Scene");
		xmlOutput.BeginElement("Layers");
		xmlOutput.EndElement();
		xmlOutput.BeginElement("Entities");
		xmlOutput.EndElement();
		xmlOutput.BeginElement("Hierarchy");
		xmlOutput.EndElement();
		xmlOutput.EndElement();
	}
	gResourceMgr.AddResourceFileToGroup(sceneFilename, "Project", ResourceSystem::RESTYPE_XMLRESOURCE, ResourceSystem::BPT_PROJECT);

	SceneInfo sceneInfo;
	sceneInfo.name = sceneName;
	sceneInfo.filename = sceneFilename;
	mSceneList.push_back(sceneInfo);
	mSceneIndex = mSceneList.size() - 1;

	if (mEditorSupport)
	{
		gEditorMgr.UpdateSceneMenu();
		gGfxWindow.SetWindowCaption(mProjectInfo.name + " (" + sceneFilename + ")");
	}

	OpenSceneAtIndex(mSceneIndex);
	return true;
}

bool Project::OpenSceneAtIndex(int32 sceneIndex)
{
	if (sceneIndex < 0 || sceneIndex >= (int32)mSceneList.size()) return false;
	if (IsSceneOpened()) CloseOpenedScene();

	ResourceSystem::ResourcePtr res = gResourceMgr.GetResource("Project", mSceneList[sceneIndex].filename);
	if (res->GetType() != ResourceSystem::RESTYPE_XMLRESOURCE)
	{
		SetResourceType(res, ResourceSystem::RESTYPE_XMLRESOURCE);
		res = gEditorMgr.ChangeResourceType(res, ResourceSystem::RESTYPE_XMLRESOURCE);
		if (gApp.IsEditMode())
		{
			gEditorMgr.GetResourceWindow()->Update();
		}
	}
	OpenScene(res);

	return true;
}

void Core::Project::OpenScene( const ResourceSystem::ResourcePtr resource )
{
	if (IsSceneOpened()) CloseOpenedScene();

	OC_ASSERT_MSG((bool)resource, "Wrong scene resource");
	mSceneIndex = GetSceneIndex(resource->GetName());
	if (mSceneIndex == -1)
	{
		ocError << "Scene resource is not in the scene list of the project";
		mSceneIndex = -1;
		return;
	}
	if (resource->GetType() != ResourceSystem::RESTYPE_XMLRESOURCE)
	{
		ocError << "Scene resource must be of the XML type";
		mSceneIndex = -1;
		return;
	}

	Core::Game& game = GlobalProperties::Get<Core::Game>("Game");

	// Set the in-game GUI root window
	if (gApp.IsEditMode()) game.SetRootWindow(gEditorMgr.GetGameViewport());
	else game.CreateDefaultRootWindow();

	gEntityMgr.LoadEntitiesFromResource(resource);

	game.Init();

	// Set the viewport cameras and run the action in non-edit mode
	if (gApp.IsEditMode())
	{
		gEditorMgr.GetEditorGUI()->RefreshCameras();
	}
	else
	{
		game.CreateDefaultRenderTarget();
		game.ResumeAction();
	}

	if (mEditorSupport)
	{
		gGfxWindow.SetWindowCaption(mProjectInfo.name + " (" + resource->GetName() + ")");
		gEditorMgr.OnSceneOpened();
	}

	ocInfo << "Scene " << resource->GetName() << " loaded.";
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
	OC_DASSERT(mSceneIndex >= 0 && mSceneIndex < (int32)mSceneList.size());
	
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
	if (!IsSceneOpened()) return;

	mSceneIndex = -1;
	gGfxWindow.SetWindowCaption(mProjectInfo.name);
	gEntityMgr.DestroyAllEntities(false, true);
	gLayerMgr.Clear();
	if (mEditorSupport)
	{
		gEditorMgr.OnSceneClosed();
	}
}

void Project::GetSceneList(SceneInfoList& scenes) const
{
	scenes = mSceneList;
}

int32 Project::GetSceneIndex(const string& scene) const
{
  int32 sceneIndex = -1;
	for (SceneInfoList::const_iterator it = mSceneList.begin(); it != mSceneList.end(); ++it)
	{
		if (it->filename == scene)
		{
			sceneIndex = it - mSceneList.begin();
			break;
		}
	}
	return sceneIndex;
}

size_t Project::GetSceneCount() const
{
	return mSceneList.size();
}

string Project::GetOpenedSceneName() const
{
	OC_ASSERT(mSceneIndex >= 0 && mSceneIndex < (int32)mSceneList.size());
	return mSceneList[mSceneIndex].name;
}

bool Core::Project::IsSceneOpened() const
{
	return mSceneIndex != -1;
}

bool Project::RequestOpenSceneAtIndex(int32 index)
{
  if (index < 0 || index >= (int32)mSceneList.size()) return false;
  mRequestSceneIndex = index;
  return true;
}

void Core::Project::Update()
{
	if (mRequestSceneIndex >= 0)
	{
		if (gApp.IsEditMode())
		{ 
			gEditorMgr.RestartAction();
			gEditorMgr.SwitchActionTool(Editor::EditorMgr::AT_RESTART);
		}
		else
		{
			CloseOpenedScene();
			OpenSceneAtIndex(mRequestSceneIndex);
		}
		mRequestSceneIndex = -1;
	}
}

bool Core::Project::IsResourceScene( const ResourceSystem::ResourcePtr resource ) const
{
	return GetSceneIndex(resource->GetName()) != -1;
}

void Core::Project::ForceOpenSceneName(const string& scene)
{
  int32 index = GetSceneIndex(scene);
  if (index != -1) mSceneIndex = index;
}

string Core::Project::GetRequestedSceneName() const
{
  return (mRequestSceneIndex != -1) ? mSceneList[mRequestSceneIndex].name : "";
}

void Core::Project::SetResourceType(ResourceSystem::ResourcePtr resource, ResourceSystem::eResourceType newType)
{
	string filePath = resource->GetName();
	mResourceTypeMap[filePath] = newType;
}
