#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"
#include "Box2D.h"
#include "PhysicsDraw.h"
#include "Editor/EditorMgr.h"

// DEBUG only
#include "EntitySystem/Components/Script.h"
#include "EntitySystem/Components/Transform.h"
GfxSystem::TextureHandle gRenderTexture;
GfxSystem::RenderTargetID gRenderTarget;

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

	// setup render targets
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0, 0.5), Vector2(0.5, 0.5), true), gEntityMgr.FindFirstEntity("Camera1"));
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0.0, 0.0), Vector2(1, 0.5), false), gEntityMgr.FindFirstEntity("Camera2"));
	gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0.5, 0.5), Vector2(0.5, 0.5), true), gEntityMgr.FindFirstEntity("Camera3"));

	gRenderTexture = gGfxRenderer.CreateRenderTexture(128, 128);
	gRenderTarget = gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(gRenderTexture, 128, 128), gEntityMgr.FindFirstEntity("Camera2"));
	


	gInputMgr.AddInputListener(this);
	gApp.ResetStats();
	ocInfo << "Game inited";

	Script::TestRunTime();
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
	MouseState& mouse = gInputMgr.GetMouseState();
	Vector2 worldPosition;
	if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldPosition))
	{
		EntityPicker picker(worldPosition);
		mHoveredEntity = picker.PickSingleEntity();
	}

	// check action scripts
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::CHECK_ACTION));

	// advance the physics forward in time
	float32 physicsDelta = delta + mPhysicsResidualDelta;
	while (physicsDelta > PHYSICS_TIMESTEP)
	{
		float32 stepSize = PHYSICS_TIMESTEP;

		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_LOGIC, Reflection::PropertyFunctionParameters() << stepSize));
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_PRE_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));

		mPhysics->Step(stepSize, PHYSICS_ITERATIONS);

		///@todo what is the purpose of this?!?
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

		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));		

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
	gGfxRenderer.ClearScreen(GfxSystem::Color(0, 0, 0));

	gGfxRenderer.SetCurrentRenderTarget(gRenderTarget);
	gGfxRenderer.ClearScreen(GfxSystem::Color(0, 255, 0));
	EntityComponents::Transform transform;
	transform.Create();
	transform.SetScale(Vector2(4.0f, 4.0f));
	gGfxRenderer.DrawSprite(gEntityMgr.GetEntityComponent(gEntityMgr.FindFirstEntity("Visual"), EntitySystem::CT_Sprite), &transform);
	//gGfxRenderer.DrawEntity(gEntityMgr.FindFirstEntity("Visual"));
	gGfxRenderer.FinalizeRenderTarget();
	


	gGfxRenderer.SetCurrentRenderTarget(0);
	gGfxRenderer.DrawEntities();
	mPhysics->DrawDebugData();
	gGfxRenderer.FinalizeRenderTarget();


	gGfxRenderer.SetCurrentRenderTarget(1);
	gGfxRenderer.DrawEntities();
	mPhysics->DrawDebugData();
	GfxSystem::TexturedQuad quad;
	quad.position.y = 150.0f;
	quad.size.x = 128;
	quad.size.y = 128;
	quad.transparency = 0.5f;
	quad.texture = gRenderTexture;
	gGfxRenderer.DrawTexturedQuad(quad);
	gGfxRenderer.FinalizeRenderTarget();


	gGfxRenderer.SetCurrentRenderTarget(2);
	gGfxRenderer.DrawEntities();
	mPhysics->DrawDebugData();
	gGfxRenderer.FinalizeRenderTarget();

}

bool Core::Game::KeyPressed( const KeyInfo& ke )
{
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

		return true;
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

		return true;
	}

	return false;
}

bool Core::Game::KeyReleased( const KeyInfo& ke )
{
	OC_UNUSED(ke);
	return false;
}

bool Core::Game::MouseMoved( const MouseInfo& mi )
{
	OC_UNUSED(mi);
	return false;
}

bool Core::Game::MouseButtonPressed( const MouseInfo& mi, const eMouseButton btn )
{
	OC_UNUSED(mi);

	if (btn == MBTN_LEFT)
	{
		if (mHoveredEntity.IsValid())
		{
			mSelectedEntities.clear();
			mSelectedEntities.push_back(mHoveredEntity);
			Editor::EditorMgr::GetSingleton().SetCurrentEntity(mHoveredEntity);
		}
		else
		{
			mSelectedEntities.clear();
			Editor::EditorMgr::GetSingleton().SetCurrentEntity(EntityHandle::Null);
		}

		return true;
	}

	return false;
}

bool Core::Game::MouseButtonReleased( const MouseInfo& mi, const eMouseButton btn )
{
	OC_UNUSED(mi);
	OC_UNUSED(btn);
	return false;
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