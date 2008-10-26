#include "Game.h"
#include "../Common.h"

using namespace Core;

Core::Game::Game(): StateMachine<eGameState>(GS_NORMAL) {}

Core::Game::~Game()
{
	Deinit();
}

void Core::Game::Init()
{

}

void Core::Game::Deinit()
{

}

void Core::Game::Update( float32 delta )
{

}

void Core::Game::Draw()
{
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));
}

