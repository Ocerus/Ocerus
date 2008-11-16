#include "Common.h"
#include "Game.h"
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

	gStringMgr.LoadLanguagePack();

	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");

	gLogMgr.LogMessage("-- STRING MANAGER DEMO START --");
	gLogMgr.LogMessage(gStringMgr.GetTextData("MAIN_MENU"));
	gLogMgr.LogMessage(gStringMgr.GetTextData("QUIT"));
	gLogMgr.LogMessage(gStringMgr.GetTextData("HELLO"));
	gLogMgr.LogMessage(gStringMgr.GetTextData("MONDAY"));
	gLogMgr.LogMessage(gStringMgr.GetTextData("ABOUT"));
	gLogMgr.LogMessage(gStringMgr.GetTextData("CZECH_TEXT"));
	gLogMgr.LogMessage("-- STRING MANAGER DEMO END --");
}

void Core::Game::Deinit()
{

}

void Core::Game::Update( float32 delta )
{
	if (gInputMgr.IsKeyDown(InputSystem::KC_ESCAPE))
		gApp.Shutdown();
	if (gInputMgr.IsKeyDown(InputSystem::KC_G)) {
		gApp.RequestStateChange(AS_GUI, true);
	}
}

void Core::Game::Draw()
{
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));
	gGfxRenderer.DrawLine(0,0,100,100,GfxSystem::Pen(GfxSystem::Color(255,0,0)));		
	gGfxRenderer.DrawImage(gResourceMgr.GetResource("test/zazaka.png"),0, 50,100,0);
	gGfxRenderer.DrawImage(gResourceMgr.GetResource("test/zazaka.png"),100, 50,100,0,130);
	GfxSystem::Rect textureRect(20,35,10,10);
	GfxSystem::Rect destRect(0,200,200,200);
	// creatig pentagon
	std::vector<GfxSystem::Point> test;
	test.push_back(GfxSystem::Point(300,300));
	test.push_back(GfxSystem::Point(350,300));
	test.push_back(GfxSystem::Point(350,400));
	test.push_back(GfxSystem::Point(325,450));
	test.push_back(GfxSystem::Point(250,380));
	// rendering pentagon
	gGfxRenderer.DrawPolygon(test,GfxSystem::Color(255,135,0));
}

