#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"
#include "Box2D.h"


// DEBUG only
#include "../EntitySystem/Components/OnInitScript.h"
#include "../EntitySystem/Components/Script.h"


using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

#define PHYSICS_TIMESTEP 0.016f
#define PHYSICS_ITERATIONS 10
#define ENGINE_VISIBLE_MAX_POWER 80
#define WEAPON_VISIBLE_ARC_RADIUS 80
#define CAMERA_SPEED_RATIO 700.0f
#define CAMERA_SCALE_RATIO 0.001f
#define WEAPON_ANGLECHANGE_SPEED 2.0f
#define ENGINE_ANGLECHANGE_SPEED 2.0f
#define ENGINE_POWERCHANGE_SPEED 1.0f

#define WATER_TEXTURE_SCALE 0.01f

Core::Game::Game():
	StateMachine<eGameState>(GS_NORMAL),
	mPhysics(0),
	mLastClickTime(0) {}

Core::Game::~Game()
{
	Deinit();
}


void Core::Game::UpdateGameProperties( void )
{
	// update current game properties
	GlobalProperties::SetPointer("CurrentGame", this);
	GlobalProperties::SetPointer("CurrentPhysics", mPhysics);
}

void Core::Game::Init()
{
	ocInfo << "Game init";

	// basic init stuff
	mHoveredEntity.Invalidate();

	// init physics engine
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-10000.0f, -10000.0f);
	worldAABB.upperBound.Set(10000.0f, 10000.0f);
	// turn off sleeping as we are moving in a space with no gravity
	mPhysics = new b2World(worldAABB, b2Vec2(0.0f, 0.0f), false);
	mPhysics->SetContactFilter(this);
	mPhysics->SetContactListener(this);
	mPhysicsResidualDelta = 0.0f;


	// update globally accessible game related properties, like the physics engine
	UpdateGameProperties();


	//// TEST ////

	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Prototypes/ship0.xml"), true);

	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Ships/ship0.xml"));


	// set camera
	mCameraFocus.Invalidate();
	EntityHandle ship = gEntityMgr.FindFirstEntity("ship0");
	if (ship.IsValid())
	{
		Vector2 shipPos = ship.GetProperty("Position").GetValue<Vector2>();
		//TODO:Gfx
		//gGfxRenderer.SetCameraPos(shipPos);
		mCameraFocus = ship;
	}
	//TODO:Gfx
	//gGfxRenderer.SetCameraScale(50.0f);


	gInputMgr.AddInputListener(this);


	gApp.ResetStats();

	ocInfo << "Game inited";


	// an example of a script
	EntityComponents::OnInitScript::TestRunTime();
	EntityComponents::Script::TestRunTime();


	// an example of how to use functions registered in the reflection system
	if (ship.IsValid())
	{
		PropertyHolder prop = ship.GetFunction("MyFunction");
		PropertyFunctionParameters params;
		string str("Calling my function");
		params.PushParameter(&str);
		prop.CallFunction(params);
	}
}

void Core::Game::Deinit()
{
	if (mPhysics)
		delete mPhysics;
	for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		delete *i;
	mPhysicsEvents.clear();
}

void Core::Game::Update( const float32 delta )
{
	PROFILE_FNC();

	UpdateGameProperties();

	// pick hover entity
	MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::MOUSE_PICK, &picker));
	mHoveredEntity = picker.GetResult();

	// react on input
	// ... TODO

	// advance the physics forward in time
	float32 physicsDelta = delta + mPhysicsResidualDelta;
	while (physicsDelta > PHYSICS_TIMESTEP)
	{
		float32 stepSize = PHYSICS_TIMESTEP;
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_LOGIC, &stepSize));
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_PHYSICS, &stepSize));
		mPhysics->Step(stepSize, PHYSICS_ITERATIONS);

		// process physics events
		for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		{
			ProcessPhysicsEvent(**i);
			delete *i;
		}
		mPhysicsEvents.clear();

		// destroy entities marked for destruction
		gEntityMgr.ProcessDestroyQueue();

		// if some of the selected entities were destroyed, remove them from the selection
		if (!mHoveredEntity.Exists())
			mHoveredEntity.Invalidate();
		for (EntityList::iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end();)
		{
			if (!it->Exists())
				it = mSelectedEntities.erase(it);
			else
				++it;
		}
		if (!mCameraFocus.Exists())
			mCameraFocus.Invalidate();

		physicsDelta -= stepSize;
	}
	mPhysicsResidualDelta = physicsDelta;

	//particle effects
	//TODO:Gfx
	//gPSMgr.Update(delta);
};

void Core::Game::Draw( const float32 delta)
{
	PROFILE_FNC();

	//TODO:Gfx
	// move camera in reaction to the user input
	/*if (gInputMgr.IsKeyDown(KC_LEFT))
	{
		gGfxRenderer.MoveCamera(-CAMERA_SPEED_RATIO / gGfxRenderer.GetCameraScale() * delta, 0.0f);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_RIGHT))
	{
		gGfxRenderer.MoveCamera(CAMERA_SPEED_RATIO / gGfxRenderer.GetCameraScale() * delta, 0.0f);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_UP))
	{
		gGfxRenderer.MoveCamera(0.0f, -CAMERA_SPEED_RATIO / gGfxRenderer.GetCameraScale() * delta);
		mCameraFocus.Invalidate();
	}
	if (gInputMgr.IsKeyDown(KC_DOWN))
	{
		gGfxRenderer.MoveCamera(0.0f, CAMERA_SPEED_RATIO / gGfxRenderer.GetCameraScale() * delta);
		mCameraFocus.Invalidate();
	}

	if (mCameraFocus.IsValid())
	{
		Vector2 pos = mCameraFocus.GetProperty("Position").GetValue<Vector2>();
		gGfxRenderer.SetCameraPos(pos);
	}

	// clear the screen
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));


	// draw underlay under selected/hovered entities
	bool hoveredEntityUnderlayDrawn = false;
	for (EntityList::iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end(); ++it)
	{
		bool hovered = false;
		gEntityMgr.PostMessage(*it, EntityMessage(EntityMessage::DRAW_UNDERLAY, &hovered));
		if ((*it) == mHoveredEntity) hoveredEntityUnderlayDrawn = true;
	}
	if (!hoveredEntityUnderlayDrawn && mHoveredEntity.IsValid())
	{
		bool hovered = true;
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::DRAW_UNDERLAY, &hovered));
	}

	// draw entitites
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::DRAW));

	// draw overlay above selected/hovered entities
	bool hoveredEntityOverlayDrawn = false;
	for (EntityList::iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end(); ++it)
	{
		bool hovered = false;
		gEntityMgr.PostMessage(*it, EntityMessage(EntityMessage::DRAW_OVERLAY, &hovered));
		if ((*it) == mHoveredEntity) hoveredEntityOverlayDrawn = true;

	}
	if (!hoveredEntityOverlayDrawn && mHoveredEntity.IsValid())
	{
		bool hovered = true;
		gEntityMgr.PostMessage(mHoveredEntity, EntityMessage(EntityMessage::DRAW_OVERLAY, &hovered));
	}


	// draw remaining (undrawn) particle effects
	gPSMgr.Render();*/
}

#include "../ResourceSystem/XMLResource.h"

void Core::Game::KeyPressed( const KeyInfo& ke )
{
	if (ke.keyAction == KC_ESCAPE)
	{
		gApp.Shutdown();
	}

	if (ke.keyAction == KC_F5 && gInputMgr.IsKeyDown(KC_LCONTROL))
	{
		if (gProfiler.IsRunning())
		{
			gProfiler.Stop();
			gProfiler.DumpIntoConsole();
		}
		else
		{
			gProfiler.Start();
		}
	}
}

void Core::Game::KeyReleased( const KeyInfo& ke )
{

}

void Core::Game::MouseMoved( const MouseInfo& mi )
{
	//TODO:Gfx
	/*if (gInputMgr.IsMouseButtonPressed(MBTN_MIDDLE))
	{
		mCameraFocus.Invalidate();
		Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
		gGfxRenderer.SetCameraPos(gGfxRenderer.GetCameraPos() - cursor + mCameraGrabWorldPos);
	}

	// zoom camera
	if (mi.wheelDelta)
		gGfxRenderer.ZoomCamera(CAMERA_SCALE_RATIO * gGfxRenderer.GetCameraScale() * mi.wheelDelta);*/
}

void Core::Game::MouseButtonPressed( const MouseInfo& mi, const eMouseButton btn )
{
	if (btn == MBTN_LEFT)
	{
		bool doubleClick = false;
		uint64 curTime = gApp.GetCurrentTimeMillis();
		if (curTime - mLastClickTime < 200)
			doubleClick = true;
		mLastClickTime = curTime;

		if (mHoveredEntity.IsValid())
		{
			// use the hover entity as a focus
			if (gInputMgr.IsKeyDown(KC_RCONTROL) || gInputMgr.IsKeyDown(KC_LCONTROL))
			{
				mCameraFocus = mHoveredEntity;
			}
			// select controllable entities
			else
			{
				// add to the current selection if SHIFT down
				if (mSelectedEntities.size()>0 && (gInputMgr.IsKeyDown(KC_RSHIFT) || gInputMgr.IsKeyDown(KC_LSHIFT)))
				{
					if (mHoveredEntity.IsValid() && mHoveredEntity.GetType() == mSelectedEntities[0].GetType()
						&& find(mSelectedEntities.begin(), mSelectedEntities.end(), mHoveredEntity) == mSelectedEntities.end())
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
		}
		else
		{
			mSelectedEntities.clear();
		}
	}
	else if (btn == MBTN_RIGHT)
	{
		//TODO:Gfx
		//Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
		for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		{
			// eEntityType type = i->GetType();
		}
	}
	else if (btn == MBTN_MIDDLE)
	{
		//TODO:Gfx
		//mCameraGrabWorldPos = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
	}
}

void Core::Game::MouseButtonReleased( const MouseInfo& mi, const eMouseButton btn )
{

}

int32 Core::Game::GetEnginePowerCircleRadius( void ) const
{
	return ENGINE_VISIBLE_MAX_POWER;
}

int32 Core::Game::GetWeaponCircleRadius( void ) const
{
	return WEAPON_VISIBLE_ARC_RADIUS;
}

bool Core::Game::ShouldCollide( b2Shape* shape1, b2Shape* shape2 )
{
	if (!b2ContactFilter::ShouldCollide(shape1, shape2))
		return false;

	return true;
}

void Core::Game::Add( const b2ContactPoint* point )
{
	PhysicsEvent* evt = new PhysicsEvent();
	if (point->shape1->GetUserData())
		evt->entity1 = *(EntityHandle*)point->shape1->GetUserData();
	else
		evt->entity1.Invalidate();
	if (point->shape2->GetUserData())
		evt->entity2 = *(EntityHandle*)point->shape2->GetUserData();
	else
		evt->entity2.Invalidate();
	mPhysicsEvents.push_back(evt);
}

void Core::Game::ProcessPhysicsEvent( const PhysicsEvent& evt )
{
	if (!evt.entity1.IsValid() || !evt.entity2.IsValid())
		return;
	EntityHandle ent1 = evt.entity1;
	EntityHandle ent2 = evt.entity2;
	eEntityType type1 = gEntityMgr.GetEntityType(ent1);
	eEntityType type2 = gEntityMgr.GetEntityType(ent2);
	// Suppress unused warning
	(void)type1;
	(void)type2;
}
