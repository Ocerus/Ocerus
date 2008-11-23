#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"

using namespace Core;

Core::Game::Game(): StateMachine<eGameState>(GS_NORMAL), mPhysics(0) {}

Core::Game::~Game()
{
	Deinit();
}

void Core::Game::Init()
{
	gLogMgr.LogMessage("Game init");

	// init physics engine
	b2AABB worldAABB;
	//TODO chtelo by to nekonecny rozmery, nebo vymyslet nejak jinak
	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(100.0f, 100.0f);
	// turn off sleeping as we are moving in a space with no gravity
	mPhysics = DYN_NEW b2World(worldAABB, b2Vec2(0.0f, 0.0f), false);

	//// TEST ////

	EntitySystem::EntityDescription entityDesc;
	EntitySystem::ComponentDescription compDesc;

	// create a materials
	entityDesc.Reset();
	compDesc.Init(EntitySystem::CT_MATERIAL);
	compDesc.AddItem(1.0f); // durability ratio
	compDesc.AddItem(1.0f); // density
	entityDesc.AddComponentDescription(compDesc);
	EntitySystem::EntityHandle material0 = gEntityMgr.CreateEntity(entityDesc);

	// create a platform types
	entityDesc.Reset();
	compDesc.Init(EntitySystem::CT_PLATFORM_PARAMS);
	compDesc.AddItem(material0);
	Vector2 shape[] = {Vector2(-1.0f,-1.0f),Vector2(0.5f,-1.0f),Vector2(1.0f,-0.5f),Vector2(1.0f,1.0f),Vector2(-1.0f,1.0f)};
	compDesc.AddItem((uint32)5); // shape length
	compDesc.AddItem(shape);
	entityDesc.AddComponentDescription(compDesc);
	EntitySystem::EntityHandle platformType0 = gEntityMgr.CreateEntity(entityDesc);

	// create free platforms
	entityDesc.Reset();
	compDesc.Init(EntitySystem::CT_PLATFORM_STATS);
	compDesc.AddItem(platformType0); // blueprints
	compDesc.AddItem(EntitySystem::EntityHandle()); // pass null ship handle to indicate this platform is free
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(EntitySystem::CT_PLATFORM_PHYSICS);
	// pass body position and rotation cos it's a free platform
	compDesc.AddItem(Vector2(5.0f,5.0f)); // position
	compDesc.AddItem(0.0f); // angle
	// pass shape info
	compDesc.AddItem(false); // flip the shape?
	compDesc.AddItem(0.0f); // shape angle relative to original shape
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(EntitySystem::CT_PLATFORM_VISUAL);
	entityDesc.AddComponentDescription(compDesc);
	EntitySystem::EntityHandle platform0 = gEntityMgr.CreateEntity(entityDesc);

	// create a ships


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
		gApp.Shutdown();
	if (gInputMgr.IsKeyDown(InputSystem::KC_G))
		gApp.RequestStateChange(AS_GUI, true);

	gEntityMgr.BroadcastMessage(EntitySystem::EntityMessage(EntitySystem::EntityMessage::TYPE_UPDATE_PHYSICS_SERVER));
	gEntityMgr.BroadcastMessage(EntitySystem::EntityMessage(EntitySystem::EntityMessage::TYPE_UPDATE_PHYSICS_CLIENT));
}

void Core::Game::Draw()
{
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));
	uint8 topLeft = GfxSystem::ANCHOR_TOP | GfxSystem::ANCHOR_LEFT;
	uint8 centre = GfxSystem::ANCHOR_VCENTER | GfxSystem::ANCHOR_HCENTER;	
	gGfxRenderer.DrawImage(gResourceMgr.GetResource("test/zazaka.png"),0, 0,topLeft);
	gGfxRenderer.DrawImage(gResourceMgr.GetResource("test/zazaka.png"),100, 50,centre,0,100);	
	/*
	// creatig pentagon
	std::vector<GfxSystem::Point> test;
	test.push_back(GfxSystem::Point(300,300));
	test.push_back(GfxSystem::Point(350,300));
	test.push_back(GfxSystem::Point(350,400));
	test.push_back(GfxSystem::Point(325,450));
	test.push_back(GfxSystem::Point(250,380));
	// rendering pentagon
	gGfxRenderer.DrawPolygon(test,GfxSystem::Color(255,135,0));*/

	gEntityMgr.BroadcastMessage(EntitySystem::EntityMessage(EntitySystem::EntityMessage::TYPE_DRAW));
}

