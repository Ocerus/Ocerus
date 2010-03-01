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

	// load entities
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("TestEntities", "test_entities.xml"));

	// set cameras
	mCameraFocus.Invalidate();
	EntityHandle ship = gEntityMgr.FindFirstEntity("ship0");
	if (ship.IsValid())
	{
		Vector2 shipPos = ship.GetProperty("Position").GetValue<Vector2>();
		mCameraFocus = ship;
	}

	// setup render targets
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0, 0.5), Vector2(0.5, 0.5), true), gGfxSceneMgr.GetCamera(0));
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0.0, 0.0), Vector2(1, 0.5), false), gGfxSceneMgr.GetCamera(1));
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0.5, 0.5), Vector2(0.5, 0.5), true), gGfxSceneMgr.GetCamera(2));



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


	// pick entity the mouse is hovering over right now
	/*MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	mHoveredEntity = picker.PickSingleEntity();*/

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
}

void Core::Game::Draw( const float32 passedDelta)
{
	PROFILE_FNC();

	float32 delta = passedDelta;
	if (!IsActionRunning()) delta = 0.0f;


	// ----------------TESTING-------------------------
	gGfxRenderer.SetCurrentRenderTarget(0);
	gGfxRenderer.DrawSprites();
	gGfxRenderer.DrawRect(Vector2(100,-100), Vector2(200,300), 45, GfxSystem::Color(255,0,0), false);
	// Testing physics draw
	mPhysics->DrawDebugData();
	gGfxRenderer.FinalizeRenderTarget();


	gGfxRenderer.SetCurrentRenderTarget(1);
	gGfxRenderer.DrawSprites();
	// Testing physics draw
	mPhysics->DrawDebugData();
	gGfxRenderer.FinalizeRenderTarget();


	gGfxRenderer.SetCurrentRenderTarget(2);
	gGfxRenderer.DrawSprites();
	// Testing physics draw
	mPhysics->DrawDebugData();
	gGfxRenderer.FinalizeRenderTarget();

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