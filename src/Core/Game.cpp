#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"

using namespace Core;
using namespace EntitySystem;

#define PHYSICS_TIMESTEP 1.0f
#define PHYSICS_ITERATIONS 10

Core::Game::Game(): StateMachine<eGameState>(GS_NORMAL), mPhysics(0), mHoveredEntity() {}

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

	EntityDescription entityDesc;
	ComponentDescription compDesc;

	// create a material
	entityDesc.Init(ET_UNKNOWN);
	compDesc.Init(CT_MATERIAL);
	compDesc.AddItem(1.0f); // durability ratio
	compDesc.AddItem(1.0f); // density
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle material0 = gEntityMgr.CreateEntity(entityDesc);

	// create a platform type
	entityDesc.Init(ET_UNKNOWN);
	compDesc.Init(CT_PLATFORM_PARAMS);
	compDesc.AddItem(material0);
	Vector2 shape[] = {Vector2(-1.0f,-1.0f),Vector2(0.5f,-1.0f),Vector2(1.0f,-0.5f),Vector2(1.0f,0.5f),Vector2(-1.0f,0.5f)};
	compDesc.AddItem((uint32)5); // shape length
	compDesc.AddItem(shape);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle platformType0 = gEntityMgr.CreateEntity(entityDesc);

	// create an engine type
	entityDesc.Init(ET_UNKNOWN);
	compDesc.Init(CT_ENGINE_PARAMS);
	compDesc.AddItem(material0);
	compDesc.AddItem(-0.5f*MathUtils::PI); // min angle
	compDesc.AddItem(0.5f*MathUtils::PI); // max angle
	compDesc.AddItem(1.0f); // angular speed
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle engineType0 = gEntityMgr.CreateEntity(entityDesc);

	// create free platforms
	/*entityDesc.Init();
	compDesc.Init(CT_PLATFORM_LOGIC);
	compDesc.AddItem(platformType0); // blueprints
	compDesc.AddItem(EntityHandle()); // pass null ship handle to indicate this platform is free
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_PHYSICS);
	// pass body position and rotation cos it's a free platform
	compDesc.AddItem(Vector2(5.0f,5.0f)); // position
	compDesc.AddItem(0.3f); // angle
	// pass shape info
	compDesc.AddItem(false); // flip the shape?
	compDesc.AddItem(0.0f); // shape angle relative to original shape
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_VISUAL);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle platform0 = gEntityMgr.CreateEntity(entityDesc);*/

	// create a ship
	entityDesc.Init(ET_SHIP);
	compDesc.Init(CT_SHIP_LOGIC);
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_SHIP_PHYSICS);
	// body position and angle
	compDesc.AddItem(Vector2(10.0f,10.0f));
	compDesc.AddItem(-1.0f);
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_SHIP_VISUAL);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle ship0 = gEntityMgr.CreateEntity(entityDesc);

	// create a platform and attach it to the ship
	entityDesc.Init(ET_PLATFORM);
	compDesc.Init(CT_PLATFORM_LOGIC);
	compDesc.AddItem(platformType0); // blueprints
	compDesc.AddItem(ship0); // pass our ship
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_PHYSICS);
	// pass position relative to the ship center
	compDesc.AddItem(Vector2(0.0f,-0.55f));
	// pass additional shape info
	compDesc.AddItem(false); // flip the shape?
	compDesc.AddItem(0.0f); // shape angle relative to original shape
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_VISUAL);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle platform1 = gEntityMgr.CreateEntity(entityDesc);

	// create an engine and attach it to the platform
	entityDesc.Init(ET_ENGINE);
	compDesc.Init(CT_PLATFORM_ITEM);
	compDesc.AddItem(engineType0); // blueprints
	compDesc.AddItem(platform1); // parent
	compDesc.AddItem(Vector2(-1.0f, 0.0f)); // position relative to the platform
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_ENGINE);
	compDesc.AddItem(MathUtils::PI); // default angle
	compDesc.AddItem(0.0f); // relative angle
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle engine0 = gEntityMgr.CreateEntity(entityDesc);

	// create another platform and attach it to the ship
	entityDesc.Init(ET_PLATFORM);
	compDesc.Init(CT_PLATFORM_LOGIC);
	compDesc.AddItem(platformType0); // blueprints
	compDesc.AddItem(ship0); // pass our ship
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_PHYSICS);
	// pass position relative to the ship center
	compDesc.AddItem(Vector2(0.0f,0.55f));
	// pass additional shape info
	compDesc.AddItem(true); // flip the shape?
	compDesc.AddItem(0.0f); // shape angle relative to original shape
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_PLATFORM_VISUAL);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle platform2 = gEntityMgr.CreateEntity(entityDesc);

	// create another engine and attach it to the second platform
	entityDesc.Init(ET_ENGINE);
	compDesc.Init(CT_PLATFORM_ITEM);
	compDesc.AddItem(engineType0); // blueprints
	compDesc.AddItem(platform2); // parent
	compDesc.AddItem(Vector2(-1.0f, 0.0f)); // position relative to the platform
	entityDesc.AddComponentDescription(compDesc);
	compDesc.Init(CT_ENGINE);
	compDesc.AddItem(MathUtils::PI); // default angle
	compDesc.AddItem(0.0f); // relative angle
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle engine1 = gEntityMgr.CreateEntity(entityDesc);

	// link platforms together
	entityDesc.Init(ET_UNKNOWN);
	compDesc.Init(CT_PLATFORM_LINKS);
	compDesc.AddItem(platform1); // first platform
	compDesc.AddItem(platform2); // second platform
	compDesc.AddItem((uint32)3); // number of links
	Vector2 anchors1[] = {Vector2(-0.5f,0.45f),Vector2(0.0f,0.45f),Vector2(0.5f,0.45f)};
	Vector2 anchors2[] = {Vector2(-0.5f,-0.45f),Vector2(0.0f,-0.45f),Vector2(0.5f,-0.45f)};
	compDesc.AddItem(anchors1);
	compDesc.AddItem(anchors2);
	entityDesc.AddComponentDescription(compDesc);
	gEntityMgr.CreateEntity(entityDesc);

	// recompute mass of the ship's body
	ship0.PostMessage(EntityMessage::TYPE_PHYSICS_UPDATE_MASS);





	gResourceMgr.AddResourceFileToGroup("zazaka.png", "test");
	gResourceMgr.LoadResourcesInGroup("test");

	gInputMgr.AddInputListener(this);

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

	InputSystem::MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_MOUSE_PICK, &picker));
	mHoveredEntity = picker.GetResult();

	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_SERVER));
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_CLIENT));
	mPhysics->Step(PHYSICS_TIMESTEP, PHYSICS_ITERATIONS);
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

	//TODO predtim by se jeste melo poslat TYPE_DRAW_SELECTED "vybranym" entitam. Ty si zavolaji Draw s parametrem "selected"
	//	nastavenym na true a poznaci si, ze pri zprave TYPE_DRAW se nemaji vykreslit (a tento priznak zrusi). Nutno jen
	//	pro entity, kterym nestaci overlay pro selection

	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW));

	// draw selection overlays above selected entities
	if (mHoveredEntity.IsValid())
		mHoveredEntity.PostMessage(EntityMessage::TYPE_DRAW_HOVER_OVERLAY);
}

void Core::Game::KeyPressed( const InputSystem::KeyInfo& ke )
{

}

void Core::Game::KeyReleased( const InputSystem::KeyInfo& ke )
{

}

void Core::Game::MouseMoved( const InputSystem::MouseInfo& mi )
{

}

void Core::Game::MouseButtonPressed( const InputSystem::eMouseButton btn )
{

}

void Core::Game::MouseButtonReleased( const InputSystem::eMouseButton btn )
{

}