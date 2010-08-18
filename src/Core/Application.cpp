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
#include "Editor/EditorMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"


using namespace Core;

#define MAX_DELTA_TIME 0.5f

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

void Application::Init(const string& startupProjectName)
{
	// set up basic data
	mStartupProjectName = startupProjectName;

	// create basic singletons
	LogSystem::LogMgr::CreateSingleton();
	LogSystem::LogMgr::GetSingleton().Init("CoreLog.txt");
	LogSystem::Profiler::CreateSingleton();

	// get access to config file
	mGlobalConfig = new Config("config.ini");
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
	if (mDevelopMode) ShowConsole();

	// create singletons

	ResourceSystem::ResourceMgr::CreateSingleton();
	ResourceSystem::ResourceMgr::GetSingleton().Init("data/");

	StringSystem::StringMgr::Init();
	gStringMgrSystem.LoadLanguagePack(mGlobalConfig->GetString("Language", "", "Editor"), mGlobalConfig->GetString("Country", "", "Editor"));

	GfxSystem::GfxWindow::CreateSingleton();
	int32 windowX = mGlobalConfig->GetInt32("WindowX", 50, "Windows"); // start at 50 to give some offset to the window
	int32 windowY = mGlobalConfig->GetInt32("WindowY", 50, "Windows");
	int32 windowWidth = mGlobalConfig->GetInt32("WindowWidth", 1024, "Windows");
	int32 windowHeight = mGlobalConfig->GetInt32("WindowHeight", 768, "Windows");
	int32 resX = mGlobalConfig->GetInt32("FullscreenResolutionWidth", 1280, "Windows");
	int32 resY = mGlobalConfig->GetInt32("FullscreenResolutionHeight", 1024, "Windows");
	bool fullscreen = mGlobalConfig->GetBool("Fullscreen", false, "Windows");
	GfxSystem::GfxWindow::GetSingleton().Init(windowX, windowY, windowWidth, windowHeight, resX, resY, fullscreen, "Loading...");

	GfxSystem::GfxRenderer::CreateSingleton<GfxSystem::OglRenderer>();
	GfxSystem::GfxRenderer::GetSingleton().Init();

	InputSystem::InputMgr::CreateSingleton();

	ScriptSystem::ScriptMgr::CreateSingleton();

	EntitySystem::EntityMgr::CreateSingleton();

	GUISystem::GUIMgr::CreateSingleton();

	Editor::EditorMgr::CreateSingleton();
	EntitySystem::LayerMgr::CreateSingleton();

	// create core states
	mLoadingScreen = new LoadingScreen();
	mGame = new Game();

	// init finished, now loading
	RequestStateChange(AS_LOADING);
	UpdateState();

	// FPS counter init
	ResetStats();
}

Application::~Application()
{
	gEntityMgr.DestroyAllEntities(true, true);

	if (mGameProject) delete mGameProject;
	delete mLoadingScreen;
	delete mGame;

	EntitySystem::LayerMgr::DestroySingleton();
	Editor::EditorMgr::DestroySingleton();

	GUISystem::GUIMgr::DestroySingleton();

	// cancel unload callback for script resources
	ScriptSystem::ScriptResource::SetUnloadCallback(0);
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
	mGlobalConfig->SetBool("Fullscreen", gGfxWindow.GetFullscreen(), "Windows");
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

			if (refreshWindow)
				gEditorMgr.RefreshResourceWindow();
		}

		// calculate time since last frame
		float32 delta = CalculateFrameDeltaTime();

		// update logic
		switch (GetState())
		{
		case AS_LOADING:

			mLoadingScreen->DoLoading(LoadingScreen::TYPE_BASIC_RESOURCES);
			mLoadingScreen->DoLoading(LoadingScreen::TYPE_GENERAL_RESOURCES);
			if (mDevelopMode) mLoadingScreen->DoLoading(LoadingScreen::TYPE_EDITOR);

			mGame->Init();

			if (mDevelopMode)
			{
				//DEBUG
				gEditorMgr.OpenProject("projects/cube");
				mGameProject = gEditorMgr.GetCurrentProject();
			}
			else
			{
				if (!mGameProject) mGameProject = new Project(false);
				OC_ASSERT_MSG(!mStartupProjectName.empty(), "Startup project must be defined when deploying the game!");
				mGameProject->OpenProject(mStartupProjectName);
			}

			RequestStateChange(AS_GAME, true);

			break;
		case AS_GAME:
			mGameProject->Update();
			mGame->Update(delta);
			gGUIMgr.Update(delta);
			if (mEditMode)
			{
				gEditorMgr.Update(delta);
			}
			break;
		default:
			break;
		}

		// draw
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
			if (gGfxWindow.GetFullscreen())
				gGfxWindow.SetSDLCursor(true);
		}
		else if (event == GfxSystem::WE_GAINED_FOCUS)
		{
			mHasFocus = true;
			gGfxWindow.SetSDLCursor(false);
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

DWORD WINAPI OpenPDF_thread(LPVOID lpParam)
{
	system(((string*)lpParam)->c_str());
	return 0;
}

void Core::Application::OpenPDF( const string& filePath )
{
	static string adjustedPath;
	
	adjustedPath = filePath;
	size_t slashPos = adjustedPath.find('/');
	while (slashPos != string::npos)
	{
		adjustedPath.replace(slashPos, 1, "\\");
		slashPos = adjustedPath.find('/');
	}
	ocInfo << "Opening " << adjustedPath;

	CreateThread(NULL, 0, OpenPDF_thread, &adjustedPath, 0, NULL);
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

void Core::Application::OpenPDF( const string& filePath )
{
	static string commands[] = { "xdg-open", "kde-open", "gnome-open", "okular", "kpdf", "evince", "xpdf", "acroread", "epdfview" };
	static size_t commandsCount = 9;
	static size_t commandIndex = 0;
	for (; commandIndex < commandsCount; ++commandIndex)
	{
		ocInfo << "Opening " << filePath << " with " << commands[commandIndex] << ".";
		bool result = system((commands[commandIndex] + " " + filePath).c_str());
		if (result == EXIT_SUCCESS)
			return;
		ocInfo << "Cannot open " << filePath << " with " << commands[commandIndex] << ".";
	}
	ocWarning << "Cannot find suitable command to open " << filePath << ".";
}

void Core::Application::YieldProcess()
{
	sleep(1);
}

#endif

