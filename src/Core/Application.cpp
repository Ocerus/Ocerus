#include "Application.h"
#include <Windows.h>
#include "LoadingScreen.h"
#include "Game.h"

using namespace Core;

Application::Application(): 
	StateMachine<eAppState>(AS_INITING), mFrameSmoothingTime(0.5f), 
	mConsoleX(0), mConsoleY(0), mConsoleHeight(768)
{
	ShowConsole(); // debug window

	// create singletons
	mLogMgr = DYN_NEW LogSystem::LogMgr("CoreLog.txt", LOG_TRIVIAL);
	mResourceMgr = DYN_NEW ResourceSystem::ResourceMgr("data");
	mGfxRenderer = DYN_NEW GfxSystem::GfxRenderer(GfxSystem::Point(1024,768), false);
	mInputMgr = DYN_NEW InputSystem::InputMgr();
	mEntityMgr = DYN_NEW EntitySystem::EntityMgr();

	// create core states
	mLoadingScreen = DYN_NEW LoadingScreen();
	mGame = DYN_NEW Game();

	// init finished, now loading
	RequestStateChange(AS_LOADING);
	UpdateState();

	ResetStats();
}

Application::~Application()
{
	DYN_DELETE mResourceMgr;
	DYN_DELETE mGfxRenderer;
	DYN_DELETE mInputMgr;
	DYN_DELETE mEntityMgr;
	DYN_DELETE mLogMgr;

	DYN_DELETE mLoadingScreen;
	DYN_DELETE mGame;

	HideConsole();
}

void Application::runMainLoop()
{
	while (GetState() != AS_SHUTDOWN)
	{
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
			break;
		}

		// draw
		switch (GetState())
		{
		case AS_GAME:
			if (gGfxRenderer.BeginRendering())
			{
				mGame->Draw();
				gGfxRenderer.EndRendering();
			}
			break;
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
	TimesList::const_iterator it = mFrameDeltaTimes.begin();
	TimesList::const_iterator iend = mFrameDeltaTimes.end() - 2; // need at least 2 times
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
	return (float32)(mFrameDeltaTimes.back() - mFrameDeltaTimes.front()) / ((mFrameDeltaTimes.size()-1)*1000);
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
			mAvgFPS = (mAvgFPS + mLastFPS) / 2; // approximation
		mLastSecond = curTimeMillis;
		mFrameCount = 0;
	}
}

void Core::Application::ShowConsole( void )
{
	AllocConsole();

	// get hwnd
	const char* uniqueName = "3248962941235952";
	SetConsoleTitle(uniqueName);
	Sleep(40);
	mConsoleHandle = FindWindow(NULL, uniqueName);

	// set title
	SetConsoleTitle("Battleships Debug Log");

	// set position
	if (mConsoleHandle)
	{
		MoveWindow(mConsoleHandle, mConsoleX, mConsoleY, 1024, mConsoleHeight, true);
	}
}

void Core::Application::HideConsole( void )
{
	FreeConsole();
}

void Core::Application::WriteToConsole( const string& str )
{
	DWORD writtenChars = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.length(), &writtenChars, NULL);	
}