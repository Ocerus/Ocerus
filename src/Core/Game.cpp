#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"
#include "PhysicsDraw.h"
#include "Editor/EditorMgr.h"
#include <Box2D.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include "GfxSystem/Mesh.h"

using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

const float PHYSICS_TIMESTEP = 0.016f;
const int32 PHYSICS_VELOCITY_ITERATIONS = 6;
const int32 PHYSICS_POSITION_ITERATIONS = 2;
const float32 SELECTION_MIN_DISTANCE = 10.0f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!


/// Callback receiver from physics.
class Core::Game::PhysicsCallbacks: public b2ContactFilter, public b2ContactListener
{
public:

	/// Default constructor.
	PhysicsCallbacks(Core::Game* parent): mParent(parent) {}

	/// Default destructor.
	virtual ~PhysicsCallbacks(void) {}

	virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);

	virtual void BeginContact(b2Contact* contact);

private:
	Core::Game* mParent;
};


Core::Game::Game():
	StateMachine<eGameState>(GS_INITING),
	mTimer(true),
	mRenderTarget(GfxSystem::InvalidRenderTargetID),
	mCamera(EntitySystem::EntityHandle::Null),
	mPhysics(0),
	mPhysicsCallbacks(0)
{
	mPhysicsCallbacks = new PhysicsCallbacks(this);
	mPhysicsDraw = new PhysicsDraw();

	GlobalProperties::SetPointer("Game", this);
}

Core::Game::~Game()
{
	Clean();
	delete mPhysicsCallbacks;
	delete mPhysicsDraw;

	GlobalProperties::SetPointer("Game", 0);
}

void Core::Game::SetRenderTarget( const GfxSystem::RenderTargetID renderTarget )
{
	OC_ASSERT(mRenderTarget == GfxSystem::InvalidRenderTargetID);
	mRenderTarget = renderTarget;
	mCamera = gGfxRenderer.GetRenderTargetCamera(mRenderTarget);
}

void Core::Game::UpdateGameProperties( void )
{
	PROFILE_FNC();

	// update current game properties
	GlobalProperties::SetPointer("Game", this);
	GlobalProperties::SetPointer("Physics", mPhysics);
	GlobalProperties::SetPointer("GameCamera", &mCamera);
}

void Core::Game::Init()
{
	ocInfo << "Game init";
	ForceStateChange(GS_INITING);

	// security check
	if (!gGfxRenderer.IsRenderTargetValid(mRenderTarget))
	{
		ocError << "Invalid render target for game";
		return;
	}

	// basic init stuff
	mSelectionStarted = false;
	mHoveredEntity.Invalidate();
	mActionState = AS_RUNNING;
	mTimer.Reset();

	// init physics engine
	mPhysics = new b2World(b2Vec2(0.0f, 0.0f), false);
	mPhysics->SetContactFilter(mPhysicsCallbacks);
	mPhysics->SetContactListener(mPhysicsCallbacks);
	mPhysics->SetDebugDraw(mPhysicsDraw);
	mPhysicsResidualDelta = 0.0f;


	// update globally accessible game related properties, like the physics engine
	UpdateGameProperties();


	// DEBUG
	gResourceMgr.AddResourceDirToGroup(ResourceSystem::BPT_SYSTEM, "test", "Test", ".+\\.(xml|model)");
	gResourceMgr.LoadResourcesInGroup("Test");
	gEntityMgr.LoadEntitiesFromResource(gResourceMgr.GetResource("Test", "test_entities.xml"));

	gInputMgr.AddInputListener(this);
	gApp.ResetStats();

	ForceStateChange(GS_NORMAL);
	ocInfo << "Game inited";
}

void Core::Game::Clean()
{	
	ForceStateChange(GS_CLEANING);

	if (mPhysics) delete mPhysics;
	for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
	{
		delete *i;
	}
	mPhysicsEvents.clear();
	gInputMgr.RemoveInputListener(this);
}

void Core::Game::Update( const float32 delta )
{
	PROFILE_FNC();

	if (GetState() != GS_NORMAL) return;

	UpdateGameProperties();

	if (!IsActionRunning()) return;

	mTimer.UpdateInSeconds(delta);

	// pick entity the mouse is hovering over right now
	MouseState& mouse = gInputMgr.GetMouseState();
	Vector2 worldPosition;
	if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldPosition, mRenderTarget))
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

		// run scripts and game logic
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::UPDATE_LOGIC, Reflection::PropertyFunctionParameters() << stepSize));

		// synchronize properties before physics
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::SYNC_PRE_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));

		// physical step forward
		mPhysics->Step(stepSize, PHYSICS_VELOCITY_ITERATIONS, PHYSICS_POSITION_ITERATIONS);

		// synchronize properties after physics
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::SYNC_POST_PHYSICS, Reflection::PropertyFunctionParameters() << stepSize));		

		// process physics events
		for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		{
			ProcessPhysicsEvent(**i);
			delete *i;
		}
		mPhysicsEvents.clear();

		// destroy entities marked for destruction
		gEntityMgr.ProcessDestroyQueue();

		// update game logic after the physics was processed
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

	if (GetState() != GS_NORMAL) return;

	float32 delta = passedDelta;
	if (!IsActionRunning()) delta = 0.0f;

	
	gGfxRenderer.SetCurrentRenderTarget(mRenderTarget);
	gGfxRenderer.ClearCurrentRenderTarget(GfxSystem::Color(0, 0, 0));
	
	gGfxRenderer.DrawEntities();

	GfxSystem::TexturedMesh mesh;
	mesh.mesh = ((GfxSystem::MeshPtr)gResourceMgr.GetResource("Test", "cube.model"))->GetMesh();
	mesh.scale = 1.5f;
	mesh.z = 0.0f;
	static float32 zAngle = 0.0f;
	mesh.zAngle = zAngle;
	zAngle += 0.8f * delta;
	mesh.angle = 0.8f;
	gGfxRenderer.DrawTexturedMesh(mesh);

	mPhysics->DrawDebugData();

	// draw the multi-selection stuff
	if (mSelectionStarted)
	{
		MouseState& mouse = gInputMgr.GetMouseState();
		Vector2 worldCursorPos;
		if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldCursorPos, mRenderTarget))
		{
			float32 minDistance = SELECTION_MIN_DISTANCE / gGfxRenderer.GetRenderTargetCameraZoom(mRenderTarget);
			if ((worldCursorPos-mSelectionCursorPosition).LengthSquared() >= MathUtils::Sqr(minDistance))
			{
				float32 rotation = gGfxRenderer.GetRenderTargetCameraRotation(mRenderTarget);
				gGfxRenderer.DrawRect(mSelectionCursorPosition, worldCursorPos, rotation, GfxSystem::Color(255,255,0,150), false);
			}
		}
	}
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
	if (btn == MBTN_LEFT)
	{
		mSelectionStarted = gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), mSelectionCursorPosition, mRenderTarget);
		return true;
	}

	return false;
}

bool Core::Game::MouseButtonReleased( const MouseInfo& mi, const eMouseButton btn )
{
	OC_UNUSED(btn);
	if (!mSelectionStarted)
	{
		// the mouse was not previously pressed probably
		return false;
	}
	mSelectionStarted = false;

	Vector2 worldCursorPos;
	if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mRenderTarget))
	{
		// we're not in the corrent viewport
		return false;
	}

	bool multiSelection = false;
	float32 minDistance = SELECTION_MIN_DISTANCE / gGfxRenderer.GetRenderTargetCameraZoom(mRenderTarget);
	if ((worldCursorPos-mSelectionCursorPosition).LengthSquared() >= MathUtils::Sqr(minDistance))
	{
		// the cursor moved far away enough
		multiSelection = true;
	}

	if (multiSelection)
	{
		// selection of multiple entities in a rectangle
		mSelectedEntities.clear();
		EntityPicker picker(mSelectionCursorPosition);
		float32 cameraRotation = gGfxRenderer.GetRenderTargetCameraRotation(mRenderTarget);
		if (picker.PickMultipleEntities(worldCursorPos, cameraRotation, mSelectedEntities) == 1)
		{
			Editor::EditorMgr::GetSingleton().SetCurrentEntity(mSelectedEntities[0]);
		}
		else
		{
			Editor::EditorMgr::GetSingleton().SetCurrentEntity(EntityHandle::Null);
		}		
	}
	else if (mHoveredEntity.IsValid())
	{
		// selection of a single entity under the cursor
		mSelectedEntities.clear();
		mSelectedEntities.push_back(mHoveredEntity);
		Editor::EditorMgr::GetSingleton().SetCurrentEntity(mHoveredEntity);
	}
	else
	{
		// nothing is selected
		mSelectedEntities.clear();
		Editor::EditorMgr::GetSingleton().SetCurrentEntity(EntityHandle::Null);
	}

	return true;
}

bool Core::Game::PhysicsCallbacks::ShouldCollide( b2Fixture* fixtureA, b2Fixture* fixtureB )
{
	if (!b2ContactFilter::ShouldCollide(fixtureA, fixtureB))
		return false;

	return true;
}

void Core::Game::PhysicsCallbacks::BeginContact( b2Contact* contact )
{
	PhysicsEvent* evt = new PhysicsEvent();
	void* userData1 = contact->GetFixtureA()->GetUserData();
	void* userData2 = contact->GetFixtureB()->GetUserData();
	if (userData1) evt->entity1 = *(EntityHandle*)userData1;
	else evt->entity1.Invalidate();
	if (userData2) evt->entity2 = *(EntityHandle*)userData2;
	else evt->entity2.Invalidate();
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
