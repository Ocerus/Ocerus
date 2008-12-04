#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"

using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

#define PHYSICS_TIMESTEP 0.016f
#define PHYSICS_ITERATIONS 10
#define ENGINE_VISIBLE_MAX_POWER 80
#define CAMERA_SPEED_RATIO 10.0f
#define CAMERA_SCALE_RATIO 0.001f
#define WATER_TEXTURE_SCALE 0.01f

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
	Vector2 shape[] = {Vector2(-0.5f,-0.5f),Vector2(0.25f,-0.5f),Vector2(0.5f,-0.25f),Vector2(0.5f,0.25f),Vector2(-0.5f,0.25f)};
	compDesc.AddItem((uint32)5); // shape length
	compDesc.AddItem(shape);
	entityDesc.AddComponentDescription(compDesc);
	EntityHandle platformType0 = gEntityMgr.CreateEntity(entityDesc);

	// create an engine type
	entityDesc.Init(ET_UNKNOWN);
	compDesc.Init(CT_ENGINE_PARAMS);
	compDesc.AddItem(material0);
	compDesc.AddItem(0.5f*MathUtils::PI); // arc angle
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
	compDesc.AddItem(-0.3f*MathUtils::PI);
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
	compDesc.AddItem(Vector2(0.0f,-0.25f));
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
	compDesc.AddItem(Vector2(-0.5f, -0.25f)); // position relative to the platform
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
	compDesc.AddItem(Vector2(0.0f,0.25f));
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
	compDesc.AddItem(Vector2(-0.5f, 0.25f)); // position relative to the platform
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
	Vector2 anchors1[] = {Vector2(-0.25f,0.2f),Vector2(0.0f,0.2f),Vector2(0.25f,0.2f)};
	Vector2 anchors2[] = {Vector2(-0.25f,-0.2f),Vector2(0.0f,-0.2f),Vector2(0.25f,-0.2f)};
	compDesc.AddItem(anchors1);
	compDesc.AddItem(anchors2);
	entityDesc.AddComponentDescription(compDesc);
	gEntityMgr.CreateEntity(entityDesc);

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

	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");
}

void Core::Game::Deinit()
{

}

void Core::Game::Update( const float32 delta )
{
	if (gInputMgr.IsKeyDown(KC_ESCAPE))
		gApp.Shutdown();
	if (gInputMgr.IsKeyDown(InputSystem::KC_G)) {
		gGUIMgr.LoadGUI();
		gApp.RequestStateChange(AS_GUI, true);
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
}

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

	if (mSelectedEntities.size() > 0)
	{
		// display selection of engines
		if (mSelectedEntities[0].GetType() == ET_ENGINE)
		{
			MouseState& mouse = gInputMgr.GetMouseState();
			GfxSystem::Pen pen(GfxSystem::Color(100,100,100,180));
			Vector2 pos;
			for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
			{
				assert(i->GetType()==ET_ENGINE);
				i->PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
				gGfxRenderer.DrawLine(mouse.x, mouse.y, gGfxRenderer.WorldToScreenX(pos.x), gGfxRenderer.WorldToScreenY(pos.y), pen);
			}
		}
	}

	bool hoverAlsoSelected = false;
	bool hover = false;
	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
	{
		// calling via EntityMgr to avoid ignored messages
		gEntityMgr.PostMessage(*i, EntityMessage(EntityMessage::TYPE_DRAW_UNDERLAY, &hover));
		if (mHoveredEntity == *i)
			hoverAlsoSelected = true;
	}

	hover = true;
	if (!hoverAlsoSelected && mHoveredEntity.IsValid())
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::TYPE_DRAW_UNDERLAY, &hover));

	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM_ITEM));

	hover = false;
	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		gEntityMgr.PostMessage(*i, EntityMessage(EntityMessage::TYPE_DRAW_OVERLAY, &hover));

	hover = true;
	if (!hoverAlsoSelected && mHoveredEntity.IsValid())
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::TYPE_DRAW_OVERLAY, &hover));

}

void Core::Game::KeyPressed( const KeyInfo& ke )
{

}

void Core::Game::KeyReleased( const KeyInfo& ke )
{

}

void Core::Game::MouseMoved( const MouseInfo& mi )
{
	// zoom camera
	if (mi.wheelDelta)
		gGfxRenderer.ZoomCamera(CAMERA_SCALE_RATIO * gGfxRenderer.GetCameraScale() * mi.wheelDelta);

	// pick hover entity
	EntityPicker picker(mi.x, mi.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_MOUSE_PICK, &picker));
	mHoveredEntity = picker.GetResult();

	// we want to do certain action even when the right button is still down
	MouseState& mouse = gInputMgr.GetMouseState();
	if ((mouse.buttons & MBTN_RIGHT) && mSelectedEntities.size()>0 
		&& mSelectedEntities[0].GetType()==ET_ENGINE)
		MouseButtonPressed(mi, MBTN_RIGHT);

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
	}
	else if (btn == MBTN_RIGHT)
	{
		if (mSelectedEntities.size() > 0)
		{
			// set the engine power and angle if selected
			if (mSelectedEntities[0].GetType() == ET_ENGINE)
			{
				Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
				for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
				{
					Vector2 pos;
					i->PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
					Vector2 dir = cursor - pos;
					float32 angle = MathUtils::Angle(dir);
					i->PostMessage(EntityMessage::TYPE_SET_ABSOLUTE_TARGET_ANGLE, &angle);
					// get back the clamped value
					i->PostMessage(EntityMessage::TYPE_GET_ABSOLUTE_TARGET_ANGLE, &angle);
					Vector2 unitDir = MathUtils::VectorFromAngle(angle);
					// project the target vector onto the correct direction vector
					dir = MathUtils::Dot(dir, unitDir) * unitDir;

					int32 screenDist = gGfxRenderer.WorldToScreenScalar(dir.Length());
					float32 powerRatio = (float32)screenDist / (float32)GetEnginePowerCircleRadius();
					i->PostMessage(EntityMessage::TYPE_SET_TARGET_POWER_RATIO, &powerRatio);
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
