#include "Common.h"
#include "Application.h"
#include "LoadingScreen.h"
#include "Game.h"
#include "Config.h"
#include "Project.h"
#include "LogSystem/LogMgr.h"
#include "GfxSystem/OglRenderer.h"
#include "GfxSystem/GfxWindow.h"
#include "GUISystem/ViewportWindow.h"
#include "ScriptSystem/ScriptResource.h"
#include "StringSystem/StringMgr.h"
#include "StringSystem/TextResource.h"
#include "Editor/EditorMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "Utils/FilesystemUtils.h"


using namespace Core;

const float32 MAX_DELTA_TIME = 0.5f;

Application::Application():
	StateMachine<eAppState>(AS_INITING),
	mDevelopMode(true),
	mEditMode(true),
	mGameProject(0),
	mHasFocus(true),
	mFrameSmoothingTime(0.5f),
	mConsoleHandle(0)
{
	#ifdef DEPLOY
	mDevelopMode = false;
	mEditMode = false;
	#else
	mDevelopMode = true;
	mEditMode = true;
	#endif
}

void Application::Init(const string& sharedDir)
{
	mSharedDir = sharedDir;
	
	if (mSharedDir.empty())
	{
		// if the data directory is not defined use defaults, try to use following cases

		#ifdef _DEBUG // in debug mode we can assume current directory as sharedDir
		if (mSharedDir.empty() && boost::filesystem::exists("data") && boost::filesystem::exists("deploy"))
		{
			mSharedDir = ".";
		}
		#endif

		#ifdef DEPLOY // in deploy mode we can assume current directory as sharedDir
		if (mSharedDir.empty() && boost::filesystem::exists("data"))
		{
			mSharedDir = ".";
		}
		#endif

		#ifdef OCERUS_SHARED_DIR // if sharedDir still not set, use OCERUS_SHARED_DIR macro
		if (mSharedDir.empty())
		{
			mSharedDir = OCERUS_SHARED_DIR;
		}
		#endif

		if (mSharedDir.empty()) // finally use the current directory as fallback
		{
			mSharedDir = ".";
		}
	}

	// prepare paths for the application
	const char* coreLogFilename = "Core.log";
	const char* configFilename = "config.ini";
	const char* ceguiLogFilename = "CEGUI.log";

	// detect if the temp path exists and create it if needed
	boost::filesystem::path tempDir;
	boost::filesystem::ofstream outStream;
	outStream.open(coreLogFilename);
	if (!outStream.is_open())
	{
		#if defined(__WIN__)
		boost::filesystem::path tempPath(std::getenv("TEMP"));
		const char* ocerusTempDir = "Ocerus";
		tempDir = tempPath /= ocerusTempDir;
		#else
		tempDir = boost::filesystem::path(string(std::getenv("HOME")) + "/.ocerus");
		#endif
		boost::filesystem::create_directory(tempDir);
	}
	else
	{
		outStream.close();
	}
	mTempDir = tempDir.string();
	if (mTempDir.empty()) mTempDir = ".";

	// create basic singletons
	LogSystem::LogMgr::CreateSingleton();
	LogSystem::LogMgr::GetSingleton().Init((tempDir / coreLogFilename).string());
	LogSystem::Profiler::CreateSingleton();

	// get access to config file
	mGlobalConfig = new Config((tempDir / configFilename).string());
	GlobalProperties::SetPointer("GlobalConfig", mGlobalConfig);

	// make the app settings public
	GlobalProperties::SetPointer("DevelopMode", &mDevelopMode);
	GlobalProperties::SetPointer("EditMode", &mEditMode);

	// load console properties
	mConsoleX = mGlobalConfig->GetInt32("ConsoleX", 0, "Windows");
	mConsoleY = mGlobalConfig->GetInt32("ConsoleY", 0, "Windows");
	mConsoleWidth = mGlobalConfig->GetInt32("ConsoleW", 1024, "Windows");
	mConsoleHeight = mGlobalConfig->GetInt32("ConsoleH", 768, "Windows");

	// debug window
	#if !NO_CONSOLE
	if (mDevelopMode) ShowConsole();
	#endif

	// test shared dirs
	if (!boost::filesystem::exists(GetDataDirectory()))
	{
		ocError << "Ocerus data directory does not exist: " << GetDataDirectory();
		CRITICAL_FAILURE(("Ocerus data directory does not exist: " + GetDataDirectory()).c_str());
	}

	if (mDevelopMode && !boost::filesystem::exists(GetDeployDirectory()))
	{
		ocError << "Ocerus deploy directory does not exist: " << GetDeployDirectory();
		CRITICAL_FAILURE(("Ocerus deploy directory does not exist: " + GetDeployDirectory()).c_str());
	}

	// create singletons

	ResourceSystem::ResourceMgr::CreateSingleton();
	ResourceSystem::ResourceMgr::GetSingleton().Init(GetDataDirectory());

	StringSystem::StringMgr::Init();
	gStringMgrSystem.LoadLanguagePack(mGlobalConfig->GetString("Language", "", "Editor"), mGlobalConfig->GetString("Country", "", "Editor"));

	GfxSystem::GfxWindow::CreateSingleton();
	int32 windowX = mGlobalConfig->GetInt32("WindowX", 50, "Windows"); // start at 50 to give some offset to the window
	int32 windowY = mGlobalConfig->GetInt32("WindowY", 50, "Windows");
	int32 windowWidth = mGlobalConfig->GetInt32("WindowWidth", 1024, "Windows");
	int32 windowHeight = mGlobalConfig->GetInt32("WindowHeight", 768, "Windows");
	int32 resX = mGlobalConfig->GetInt32("FullscreenResolutionWidth", 0, "Windows");
	int32 resY = mGlobalConfig->GetInt32("FullscreenResolutionHeight", 0, "Windows");
	bool fullscreen = mGlobalConfig->GetBool("Fullscreen", false, "Windows");
	GfxSystem::GfxWindow::GetSingleton().Init(windowX, windowY, windowWidth, windowHeight, resX, resY, fullscreen, "Loading...");

	GfxSystem::GfxRenderer::CreateSingleton<GfxSystem::OglRenderer>();
	GfxSystem::GfxRenderer::GetSingleton().Init();

	InputSystem::InputMgr::CreateSingleton();

	ScriptSystem::ScriptMgr::CreateSingleton();

	EntitySystem::EntityMgr::CreateSingleton();

	GUISystem::GUIMgr::CreateSingleton();
	GUISystem::GUIMgr::GetSingleton().Init((tempDir / ceguiLogFilename).string());

	Editor::EditorMgr::CreateSingleton();
	EntitySystem::LayerMgr::CreateSingleton();

	// create core states
	mLoadingScreen = new LoadingScreen();
	mGame = new Game(tempDir.string());

	// init finished, now loading
	RequestStateChange(AS_LOADING);
	UpdateState();

	// FPS counter init
	ResetStats();
}

Application::~Application()
{
	gEntityMgr.DestroyAllEntities(true, true);

	// mGameProject is deleted by EditorMgr in develop mode.
	if (mGameProject && !mDevelopMode) delete mGameProject;
	delete mLoadingScreen;
	delete mGame;

	EntitySystem::LayerMgr::DestroySingleton();

	if (mDevelopMode)
		gEditorMgr.Deinit();
	Editor::EditorMgr::DestroySingleton();

	GUISystem::GUIMgr::DestroySingleton();

	// Cancel unload callback for script and text resources
	ScriptSystem::ScriptResource::SetUnloadCallback(0);
	StringSystem::TextResource::SetUnloadCallback(0);
	ResourceSystem::ResourceMgr::GetSingleton().UnloadAllResources();

	EntitySystem::EntityMgr::DestroySingleton();

	ScriptSystem::ScriptMgr::DestroySingleton();

	InputSystem::InputMgr::DestroySingleton();

	GfxSystem::GfxRenderer::DestroySingleton();

	mGlobalConfig->SetInt32("WindowX", gGfxWindow.GetWindowX(), "Windows");
	mGlobalConfig->SetInt32("WindowY", gGfxWindow.GetWindowY(), "Windows");
	mGlobalConfig->SetInt32("WindowWidth", gGfxWindow.GetWindowWidth(), "Windows");
	mGlobalConfig->SetInt32("WindowHeight", gGfxWindow.GetWindowHeight(), "Windows");
	mGlobalConfig->SetInt32("FullscreenResolutionWidth", gGfxWindow.GetFullscreenResolutionWidth(), "Windows");
	mGlobalConfig->SetInt32("FullscreenResolutionHeight", gGfxWindow.GetFullscreenResolutionHeight(), "Windows");
	mGlobalConfig->SetBool("Fullscreen", gGfxWindow.IsFullscreen(), "Windows");
	GfxSystem::GfxWindow::DestroySingleton();

	StringSystem::StringMgr::Deinit();

	ResourceSystem::ResourceMgr::DestroySingleton();

	Utils::Hash::ClearHashMap();

	// must come last
	HideConsole();
	delete mGlobalConfig;
	LogSystem::Profiler::DestroySingleton();
	LogSystem::LogMgr::DestroySingleton();
	Reflection::PropertySystem::DestroyProperties();
}

void Application::RunMainLoop()
{
	while (GetState() != AS_SHUTDOWN)
	{
		// process window events
		MessagePump();

		// make sure we have everything ok
		OC_ASSERT(mGame);
		OC_ASSERT(mLoadingScreen);

		// update the profiler
		gProfiler.Update();

		// process input events
		gInputMgr.CaptureInput();

		// make sure the resources are up to date
		if ((mDevelopMode) && (GetState() != AS_LOADING) && (gEditorMgr.IsProjectOpened()))
		{
			bool refreshWindow = false;

			if (gResourceMgr.CheckForRefreshPath())
				refreshWindow = true;
			if (gResourceMgr.CheckForResourcesUpdates())
				refreshWindow = true;

			// refresh strings && layouts
			gStringMgrSystem.Update();
			if (gStringMgrProject.Update() || gGUIMgr.Update())
			{
				// refresh project GUI layout
				EntitySystem::EntityList entities;
				gEntityMgr.GetEntitiesWithComponent(entities, EntitySystem::CT_GUILayout);
				for (EntitySystem::EntityList::const_iterator it = entities.begin(); it != entities.end(); ++it)
				{
					gEntityMgr.PostMessage(*it, EntitySystem::EntityMessage::RESOURCE_UPDATE);
				}
			}

			if (refreshWindow)
				gEditorMgr.UpdateResourceWindow();
		}

		// calculate time since last frame
		float32 delta = CalculateFrameDeltaTime();

		// update logic
		FrameUpdate(delta);

		// draw
		FrameDraw(delta);

		// update FPS and other performance counters
		UpdateStats();

		// update app state machine
		UpdateState();

		// if we don't have focus yield for a while to allow other apps to work
		if (!mHasFocus) YieldProcess();
	}
}

float32 Application::CalculateFrameDeltaTime( void )
{
	PROFILE_FNC();

	uint64 curTime = mTimer.GetMilliseconds();
	mFrameDeltaTimes.push_back(curTime);

	if (mFrameDeltaTimes.size() == 1)
		return 0;

	uint64 discardThreshold = static_cast<uint64>(mFrameSmoothingTime*1000.0f);

	// discard times older then smoothing time
	TimesList::iterator it = mFrameDeltaTimes.begin();
	TimesList::iterator iend = mFrameDeltaTimes.end() - 2; // need at least 2 times
	while (it != iend)
	{
		if (curTime - *it > discardThreshold)
			++it;
		else
			break;
	}

	// remove old times
	mFrameDeltaTimes.erase(mFrameDeltaTimes.begin(), it);

	// average and convert to seconds
	float32 result = (float32)(mFrameDeltaTimes.back() - mFrameDeltaTimes.front()) / ((mFrameDeltaTimes.size()-1)*1000);
	return MathUtils::Min(result, MAX_DELTA_TIME);
}

void Application::ResetStats()
{
	mLastFPS = 0.0f;
	mAvgFPS = 0.0f;
	mFrameCount = 0;
	mLastSecond = mTimer.GetMilliseconds();
}

void Application::UpdateStats()
{
	++mFrameCount;
	uint64 curTimeMillis = mTimer.GetMilliseconds();
	// update only once per second
	if (curTimeMillis - mLastSecond > 1000)
	{
		mLastFPS = 1000.0f * (float32)mFrameCount / (float32)(curTimeMillis - mLastSecond);
		if (mAvgFPS != 0)
			mAvgFPS = mLastFPS;
		else
			mAvgFPS = 0.05f * (mAvgFPS + mLastFPS); // approximation
		mLastSecond = curTimeMillis;
		mFrameCount = 0;
	}
}

void Core::Application::Shutdown( void )
{
	RequestStateChange(AS_SHUTDOWN);
}

void Application::MessagePump( void )
{
    GfxSystem::eWindowEvent event;
    while (GfxSystem::GfxWindow::GetSingleton().PopEvent(event))
	{
		if (event == GfxSystem::WE_QUIT)
		{
			RequestStateChange(AS_SHUTDOWN, true);
		}
		else if (event == GfxSystem::WE_LOST_FOCUS)
		{
			mHasFocus = false;
			gInputMgr.ReleaseAll();

			// SDL won't let the mouse move when the SDL cursor is disabled, app is in fullscreen but dont have focus
			if (gGfxWindow.IsFullscreen())
				gGfxWindow.SetSDLCursorVisibility(true);
		}
		else if (event == GfxSystem::WE_GAINED_FOCUS)
		{
			mHasFocus = true;
			gGfxWindow.SetSDLCursorVisibility(false);
		}
	}
}

void Core::Application::RegisterGameInputListener( InputSystem::IInputListener* listener )
{
	if (mDevelopMode)
	{
		gEditorMgr.GetGameViewport()->AddInputListener(listener);
	}
	else
	{
		gInputMgr.AddInputListener(listener);
	}
}

void Core::Application::UnregisterGameInputListener( InputSystem::IInputListener* listener )
{
	if (mDevelopMode)
	{
		if (gEditorMgr.GetGameViewport())
			gEditorMgr.GetGameViewport()->RemoveInputListener(listener);
	}
	else
	{
		gInputMgr.RemoveInputListener(listener);
	}
}

void Core::Application::GetAvailableDeployPlatforms( vector<string>& out )
{
	boost::filesystem::path deployDirectory = GetDeployDirectory();
	if (!boost::filesystem::exists(deployDirectory)) return;
	boost::filesystem::directory_iterator iend;
	for (boost::filesystem::directory_iterator i(deployDirectory); i!=iend; ++i)
	{
		if (boost::filesystem::is_directory(i->path()))
		{	
			out.push_back(i->path().filename());
		}
	}
}



bool Core::Application::DeployCurrentProject( const string& platform, const string& destination )
{
	if (!mGameProject || !mGameProject->IsProjectOpened())
	{
		ocWarning << "Invalid project to deploy";
		return false;
	}

	Core::ProjectInfo pi;
	mGameProject->GetOpenedProjectInfo(pi);
	ocInfo << "Deploying project " << pi.name << " for the " << platform << " platform into " << destination;

	try
	{
		boost::filesystem::path destinationPath = destination;
		boost::filesystem::remove_all(destinationPath);
		FilesystemUtils::CopyDirectory(".", destination, ".*\\.dll", "\\.svn|libexpat\\.dll");
		FilesystemUtils::CopyDirectory(GetDeployDirectory() + platform, destination, ".*", "\\.svn");
		FilesystemUtils::CopyDirectory(GetDataDirectory() + "general", destination + "/data/general", ".*", "\\.svn");
		FilesystemUtils::CopyDirectory(mGameProject->GetOpenedProjectPath(), destination, ".*", "\\.svn");
	}
	catch (std::exception& e)
	{
		ocError << "Deploying failed: " << e.what();
		return false;
	}
	return true;
}

bool Application::CheckDeployDestination(const string& destination)
{
	boost::filesystem::path destinationPath = destination;
	if (!boost::filesystem::exists(destinationPath))
	{
		ocWarning << "Deployment destination path does not exist.";
		return false;
	}
	if (!boost::filesystem::is_directory(destinationPath))
	{
		ocWarning << "Deployment destination path is not a directory.";
		return false;
	}
	if (!boost::filesystem::is_empty(destinationPath))
	{
		ocWarning << "Deployment destination path is not empty.";
		return false;
	}
	return true;
}

string Application::GetDataDirectory() const
{
	boost::filesystem::path boostPath = mSharedDir;
	boostPath /= "data/";
	return boostPath.string();
}

string Application::GetDeployDirectory() const
{
	boost::filesystem::path boostPath = mSharedDir;
	boostPath /= "deploy/";
	return boostPath.string();
}

void Application::FrameUpdate(float32 delta)
{
	switch (GetState())
	{
	case AS_LOADING:

		mLoadingScreen->DoLoading(LoadingScreen::TYPE_BASIC_RESOURCES);
		mLoadingScreen->DoLoading(LoadingScreen::TYPE_GENERAL_RESOURCES);
		if (mDevelopMode) mLoadingScreen->DoLoading(LoadingScreen::TYPE_EDITOR);

		mGame->Init();

		gGfxWindow.SetWindowCaption("");

		if (mDevelopMode)
		{
			// note that this must come before the project is being opened as it's referenced during opening of the project
			mGameProject = gEditorMgr.GetCurrentProject();
			gEditorMgr.OnProjectClosed();
			gEditorMgr.OpenProject(mGlobalConfig->GetString("LastProject"));
		}
		else
		{
			if (!mGameProject) mGameProject = new Project(false);
			if (!mGameProject->OpenProject("."))
			{
				ocError << "Invalid project; quiting...";
				Shutdown();
				break;
			}
		}

		RequestStateChange(AS_GAME, true);

		break;
	case AS_GAME:
		gGUIMgr.Update(delta);
		mGameProject->Update();
		mGame->Update(delta);
		if (mEditMode)
		{
			gEditorMgr.Update(delta);
		}
		break;
	default:
		break;
	}
}

void Application::FrameDraw( float32 delta )
{
	if (gGfxRenderer.BeginRendering())
	{
		switch (GetState())
		{
		case AS_GAME:

			if (mEditMode)
			{
				gEditorMgr.Draw(delta);
			}
			else
			{
				mGame->Draw(delta);
			}
			gGUIMgr.RenderGUI();
			break;
		default:
			break;
		}

		gGfxRenderer.EndRendering();
	}
}

string Core::Application::GetTempDirectory() const
{
	return mTempDir;
}

//-----------------------------------------------------
// Platfofm specific functions follow.

#if defined(__WIN__)
//------------
// Windows
//------------
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>

void Core::Application::ShowConsole( void )
{
	// create the window
	AllocConsole();

	// get hwnd
	const char* uniqueName = "3248962941235952";
	SetConsoleTitle(uniqueName);
	Sleep(40);
	mConsoleHandle = (uint32)(FindWindow(NULL, uniqueName));

	// set title
	SetConsoleTitle("Debug Log");

	// set position
	if (mConsoleHandle)
	{
		MoveWindow((HWND)mConsoleHandle, mConsoleX, mConsoleY, mConsoleWidth, mConsoleHeight, true);
	}
}

void Core::Application::HideConsole( void )
{
	// save console settings first
	RECT windowRect;
	if (GetWindowRect((HWND)mConsoleHandle, &windowRect))
	{
		mGlobalConfig->SetInt32("ConsoleX", windowRect.left, "Windows");
		mGlobalConfig->SetInt32("ConsoleY", windowRect.top, "Windows");
		mGlobalConfig->SetInt32("ConsoleW", windowRect.right-windowRect.left, "Windows");
		mGlobalConfig->SetInt32("ConsoleH", windowRect.bottom-windowRect.top, "Windows");
	}

	// destroy the window
	ShowWindow((HWND)mConsoleHandle, SW_HIDE);
	FreeConsole();
}

void Core::Application::WriteToConsole( const string& str )
{
	DWORD writtenChars = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.length(), &writtenChars, NULL);
}

DWORD WINAPI Open_thread(LPVOID lpParam)
{
	system(((string*)lpParam)->c_str());
	return 0;
}

void Core::Application::OpenFileInExternalApp( const string& filePath )
{
	static string adjustedPath;
	
	if (boost::filesystem::path(filePath).is_complete())
	{
		// the path is absolute, so use it as it is
		adjustedPath = string("\"") + filePath + "\"";
	}
	else
	{
		// otherwise use it relative to the shared directory
		adjustedPath = string("\"") + mSharedDir + "\\" + filePath + "\"";
	}
	size_t slashPos = adjustedPath.find('/');
	while (slashPos != string::npos)
	{
		adjustedPath.replace(slashPos, 1, "\\");
		slashPos = adjustedPath.find('/');
	}
	ocInfo << "Opening " << adjustedPath;

	CreateThread(NULL, 0, Open_thread, &adjustedPath, 0, NULL);
}

void Core::Application::YieldProcess()
{
	Sleep(1);
}

#else

//------------
// Unix
//------------

#include <iostream>

void Core::Application::ShowConsole( void )
{
}

void Core::Application::HideConsole( void )
{
}

void Core::Application::WriteToConsole( const string& message )
{
    std::cerr << message;
}

bool OpenFileUsingCommands(const string& filePath, string commands[], uint32 commandsCount)
{
	for (uint32 idx = 0; idx < commandsCount; ++idx)
	{
		ocInfo << "Opening " << filePath << " with " << commands[idx] << ".";
		bool result = system((commands[idx] + " " + filePath).c_str());
		if (result == EXIT_SUCCESS)
			return true;
		ocInfo << "Cannot open " << filePath << " with " << commands[idx] << ".";
	}
	return false;
}

void Core::Application::OpenFileInExternalApp( const string& filePath )
{
	static string generalCommands[] = { "xdg-open", "kde-open", "gnome-open" };
	static uint32 generalCommandsCount = 3;
	static string pdfCommands[] = { "okular", "kpdf", "evince", "xpdf", "acroread", "epdfview" };
	static uint32 pdfCommandsCount = 6;

	string adjustedPath;
	if (boost::filesystem::path(filePath).is_complete())
	{
		// the path is absolute, so use it as it is
		adjustedPath = filePath;
	}
	else
	{
		// otherwise use it relative to the shared directory
		adjustedPath = mSharedDir + "/" + filePath;
	}

	bool result = OpenFileUsingCommands(adjustedPath, generalCommands, generalCommandsCount);
	if (!result)
	{
		if (adjustedPath.compare(adjustedPath.size() - 3, 3, "pdf") == 0)
		{
			result = OpenFileUsingCommands(adjustedPath, pdfCommands, pdfCommandsCount);
		}

		if (!result)
			ocWarning << "Cannot find suitable command to open " << adjustedPath << ".";		
	}
}

void Core::Application::YieldProcess()
{
	// Update each 20ms, mouse is still OK
	usleep(20000);
}

#endif
