#include "Game.h"
#include "../Common.h"
#include "Application.h"

using namespace Core;

Core::Game::Game(): StateMachine<eGameState>(GS_NORMAL) {}

Core::Game::~Game()
{
	Deinit();
}

void Core::Game::Init()
{
	gLogMgr.LogMessage("Game init");

	gResourceMgr.AddResourceFileToGroup("zazaka.png", "test");
	gResourceMgr.LoadResourcesInGroup("test");

	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");
}

void Core::Game::Deinit()
{

}

void Core::Game::Update( float32 delta )
{
	if (gInputMgr.IsKeyDown(InputSystem::KC_ESCAPE))
		gApp.RequestStateChange(AS_SHUTDOWN, true);
	if (gInputMgr.IsKeyDown(InputSystem::KC_G))
		gApp.RequestStateChange(AS_GUI, true);
}

void Core::Game::Draw()
{
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));
	gGfxRenderer.DrawLine(0,0,100,100,GfxSystem::Pen(GfxSystem::Color(255,0,0)));
	gGfxRenderer.DrawImage(gResourceMgr.GetResource("test/zazaka.png"), 50, 100);
}

