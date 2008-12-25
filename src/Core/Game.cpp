#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"
#include <iostream>
#include "../GfxSystem/ParticleResource.h"
///@name Included for particle manager demo rand() function.
#include <cstdlib>


using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

#define PHYSICS_TIMESTEP 0.016f
#define PHYSICS_ITERATIONS 10
#define ENGINE_VISIBLE_MAX_POWER 80
#define CAMERA_SPEED_RATIO 10.0f
#define CAMERA_SCALE_RATIO 0.001f
#define WATER_TEXTURE_SCALE 0.01f
#define ENGINE_ANGLECHANGE_SPEED 2.0f
#define ENGINE_POWERCHANGE_SPEED 1.0f

Core::Game::Game(): StateMachine<eGameState>(GS_NORMAL), mPhysics(0) {}

Core::Game::~Game()
{
	Deinit();
}

void Core::Game::Init()
{
	gLogMgr.LogMessage("Game init");

	// basic init stuff
	mHoveredEntity.Invalidate();

	// init physics engine
	b2AABB worldAABB;
	//TODO chtelo by to nekonecny rozmery, nebo vymyslet nejak jinak
	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(100.0f, 100.0f);
	// turn off sleeping as we are moving in a space with no gravity
	mPhysics = DYN_NEW b2World(worldAABB, b2Vec2(0.0f, 0.0f), false);
	mPhysicsResidualDelta = 0.0f;



	//// @name TEST ////
	EntityDescription desc;
	PropertyList props;

	// create a material
	desc.Init(ET_UNKNOWN);
	desc.AddComponent(CT_MATERIAL);
	EntityHandle material0 = gEntityMgr.CreateEntity(desc, props);
	props["DurabilityRatio"].SetValue(1.0f);
	props["Density"].SetValue(1.0f);
	material0.FinishInit();

	// create a platform type
	desc.Init(ET_UNKNOWN);
	desc.AddComponent(CT_PLATFORM_PARAMS);
	EntityHandle platformType0 = gEntityMgr.CreateEntity(desc, props);
	props["Material"].SetValue(material0);
	Vector2 shape[] = {Vector2(-0.5f,-0.5f),Vector2(0.25f,-0.5f),Vector2(0.5f,-0.25f),Vector2(0.5f,0.25f),Vector2(-0.5f,0.25f)};
	props["ShapeLength"].SetValue((uint32)5);
	props["Shape"].SetValue(shape);
	platformType0.FinishInit();

	// create an engine type
	desc.Init(ET_UNKNOWN);
	desc.AddComponent(CT_ENGINE_PARAMS);
	EntityHandle engineType0 = gEntityMgr.CreateEntity(desc, props);
	props["Material"].SetValue(material0);
	props["ArcAngle"].SetValue(0.5f*MathUtils::PI);
	props["StabilizationRatio"].SetValue((uint32)1000);
	//TODO predelat char*
	props["Texture"].SetValue<char*>("ShipParts/engine0.png");
	props["TextureScale"].SetValue(0.5f);
	engineType0.FinishInit();

	// create a free platform
	/*desc.Init(ET_PLATFORM);
	desc.AddComponent(CT_PLATFORM_PHYSICS);
	desc.AddComponent(CT_PLATFORM_LOGIC);
	desc.AddComponent(CT_PLATFORM_VISUAL);
	EntityHandle platform0 = gEntityMgr.CreateEntity(desc, props);
	props["Blueprints"].SetValue(platformType0);
	props["InitBodyPosition"].SetValue(Vector2(5.0f,5.0f));
	props["InitBodyAngle"].SetValue(0.3f);
	platform0.FinishInit();*/

	// create a ship
	desc.Init(ET_SHIP);
	desc.AddComponent(CT_SHIP_PHYSICS);
	desc.AddComponent(CT_SHIP_LOGIC);
	desc.AddComponent(CT_SHIP_VISUAL);
	EntityHandle ship0 = gEntityMgr.CreateEntity(desc, props);
	props["InitBodyPosition"].SetValue(Vector2(10.0f, 10.0f));
	props["InitBodyAngle"].SetValue(-0.3f*MathUtils::PI);
	ship0.FinishInit();

	// create a platform and attach it to the ship
	desc.Init(ET_PLATFORM);
	desc.AddComponent(CT_PLATFORM_PHYSICS);
	desc.AddComponent(CT_PLATFORM_LOGIC);
	desc.AddComponent(CT_PLATFORM_VISUAL);
	EntityHandle platform1 = gEntityMgr.CreateEntity(desc, props);
	props["Blueprints"].SetValue(platformType0);
	props["ParentShip"].SetValue(ship0);
	props["RelativePosition"].SetValue<Vector2&>(Vector2(0.0f,-0.25f));
	props["InitShapeFlip"].SetValue(false);
	props["InitShapeAngle"].SetValue(0.0f);
	platform1.FinishInit();

	// create an engine and attach it to the platform
	desc.Init(ET_ENGINE);
	desc.AddComponent(CT_PLATFORM_ITEM);
	desc.AddComponent(CT_ENGINE);
	EntityHandle engine0 = gEntityMgr.CreateEntity(desc, props);
	props["Blueprints"].SetValue(engineType0);
	props["ParentPlatform"].SetValue(platform1);
	props["RelativePosition"].SetValue<Vector2&>(Vector2(-0.5f, -0.25f));
	props["DefaultAngle"].SetValue(MathUtils::PI);
	props["RelativeAngle"].SetValue(0.0f);
	engine0.FinishInit();

	// create another platform and attach it to the ship
	desc.Init(ET_PLATFORM);
	desc.AddComponent(CT_PLATFORM_PHYSICS);
	desc.AddComponent(CT_PLATFORM_LOGIC);
	desc.AddComponent(CT_PLATFORM_VISUAL);
	EntityHandle platform2 = gEntityMgr.CreateEntity(desc, props);
	props["Blueprints"].SetValue(platformType0);
	props["ParentShip"].SetValue(ship0);
	props["RelativePosition"].SetValue<Vector2&>(Vector2(0.0f,0.25f));
	props["InitShapeFlip"].SetValue(true);
	props["InitShapeAngle"].SetValue(0.0f);
	platform2.FinishInit();

	// create another engine and attach it to the second platform
	desc.Init(ET_ENGINE);
	desc.AddComponent(CT_PLATFORM_ITEM);
	desc.AddComponent(CT_ENGINE);
	EntityHandle engine1 = gEntityMgr.CreateEntity(desc, props);
	props["Blueprints"].SetValue(engineType0);
	props["ParentPlatform"].SetValue(platform2);
	props["RelativePosition"].SetValue<Vector2&>(Vector2(-0.5f, 0.25f));
	props["DefaultAngle"].SetValue(MathUtils::PI);
	props["RelativeAngle"].SetValue(0.0f);
	engine1.FinishInit();

	// link platforms together
	desc.Init(ET_UNKNOWN);
	desc.AddComponent(CT_PLATFORM_LINKS);
	EntityHandle links1To2 = gEntityMgr.CreateEntity(desc, props);
	props["FirstPlatform"].SetValue(platform1);
	props["SecondPlatform"].SetValue(platform2);
	props["NumLinks"].SetValue<uint32>(3);
	Vector2 anchors1[] = {Vector2(-0.25f,0.2f),Vector2(0.0f,0.2f),Vector2(0.25f,0.2f)};
	props["FirstAnchors"].SetValue(anchors1);
	Vector2 anchors2[] = {Vector2(-0.25f,-0.2f),Vector2(0.0f,-0.2f),Vector2(0.25f,-0.2f)};
	props["SecondAnchors"].SetValue(anchors2);
	links1To2.FinishInit();

	// recompute mass of the ship's body
	ship0.PostMessage(EntityMessage::TYPE_PHYSICS_UPDATE_MASS);



	// set camera
	Vector2 shipPos;
	ship0.PostMessage(EntityMessage::TYPE_GET_POSITION, &shipPos);
	gGfxRenderer.SetCameraPos(shipPos);
	//gGfxRenderer.SetCameraX(50.0f* gGfxRenderer.GetScreenWidthHalf());
	//gGfxRenderer.SetCameraY(50.0f* gGfxRenderer.GetScreenHeightHalf());
	gGfxRenderer.SetCameraScale(50.0f);
	mCameraFocus.Invalidate();

	gInputMgr.AddInputListener(this);

	////////////////// @name XML MANAGER DEMO START //////////////////
	gLogMgr.LogMessage("*** XML MANAGER DEMO START ***");
	ResourceSystem::XMLResourcePtr test = gResourceMgr.GetResource("xml", "test.xml");

	// Set rootnode
	test->Enter("Powerups/Powerup/g_PowerUps");
	test->Enter("entry1");

	std::stringstream ss;
	ss << "TEST ";
	ss << test->GetAttribute<string>("szName") << " ";
	int move = test->GetAttribute<int>("iFastMove");
	move++;
	ss << move << " ";
	gLogMgr.LogMessage(ss.str());
	
	// absolute path specified
	test->Enter("", ResourceSystem::XMLResource::ABS);
	int jump = test->GetAttribute<int>("Powerups/Powerup/g_PowerUps/entry2/iHighJump");	
	ss.clear();
	ss << jump << " ";
	gLogMgr.LogMessage(ss.str());

	test->Enter("Powerups/Powerup/g_PowerUps/entry2");
	// leave 2 levels up
	test->Leave(2);
	test->Enter("g_PowerUps/entry3");
	int inv = test->GetAttribute<int>("iInvulnerability");
	ss.clear();
	ss << inv << " ";
	gLogMgr.LogMessage(ss.str());

	
	// XMLRES ITERATOR DEMO
	// Iterator iterates through current RootNode and return all immediate values
	ss.clear();
	for (ResourceSystem::XMLResource::node_iterator it = test->begin(); it != test->end(); it++)
	{
		gLogMgr.LogMessage("INSIDE LOOP " + it.GetName());
		ss << it.GetName() << " ";
	}
	gLogMgr.LogMessage("ITERATOR END: " + ss.str());
	gLogMgr.LogMessage("*** XML MANAGER DEMO END ***");
	////////////////// @name XML MANAGER DEMO END //////////////////

	////////////////// @name PARTICLE DEMO START ///////////////////
	gPSMgr.SpawnPS("engine1.psi", 100, 100);
	gPSMgr.SpawnPS("gunfire.psi", 100, 300);
	GfxSystem::ParticleSystemPtr x = gPSMgr.SpawnPS("bullets.psi", 100, 300);
	x->SetAngle(MathUtils::PI/2);
	x->SetSpeed(500,500);	
	////////////////// @name PARTICLE DEMO END /////////////////////

	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");
}

void Core::Game::Deinit()
{

}

void Core::Game::Update( const float32 delta )
{
	//// @name Input reactions ////

	if (gInputMgr.IsKeyDown(KC_ESCAPE))
		gApp.Shutdown();
	/*
	if (gInputMgr.IsKeyDown(InputSystem::KC_G)) {
		gGUIMgr.LoadGUI();
		gApp.RequestStateChange(AS_GUI, true);
	}
	*/

	// pick hover entity
	MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_MOUSE_PICK, &picker));
	mHoveredEntity = picker.GetResult();

	// we want to do certain action even when the right button is still down
	if (gInputMgr.IsMouseButtonPressed(MBTN_RIGHT) && mSelectedEntities.size()>0 
		&& mSelectedEntities[0].GetType()==ET_ENGINE)
		MouseButtonPressed(mouse, MBTN_RIGHT);

	// control engines by using keys
	if (mSelectedEntities.size()>0 && mSelectedEntities[0].GetType()==ET_ENGINE)
	{
		if (gInputMgr.IsKeyDown(KC_NUMPAD4) || gInputMgr.IsKeyDown(KC_NUMPAD6))
		{
			for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
			{
				float32 angle;
				i->PostMessage(EntityMessage::TYPE_GET_RELATIVE_ANGLE, &angle);
				angle += (gInputMgr.IsKeyDown(KC_NUMPAD4)?-1:1) * ENGINE_ANGLECHANGE_SPEED * delta;
				i->PostMessage(EntityMessage::TYPE_SET_RELATIVE_ANGLE, &angle);
			}
		}
		if (gInputMgr.IsKeyDown(KC_NUMPAD8) || gInputMgr.IsKeyDown(KC_NUMPAD5))
		{
			for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
			{
				float32 powerRatio;
				i->PostMessage(EntityMessage::TYPE_GET_POWER_RATIO, &powerRatio);
				powerRatio += (gInputMgr.IsKeyDown(KC_NUMPAD5)?-1:1) * ENGINE_POWERCHANGE_SPEED * delta;
				i->PostMessage(EntityMessage::TYPE_SET_POWER_RATIO, &powerRatio);
			}
		}
	}


	float32 physicsDelta = delta + mPhysicsResidualDelta;
	while (physicsDelta > PHYSICS_TIMESTEP)
	{
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_SERVER));
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_CLIENT));
		mPhysics->Step(PHYSICS_TIMESTEP, PHYSICS_ITERATIONS);
		physicsDelta -= PHYSICS_TIMESTEP;
	}
	mPhysicsResidualDelta = physicsDelta;

	//particle effects
	gPSMgr.Update(delta);
};

void Core::Game::Draw( const float32 delta)
{
	// move camera in reaction to the user input
	if (gInputMgr.IsKeyDown(KC_LEFT))
	{
		gGfxRenderer.MoveCamera(-CAMERA_SPEED_RATIO * delta, 0.0f);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_RIGHT))
	{
		gGfxRenderer.MoveCamera(CAMERA_SPEED_RATIO * delta, 0.0f);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_UP))
	{
		gGfxRenderer.MoveCamera(0.0f, -CAMERA_SPEED_RATIO * delta);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_DOWN))
	{
		gGfxRenderer.MoveCamera(0.0f, CAMERA_SPEED_RATIO * delta);
		mCameraFocus.Invalidate();
	}

	if (mCameraFocus.IsValid())
	{
		Vector2 pos;
		mCameraFocus.PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
		gGfxRenderer.SetCameraPos(pos);
	}

	// clear the screen
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));

	// draw the water
	GfxSystem::TexturePtr waterTex = gResourceMgr.GetResource("Backgrounds", "water.png");
	float32 texW_ws = WATER_TEXTURE_SCALE * waterTex->GetWidth();
	float32 texH_ws = WATER_TEXTURE_SCALE * waterTex->GetHeight();
	int32 texW = gGfxRenderer.WorldToScreenScalar(texW_ws);
	int32 texH = gGfxRenderer.WorldToScreenScalar(texH_ws);
	Vector2 topleft = gGfxRenderer.GetCameraWorldBoxTopLeft();
	int32 x, y;
	x = - gGfxRenderer.WorldToScreenScalar(topleft.x - MathUtils::Floor(topleft.x / texW_ws) * texW_ws);
	y = - gGfxRenderer.WorldToScreenScalar(topleft.y - MathUtils::Floor(topleft.y / texH_ws) * texH_ws);
	int32 screenW = gGfxRenderer.GetScreenWidth();
	int32 screenH = gGfxRenderer.GetScreenHeight();
	int32 oldY = y;
	for (; x<screenW; x+=texW)
		for (y=oldY; y<screenH; y+=texH)
			gGfxRenderer.DrawImage(waterTex, GfxSystem::Rect(x, y, texW, texH));


	// draw ships
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM));
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM_LINK));

	// draw direction lines to the mouse cursor
	if (mSelectedEntities.size() > 0)
	{
		MouseState& mouse = gInputMgr.GetMouseState();
		for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		{
			if (i->GetType() == ET_ENGINE)
			{
				GfxSystem::Pen pen(GfxSystem::Color(100,100,100,180));
				// if ALT is pressed, we want to set the direction from the ship center instead of the engine center
				if (gInputMgr.IsKeyDown(KC_LMENU) || gInputMgr.IsKeyDown(KC_RMENU))
				{
					if (i == mSelectedEntities.begin())
					{
						EntityHandle platform;
						i->PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
						EntityHandle ship;
						platform.PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
						Vector2 pos;
						ship.PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
						gGfxRenderer.DrawLine(mouse.x, mouse.y, gGfxRenderer.WorldToScreenX(pos.x), gGfxRenderer.WorldToScreenY(pos.y), pen);
					}
				}
				else
				{
					Vector2 pos;
					i->PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
					gGfxRenderer.DrawLine(mouse.x, mouse.y, gGfxRenderer.WorldToScreenX(pos.x), gGfxRenderer.WorldToScreenY(pos.y), pen);
				}
			}
		}
	}

	bool hoverAlsoSelected = false;
	bool hover = false;

	// draw underlays
	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
	{
		// calling via EntityMgr to avoid ignored messages
		gEntityMgr.PostMessage(*i, EntityMessage(EntityMessage::TYPE_DRAW_UNDERLAY, &hover));
		if (mHoveredEntity == *i)
			hoverAlsoSelected = true;
	}

	// draw underlay of a hovered entity
	hover = true;
	if (!hoverAlsoSelected && mHoveredEntity.IsValid())
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::TYPE_DRAW_UNDERLAY, &hover));

	// draw entities
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM_ITEM));

	// draw overlays
	hover = false;
	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		gEntityMgr.PostMessage(*i, EntityMessage(EntityMessage::TYPE_DRAW_OVERLAY, &hover));

	// draw overlay of a hovered entity
	hover = true;
	if (!hoverAlsoSelected && mHoveredEntity.IsValid())
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::TYPE_DRAW_OVERLAY, &hover));
	
	//particle effects
	gPSMgr.SetScale(gGfxRenderer.GetCameraScale()/50); //scale divided by 50 to compensate default zoom value
	gPSMgr.Render();
}

void Core::Game::KeyPressed( const KeyInfo& ke )
{
	/*if (ke.keyAction == KC_F)
	{
		gGfxRenderer.SetFullscreen(!gGfxRenderer.IsFullscreen());
	}
	if (ke.keyAction==KC_D)
	{
		gGfxRenderer.ChangeResolution(800,600);
	}*/
}

void Core::Game::KeyReleased( const KeyInfo& ke )
{

}

void Core::Game::MouseMoved( const MouseInfo& mi )
{
	// zoom camera
	if (mi.wheelDelta)
		gGfxRenderer.ZoomCamera(CAMERA_SCALE_RATIO * gGfxRenderer.GetCameraScale() * mi.wheelDelta);
}

void Core::Game::MouseButtonPressed( const MouseInfo& mi, const eMouseButton btn )
{
	if (btn == MBTN_LEFT)
	{
		// use the hover entity as a focus
		if (gInputMgr.IsKeyDown(KC_RCONTROL) || gInputMgr.IsKeyDown(KC_LCONTROL))
		{
			if (mHoveredEntity.IsValid())
				mCameraFocus = mHoveredEntity;
		}
		// add to the current selection if SHIFT down
		else if (mSelectedEntities.size()>0 && (gInputMgr.IsKeyDown(KC_RSHIFT) || gInputMgr.IsKeyDown(KC_LSHIFT)))
		{
			if (mHoveredEntity.IsValid() && mHoveredEntity.GetType() == mSelectedEntities[0].GetType())
				mSelectedEntities.push_back(mHoveredEntity);
		}
		// clear the selection and add the hovered one if any
		else
		{
			mSelectedEntities.clear();
			if (mHoveredEntity.IsValid())
				mSelectedEntities.push_back(mHoveredEntity);
		}
		///////////////////// @name PARTICLE DEMO 2 START ////////////////
		//gPSMgr.SpawnPS("explosion.psi", (float)mi.x, (float)mi.y);
		srand((unsigned int)time(NULL));
		// 4-vybuch :P //includujem cstdlib kvoli rand, po odstraneni dema odstranit aj include
		gPSMgr.SpawnPS("explosion.psi", (float)(mi.x - 50 + (rand() % 100)), (float)(mi.y - 50 + (rand() % 100)));
		gPSMgr.SpawnPS("explosion.psi", (float)(mi.x - 50 + (rand() % 100)), (float)(mi.y - 50 + (rand() % 100)));
		gPSMgr.SpawnPS("explosion.psi", (float)(mi.x - 50 + (rand() % 100)), (float)(mi.y - 50 + (rand() % 100)));
		gPSMgr.SpawnPS("explosion.psi", (float)(mi.x - 50 + (rand() % 100)), (float)(mi.y - 50 + (rand() % 100)));	
		std::stringstream ss;
		ss << gPSMgr.GetNum();
		gLogMgr.LogMessage("Particle systems alive: " + ss.str());
		///////////////////// @name PARTICLE DEMO 2 END ////////////////

	}
	else if (btn == MBTN_RIGHT)
	{
		if (mSelectedEntities.size() > 0)
		{
			// set the engine power and angle if selected
			Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
			for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
			{
				if (i->GetType() == ET_ENGINE)
				{
					Vector2 pos;
					// if ALT is pressed, we want to set the direction from the ship center instead of the engine center
					if (gInputMgr.IsKeyDown(KC_LMENU) || gInputMgr.IsKeyDown(KC_RMENU))
					{
						EntityHandle platform;
						i->PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
						EntityHandle ship;
						platform.PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
						ship.PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
					}
					else
					{
						i->PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
					}
					Vector2 dir = cursor - pos;
					float32 angle = MathUtils::Angle(dir);
					i->PostMessage(EntityMessage::TYPE_SET_ANGLE, &angle);
					// get back the clamped value
					i->PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
					Vector2 unitDir = MathUtils::VectorFromAngle(angle);
					// project the target vector onto the correct direction vector
					dir = MathUtils::Dot(dir, unitDir) * unitDir;

					int32 screenDist = gGfxRenderer.WorldToScreenScalar(dir.Length());
					float32 powerRatio = (float32)screenDist / (float32)GetEnginePowerCircleRadius();
					i->PostMessage(EntityMessage::TYPE_SET_POWER_RATIO, &powerRatio);
				}
			}
		}
	}
}

void Core::Game::MouseButtonReleased( const MouseInfo& mi, const eMouseButton btn )
{

}

int32 Core::Game::GetEnginePowerCircleRadius( void ) const
{
	return ENGINE_VISIBLE_MAX_POWER;
}
