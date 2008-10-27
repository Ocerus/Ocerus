#include "Application.h"
#include <Windows.h>
#include "LoadingScreen.h"
#include "Game.h"

using namespace Core;

Application::Application(): StateMachine<eAppState>(AS_INITING), mFrameSmoothingTime(0.5f)
{
	mResourceMgr = DYN_NEW ResourceSystem::ResourceMgr("data");
	mGfxRenderer = DYN_NEW GfxSystem::GfxRenderer(GfxSystem::Point(1024,768), false);
	mInputMgr = DYN_NEW InputSystem::InputMgr();
	mEntityMgr = DYN_NEW EntitySystem::EntityMgr();

	mLoadingScreen = DYN_NEW LoadingScreen();
	mGame = DYN_NEW Game();

	RequestStateChange(AS_LOADING);
	UpdateState();
}

Application::~Application()
{
	DYN_DELETE mResourceMgr;
	DYN_DELETE mGfxRenderer;
	DYN_DELETE mInputMgr;
	DYN_DELETE mEntityMgr;

	DYN_DELETE mLoadingScreen;
	DYN_DELETE mGame;
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