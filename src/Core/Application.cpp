#include "Common.h"
#include "Application.h"
#include "LoadingScreen.h"
#include "Game.h"
#include "Config.h"
#include "../LogSystem/LogMgr.h"
#include "../GfxSystem/OglRenderer.h"
#include "../GfxSystem/GfxWindow.h"


using namespace Core;

#define MAX_DELTA_TIME 0.5f

Application::Application():
	StateMachine<eAppState>(AS_INITING), mFrameSmoothingTime(0.5f), mConsoleHandle(0)
{
}

void Application::Init()
{
	// create basic singletons
	LogSystem::LogMgr::CreateSingleton();
	LogSystem::LogMgr::GetSingleton().Init("CoreLog.txt");

	// get access to config file
	mGlobalConfig = new Config("config.txt");
	GlobalProperties::SetPointer("GlobalConfig", mGlobalConfig);

	// make the app settings public
	GlobalProperties::SetPointer("DevelopMode", &mDevelopMode);

	// load console properties
	mConsoleX = mGlobalConfig->GetInt32("ConsoleX", 0, "Windows");
	mConsoleY = mGlobalConfig->GetInt32("ConsoleY", 0, "Windows");
	mConsoleWidth = mGlobalConfig->GetInt32("ConsoleW", 1024, "Windows");
	mConsoleHeight = mGlobalConfig->GetInt32("ConsoleH", 768, "Windows");

	// debug window
	ShowConsole();

	// create singletons
	ResourceSystem::ResourceMgr::CreateSingleton();
	ResourceSystem::ResourceMgr::GetSingleton().Init("data/");

	StringSystem::StringMgr::CreateSingleton();

	GfxSystem::GfxWindow::CreateSingleton();
	GfxSystem::GfxWindow::GetSingleton().Init(1024, 768, false, "Ocerus");

	GfxSystem::GfxRenderer::CreateSingleton<GfxSystem::OglRenderer>();
	GfxSystem::GfxRenderer::GetSingleton().Init();

	InputSystem::InputMgr::CreateSingleton();

	ScriptSystem::ScriptMgr::CreateSingleton();
	
	EntitySystem::EntityMgr::CreateSingleton();

	GUISystem::GUIMgr::CreateSingleton();

	//TODO:Gfx
	//GfxSystem::ParticleSystemMgr::CreateSingleton();

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
	HideConsole();

	delete mLoadingScreen;

	GUISystem::GUIMgr::DestroySingleton();

	ResourceSystem::ResourceMgr::GetSingleton().UnloadAllResources();

	EntitySystem::EntityMgr::DestroySingleton();
	ScriptSystem::ScriptMgr::DestroySingleton();
	delete mGame;
	InputSystem::InputMgr::DestroySingleton();
	GfxSystem::GfxRenderer::DestroySingleton();
	GfxSystem::GfxWindow::DestroySingleton();
	StringSystem::StringMgr::DestroySingleton();
	ResourceSystem::ResourceMgr::DestroySingleton();
	//TODO:Gfx
	//GfxSystem::ParticleSystemMgr::DestroySingleton();
	Utils::Hash::ClearHashMap();

	// must come last
	delete mGlobalConfig;
	LogSystem::LogMgr::DestroySingleton();
}

void Application::RunMainLoop()
{
	while (GetState() != AS_SHUTDOWN)
	{
		// process window events
		MessagePump();

		// process input events
		gInputMgr.CaptureInput();

		// make sure the resources are up to date
		if (mDevelopMode) gResourceMgr.CheckForResourcesUpdates();

		// calculate time since last frame
		float32 delta = CalculateFrameDeltaTime();

		// update logic
		switch (GetState())
		{
		case AS_LOADING:
			mLoadingScreen->DoLoading(LoadingScreen::TYPE_BASIC_RESOURCES);
			mLoadingScreen->DoLoading(LoadingScreen::TYPE_GENERAL_RESOURCES);
			mGame->Init();
			RequestStateChange(AS_GAME, true);
			break;
		case AS_GAME:
			mGame->Update(delta);
			gGUIMgr.Update(delta);
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
				mGame->Draw(delta);
				gGUIMgr.RenderGUI();
				break;
			default:
				break;
			}

			gGfxRenderer.DrawTestQuad();

			// draw stats (from previous frame)
			//TODO:Gfx
			//gGfxRenderer.DrawString(0.0f, 0.0f, "FPS", "FPS: " + StringConverter::ToString(mAvgFPS),
			//	GfxSystem::Color(0,180,0), ANCHOR_LEFT | ANCHOR_BOTTOM,
			//	ANCHOR_LEFT | ANCHOR_BOTTOM);

			gGfxRenderer.EndRendering();
		}

		// update FPS and other performance counters
		UpdateStats();

		// update app state machine
		UpdateState();
	}
}

float32 Application::CalculateFrameDeltaTime( void )
{
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
    GfxSystem::EWindowEvent event;
    while (GfxSystem::GfxWindow::GetSingleton().PopEvent(event))
	{
		if (event == GfxSystem::WE_QUIT)
		{
			RequestStateChange(AS_SHUTDOWN, true);
		}
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
	FreeConsole();
}

void Core::Application::WriteToConsole( const string& str )
{
	DWORD writtenChars = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.length(), &writtenChars, NULL);
}

#else

//------------
// Unix
//------------

void Core::Application::ShowConsole( void )
{
    ///@todo port me to unix
}

void Core::Application::HideConsole( void )
{
    ///@todo port me to unix
}

void Core::Application::WriteToConsole( const string& str )
{
    ///@todo port me to unix
}

#endif

