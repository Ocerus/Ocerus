#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"
#include "Box2D.h"
#include "PhysicsDraw.h"

//temporary here
#include "../GfxSystem/GfxSceneMgr.h"


// DEBUG only
#include "../EntitySystem/Components/Script.h"


using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

const float PHYSICS_TIMESTEP = 0.016f;
const int32 PHYSICS_ITERATIONS = 10;


Core::Game::Game():
	StateMachine<eGameState>(GS_NORMAL),
	mTimer(true),
	mPhysics(0),
	mPhysicsCallbacks(0)
{
	mPhysicsCallbacks = new PhysicsCallbacks(this);
	mPhysicsDraw = new PhysicsDraw();
}

Core::Game::~Game()
{
	Deinit();
	delete mPhysicsCallbacks;
	delete mPhysicsDraw;
}


void Core::Game::UpdateGameProperties( void )
{
	PROFILE_FNC();

	// update current game properties
	GlobalProperties::SetPointer("Game", this);
	GlobalProperties::SetPointer("Physics", mPhysics);
}

void Core::Game::Init()
{
	ocInfo << "Game init";

	// basic init stuff
	mHoveredEntity.Invalidate();
	mActionState = AS_RUNNING;
	mTimer.Reset();

	// init physics engine
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-10000.0f, -10000.0f);
	worldAABB.upperBound.Set(10000.0f, 10000.0f);
	// turn off sleeping as we are moving in a space with no gravity
	mPhysics = new b2World(worldAABB, b2Vec2(0.0f, 0.0f), false);
	mPhysics->SetContactFilter(mPhysicsCallbacks);
	mPhysics->SetContactListener(mPhysicsCallbacks);
	mPhysics->SetDebugDraw(mPhysicsDraw);
	mPhysicsResidualDelta = 0.0f;


	// update globally accessible game related properties, like the physics engine
	UpdateGameProperties();


	//// TEST ////

	//-------------------
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("TestEntities", "test_entities.xml"));
	//-------------------

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
	gInputMgr.RemoveInputListener(this);
}

void Core::Game::Update( const float32 delta )
{
	PROFILE_FNC();

	UpdateGameProperties();
	
	if (!IsActionRunning()) return;
	
	mTimer.UpdateInSeconds(delta);


	// pick hover entity
	MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::MOUSE_PICK));
	mHoveredEntity = picker.GetResult();

	// check action scripts
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::CHECK_ACTION));

	// advance the physics forward in time
	float32 physicsDelta = delta + mPhysicsResidualDelta;
	while (physicsDelta > PHYSICS_TIMESTEP)
	{
		float32 stepSize = PHYSICS_TIMESTEP;
		
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_LOGIC, Reflection::PropertyFunctionParameters() << stepSize));
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));
		
		mPhysics->Step(stepSize, PHYSICS_ITERATIONS);

		mPhysicsEvents.push_back(new PhysicsEvent());
		mPhysicsEvents.push_back(new PhysicsEvent());

		// process physics events
		for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		{
			ProcessPhysicsEvent(**i);
			delete *i;
		}
		mPhysicsEvents.clear();

		// destroy entities marked for destruction
		gEntityMgr.ProcessDestroyQueue();

		// synchronize the entities with their new state after the physics was updated
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_POST_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));

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

void Core::Game::Draw( const float32 passedDelta)
{
	PROFILE_FNC();

	float32 delta = passedDelta;
	if (!IsActionRunning()) delta = 0.0f;


	// ----------------TESTING-------------------------
	EntitySystem::EntityHandle cam_handle;

	// Viewport 1 -------------------------------------
	cam_handle = gGfxSceneMgr.GetCamera(0);
	if (cam_handle.IsValid())
	{
		PropertyHolder pos_holder = gEntityMgr.GetEntityProperty(cam_handle, "Position" );
		PropertyHolder zoom_holder = gEntityMgr.GetEntityProperty(cam_handle, "Zoom" );
		PropertyHolder rot_holder = gEntityMgr.GetEntityProperty(cam_handle, "Rotation" );
		gGfxRenderer.SetCamera(pos_holder.GetValue<Vector2>(), zoom_holder.GetValue<float32>(), rot_holder.GetValue<float32>());

		GfxSystem::GfxViewport vp(Vector2(0, 0.5), Vector2(0.5, 0.5), true);
		gGfxRenderer.SetViewport(vp);

		gGfxRenderer.DrawSprites();

		gGfxRenderer.DrawRect(Vector2(100,-100), Vector2(200,300), 45, GfxSystem::Color(255,0,0), false);

		// Testing physics draw
		mPhysics->DrawDebugData();

		gGfxRenderer.FinalizeViewport();
	}

	// Viewport 2 -------------------------------------
	cam_handle = gGfxSceneMgr.GetCamera(1);
	if (cam_handle.IsValid())
	{
		PropertyHolder pos_holder = gEntityMgr.GetEntityProperty(cam_handle, "Position" );
		PropertyHolder zoom_holder = gEntityMgr.GetEntityProperty(cam_handle, "Zoom" );
		PropertyHolder rot_holder = gEntityMgr.GetEntityProperty(cam_handle, "Rotation" );
		gGfxRenderer.SetCamera(pos_holder.GetValue<Vector2>(), zoom_holder.GetValue<float32>(), rot_holder.GetValue<float32>());

		GfxSystem::GfxViewport vp(Vector2(0.0, 0.0), Vector2(1, 0.5), false);
		gGfxRenderer.SetViewport(vp);

		gGfxRenderer.DrawSprites();
		
		// Testing physics draw
		mPhysics->DrawDebugData();

		gGfxRenderer.FinalizeViewport();
	}

	// Viewport 3 -------------------------------------
	cam_handle = gGfxSceneMgr.GetCamera(2);
	if (cam_handle.IsValid())
	{
		PropertyHolder pos_holder = gEntityMgr.GetEntityProperty(cam_handle, "Position" );
		PropertyHolder zoom_holder = gEntityMgr.GetEntityProperty(cam_handle, "Zoom" );
		PropertyHolder rot_holder = gEntityMgr.GetEntityProperty(cam_handle, "Rotation" );
		gGfxRenderer.SetCamera(pos_holder.GetValue<Vector2>(), zoom_holder.GetValue<float32>(), rot_holder.GetValue<float32>());

		GfxSystem::GfxViewport vp(Vector2(0.5, 0.5), Vector2(0.5, 0.5), true);
		gGfxRenderer.SetViewport(vp);

		gGfxRenderer.DrawSprites();
		
		// Testing physics draw
		mPhysics->DrawDebugData();

		gGfxRenderer.FinalizeViewport();
	}

	//GfxSystem::GfxViewport vp(Vector2(0, 0), Vector2(1, 1), true);
	//gGfxRenderer.SetViewport(vp);
	
	//TODO: odstranit - pouze pro testovaci ucely
	//GfxSystem::TexturePtr t = gResourceMgr.GetResource("textures", "Logo.png");
	//gGfxRenderer.DrawTestTexturedQuad(t->GetTexture());
	//------------------------------------------

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

	if (ke.keyAction == KC_F9) {
		if (IsActionRunning())
		{
			PauseAction();
			ocInfo << "Game action paused";
		}
		else 
		{
			ResumeAction();
			ocInfo << "Game action resumed";
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
					if (find(mSelectedEntities.begin(), mSelectedEntities.end(), mHoveredEntity) == mSelectedEntities.end())
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

bool Core::Game::PhysicsCallbacks::ShouldCollide( b2Shape* shape1, b2Shape* shape2 )
{
	if (!b2ContactFilter::ShouldCollide(shape1, shape2))
		return false;

	return true;
}

void Core::Game::PhysicsCallbacks::Add( const b2ContactPoint* point )
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
	mParent->mPhysicsEvents.push_back(evt);
}

void Core::Game::ProcessPhysicsEvent( const PhysicsEvent& evt )
{
	if (!evt.entity1.IsValid() || !evt.entity2.IsValid())
		return;
}

void Core::Game::PauseAction( void )
{
	mActionState = AS_PAUSED;
}

void Core::Game::ResumeAction( void )
{
	mActionState = AS_RUNNING;
}

void Core::Game::RestartAction( void )
{
	// first somebody has to implement scene saving
	OC_ASSERT_MSG(false, "not implemented");
}