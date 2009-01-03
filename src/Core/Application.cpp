#include "Common.h"
#include "Application.h"
#include <Windows.h>
#include "LoadingScreen.h"
#include "Game.h"
#include "Config.h"

using namespace Core;

#define MAX_DELTA_TIME 0.5f

Application::Application(): 
	StateMachine<eAppState>(AS_INITING), mFrameSmoothingTime(0.5f), mConsoleHandle(0)
{
}

void Application::Init()
{
	// create basic singletons
	mLogMgr = DYN_NEW LogSystem::LogMgr("CoreLog.txt", LOG_TRIVIAL);

	// get access to config file
	mGlobalConfig = new Config("config.txt");

	// load console properties
	mConsoleX = mGlobalConfig->GetInt32("ConsoleX", 0, "Windows");
	mConsoleY = mGlobalConfig->GetInt32("ConsoleY", 0, "Windows");
	mConsoleWidth = mGlobalConfig->GetInt32("ConsoleW", 1024, "Windows");
	mConsoleHeight = mGlobalConfig->GetInt32("ConsoleH", 768, "Windows");

	// debug window
	ShowConsole(); 

	// create singletons
	mResourceMgr = DYN_NEW ResourceSystem::ResourceMgr("data/");
	mStringMgr = DYN_NEW StringSystem::StringMgr(); 
	mGfxRenderer = DYN_NEW GfxSystem::GfxRenderer(GfxSystem::Point(1024,768), false);
	mInputMgr = DYN_NEW InputSystem::InputMgr();
	mEntityMgr = DYN_NEW EntitySystem::EntityMgr();
	mGUIMgr = DYN_NEW GUISystem::GUIMgr();
	mPSMgr = DYN_NEW GfxSystem::ParticleSystemMgr();

	// create core states
	mLoadingScreen = DYN_NEW LoadingScreen();
	mGame = DYN_NEW Game();

	// init finished, now loading
	RequestStateChange(AS_LOADING);
	UpdateState();

	// FPS counter init
	ResetStats();
}

Application::~Application()
{
	HideConsole();

	DYN_DELETE mLoadingScreen;
	
	DYN_DELETE mGUIMgr;

	mResourceMgr->UnloadAllResources();

	DYN_DELETE mEntityMgr;
	DYN_DELETE mGame;
	DYN_DELETE mInputMgr;		
	DYN_DELETE mGfxRenderer;
	DYN_DELETE mStringMgr;
	DYN_DELETE mResourceMgr;
	DYN_DELETE mPSMgr;
	
	// must come last
	DYN_DELETE mGlobalConfig;
	DYN_DELETE mLogMgr;
}

void Application::RunMainLoop()
{
	while (GetState() != AS_SHUTDOWN)
	{
		// process window events
		MessagePump();

		// process input events
		mInputMgr->CaptureInput();

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
			mGUIMgr->Update(delta);
			break;
		case AS_GUI:
			mGUIMgr->Update(delta);			
			break;
		}
		
		// draw
		if (gGfxRenderer.BeginRendering())
		{				
			switch (GetState())
			{
			case AS_GAME:
				mGame->Draw(delta);
				mGUIMgr->RenderGUI();
				break;
			}

			// draw stats (from previous frame)
			gGfxRenderer.DrawString(0.0f, 0.0f, "FPS", "FPS: " + StringConverter::ToString(mAvgFPS),
				GfxSystem::Color(0,180,0), GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_BOTTOM,
				GfxSystem::ANCHOR_LEFT | GfxSystem::ANCHOR_BOTTOM);

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

void Application::MessagePump( void )
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			RequestStateChange(AS_SHUTDOWN, true);				
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
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
		if (mAvgFPS == 0)
			mAvgFPS = mLastFPS;
		else
			mAvgFPS = 0.05f * (mAvgFPS + mLastFPS); // approximation
		mLastSecond = curTimeMillis;
		mFrameCount = 0;
	}
}

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

void Core::Application::Shutdown( void )
{
	RequestStateChange(AS_SHUTDOWN);
}
