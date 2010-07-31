#include "Common.h"
#include "Game.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"
#include "Core/Application.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "ResourceSystem/XMLResource.h"
#include "GUISystem/CEGUITools.h"
#include "GfxSystem/PhysicsDraw.h"
#include "GfxSystem/Mesh.h"

#include <Box2D.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>

using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

const float PHYSICS_TIMESTEP = 0.016f;
const int32 PHYSICS_VELOCITY_ITERATIONS = 6;
const int32 PHYSICS_POSITION_ITERATIONS = 2;
const char* Game::ActionFile = "ActionSave.xml";
const string Game::GameCameraName = "GameCamera";


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

	virtual void EndContact(b2Contact* contact);

private:
	Core::Game* mParent;
};


Core::Game::Game():
	StateMachine<eGameState>(GS_NOT_INITED),
	mActionRestarted(true),
	mTimer(true),
	mRenderTarget(GfxSystem::InvalidRenderTargetID),
	mCamera(EntitySystem::EntityHandle::Null),
	mRootWindow(0),
	mPhysics(0),
	mPhysicsCallbacks(0)
{
	mPhysicsCallbacks = new PhysicsCallbacks(this);
	mPhysicsDraw = new GfxSystem::PhysicsDraw();
	mPhysics = new b2World(b2Vec2(0.0f, 0.0f), true);

	UpdateGameProperties();
}

Core::Game::~Game()
{
	Clean();

	OC_ASSERT_MSG(mPhysics->GetBodyCount() == 0, "There are physical bodies left");
	OC_ASSERT_MSG(mPhysics->GetJointCount() == 0, "There are physical joints left");

	delete mPhysics;
	delete mPhysicsCallbacks;
	delete mPhysicsDraw;
	if (mRootWindow && mRootWindow->getName() == "GameRoot") gGUIMgr.DestroyWindow(mRootWindow);

	GlobalProperties::SetPointer("Game", 0);
}

void Core::Game::CreateDefaultRenderTarget()
{
	ocInfo << "Creating default render target for game";

	EntitySystem::EntityHandle mCamera = gEntityMgr.FindFirstEntity(GameCameraName);
	if (!mCamera.Exists())
	{
		// Create the game camera.
		EntitySystem::EntityDescription desc;
		desc.SetName(GameCameraName);
		desc.AddComponent(EntitySystem::CT_Camera);
		mCamera = gEntityMgr.CreateEntity(desc);
		mCamera.FinishInit();
	}

	if (mRenderTarget != GfxSystem::InvalidRenderTargetID)
	{
	  gGfxRenderer.RemoveRenderTarget(mRenderTarget);
	}
	mRenderTarget = gGfxRenderer.AddRenderTarget(GfxSystem::GfxViewport(Vector2(0, 0), Vector2(1.0f, 1.0f), false, true), mCamera);
	gGfxRenderer.GetRenderTargetViewport(mRenderTarget)->SetGridEnabled(false);
}

void Core::Game::CreateDefaultRootWindow()
{
	if (mRootWindow && mRootWindow->getName() == "GameRoot") return;
	mRootWindow = gGUIMgr.CreateWindow("DefaultWindow");
	mRootWindow->setProperty("UnifiedAreaRect", "{{0,0},{0,0},{1,0},{1,0}}");
	gGUIMgr.SetGUISheet(mRootWindow);
}

void Core::Game::SetRootWindow(CEGUI::Window* window)
{ 
	if (mRootWindow && mRootWindow->getName() == "GameRoot") gGUIMgr.DestroyWindow(mRootWindow);
	mRootWindow = window;
}

void Core::Game::SetRenderTarget(const GfxSystem::RenderTargetID renderTarget)
{
	mRenderTarget = renderTarget;
	mCamera = gGfxRenderer.GetRenderTargetCamera(mRenderTarget);
	gGfxRenderer.GetRenderTargetViewport(mRenderTarget)->SetGridEnabled(false);
}

void Core::Game::UpdateGameProperties(void)
{
	PROFILE_FNC();

	// update current game properties
	GlobalProperties::SetPointer("Game", this);
	GlobalProperties::SetPointer("Physics", mPhysics);
	GlobalProperties::SetPointer("GameCamera", &mCamera);
}

void Core::Game::Init()
{
	if (GetState() != GS_NOT_INITED) Clean();

	ocInfo << "Game init";
	ForceStateChange(GS_INITING);

	// basic init stuff
	mActionState = AS_PAUSED;
	mTimer.Reset();

	// init physics engine
	OC_ASSERT(mPhysics);
	mPhysics->SetGravity(Vector2(0.0f, 10.0f));
	mPhysics->SetContactFilter(mPhysicsCallbacks);
	mPhysics->SetContactListener(mPhysicsCallbacks);
	mPhysics->SetDebugDraw(mPhysicsDraw);
	mPhysicsResidualDelta = 0.0f;


	// update globally accessible game related properties, like the physics engine
	UpdateGameProperties();

	gApp.RegisterGameInputListener(this);
	gApp.ResetStats();

	ForceStateChange(GS_NORMAL);
	ocInfo << "Game inited";
}

void Core::Game::Clean()
{	
	gApp.UnregisterGameInputListener(this);
	ForceStateChange(GS_CLEANING);

	for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
	{
		delete *i;
	}
	mPhysicsEvents.clear();
	
	ForceStateChange(GS_NOT_INITED);
	ocInfo << "Game cleaned";
}

void Core::Game::Update(const float32 delta)
{
	PROFILE_FNC();

	if (GetState() != GS_NORMAL) return;

	UpdateGameProperties();

	if (!IsActionRunning()) return;

	mTimer.UpdateInSeconds(delta);

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

		physicsDelta -= stepSize;
	}
	mPhysicsResidualDelta = physicsDelta;
}

void Core::Game::Draw(const float32 passedDelta)
{
	PROFILE_FNC();	

	if (GetState() != GS_NORMAL) return;

	float32 delta = passedDelta;
	if (!IsActionRunning()) delta = 0.0f;

	if (gApp.IsDevelopMode() && !IsActionRunning() && mActionRestarted)
	{
		// sync the game camera with the editor camera
		EntityHandle gameCamera = gEntityMgr.FindFirstEntity(GameCameraName);
		EntityHandle editorCamera = gEntityMgr.FindFirstEntity(Editor::EditorGUI::EditorCameraName);
		gameCamera.GetProperty("Zoom").SetValue(editorCamera.GetProperty("Zoom").GetValue<float32>());
		gameCamera.GetProperty("Rotation").SetValue(editorCamera.GetProperty("Rotation").GetValue<float32>());
		gameCamera.GetProperty("Position").SetValue(editorCamera.GetProperty("Position").GetValue<Vector2>());
	}
	
	gGfxRenderer.SetCurrentRenderTarget(mRenderTarget);
	gGfxRenderer.ClearCurrentRenderTarget(GfxSystem::Color(0, 0, 0));
	
	if (IsActionRunning())
	{
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::DRAW, Reflection::PropertyFunctionParameters() << delta));
	}

	gGfxRenderer.DrawEntities();

	gGfxRenderer.FinalizeRenderTarget();
}

bool Core::Game::KeyPressed(const KeyInfo& ke)
{
	if (!IsActionRunning()) return false;

	if (GlobalProperties::Get<bool>("DevelopMode"))
	{
		if (gEditorMgr.HandleShortcuts(ke.keyCode)) return true;
	}

	gEntityMgr.BroadcastMessage(EntityMessage::KEY_PRESSED, PropertyFunctionParameters() << ke.keyCode << ke.charCode);

	return true;
}

bool Core::Game::KeyReleased(const KeyInfo& ke)
{
	if (!IsActionRunning()) return false;

	gEntityMgr.BroadcastMessage(EntityMessage::KEY_RELEASED, PropertyFunctionParameters() << ke.keyCode << ke.charCode);

	return true;
}

bool Core::Game::MouseMoved(const MouseInfo& mi)
{
	if (!IsActionRunning()) return false;

	OC_UNUSED(mi);
	return false;
}

bool Core::Game::MouseButtonPressed(const MouseInfo& mi, const eMouseButton btn)
{
	if (!IsActionRunning()) return false;

	OC_UNUSED(mi);
	OC_UNUSED(btn);
	return false;
}

bool Core::Game::MouseButtonReleased(const MouseInfo& mi, const eMouseButton btn)
{
	if (!IsActionRunning()) return false;

	OC_UNUSED(btn);
	OC_UNUSED(mi);
	return false;
}

bool Core::Game::PhysicsCallbacks::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	if (!b2ContactFilter::ShouldCollide(fixtureA, fixtureB))
		return false;

	return true;
}

void Core::Game::PhysicsCallbacks::BeginContact(b2Contact* contact)
{
	PhysicsEvent* evt = new PhysicsEvent();

	evt->type = PhysicsEvent::COLLISION_STARTED;

	void* userData1 = contact->GetFixtureA()->GetUserData();
	void* userData2 = contact->GetFixtureB()->GetUserData();
	if (userData1) evt->entity1 = *(EntityHandle*)userData1;
	else evt->entity1.Invalidate();
	if (userData2) evt->entity2 = *(EntityHandle*)userData2;
	else evt->entity2.Invalidate();

	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	
	evt->normal = worldManifold.normal;

	Vector2 worldPoint = Vector2_Zero;
	int32 pointCount = contact->GetManifold()->pointCount;
	for (int32 i=0; i<pointCount; ++i)
	{
		worldPoint += worldManifold.points[i];
	}
	if (pointCount > 0)
	{
		worldPoint.x = worldPoint.x / pointCount;
		worldPoint.y = worldPoint.y / pointCount;
	}
	evt->contactPoint = worldPoint;

	mParent->mPhysicsEvents.push_back(evt);
}

void Core::Game::PhysicsCallbacks::EndContact(b2Contact* contact)
{
	PhysicsEvent* evt = new PhysicsEvent();

	evt->type = PhysicsEvent::COLLISION_ENDED;

	void* userData1 = contact->GetFixtureA()->GetUserData();
	void* userData2 = contact->GetFixtureB()->GetUserData();
	if (userData1) evt->entity1 = *(EntityHandle*)userData1;
	else evt->entity1.Invalidate();
	if (userData2) evt->entity2 = *(EntityHandle*)userData2;
	else evt->entity2.Invalidate();

	mParent->mPhysicsEvents.push_back(evt);
}

void Core::Game::ProcessPhysicsEvent(const PhysicsEvent& evt)
{
	if (!evt.entity1.IsValid() || !evt.entity2.IsValid() || !evt.entity1.Exists() || !evt.entity2.Exists())
		return;

	if (evt.type == PhysicsEvent::COLLISION_STARTED)
	{
		gEntityMgr.PostMessage(evt.entity1, EntityMessage(EntityMessage::COLLISION_STARTED, PropertyFunctionParameters() << evt.entity2 << evt.normal << evt.contactPoint));
		gEntityMgr.PostMessage(evt.entity2, EntityMessage(EntityMessage::COLLISION_STARTED, PropertyFunctionParameters() << evt.entity1 << evt.normal << evt.contactPoint));
	}
	else if (evt.type == PhysicsEvent::COLLISION_ENDED)
	{
		gEntityMgr.PostMessage(evt.entity1, EntityMessage(EntityMessage::COLLISION_ENDED, PropertyFunctionParameters() << evt.entity2));
		gEntityMgr.PostMessage(evt.entity2, EntityMessage(EntityMessage::COLLISION_ENDED, PropertyFunctionParameters() << evt.entity1));
	}
	else
	{
		ocError << "Unknown physics event";
	}
}

void Core::Game::PauseAction(void)
{
	if (mActionState == AS_RUNNING)
	{
		mActionState = AS_PAUSED;
	}
}

void Core::Game::ResumeAction(void)
{
	mActionRestarted = false;
	if (mActionState == AS_PAUSED)
	{
		mActionState = AS_RUNNING;
	}
}

void Core::Game::SaveAction(void)
{
	bool result = true;
	ResourceSystem::XMLOutput storage(gResourceMgr.GetBasePath(ResourceSystem::BPT_SYSTEM) + ActionFile);
	storage.BeginElement("Action");
	if (!gEntityMgr.SaveEntitiesToStorage(storage, false, true)) result = false;
	if (!SaveGameInfoToStorage(storage)) result = false;
	storage.EndElement();
	result = result && storage.CloseAndReport();
	
	if (result)
	{
		ocInfo << "Action saved in " << ActionFile;
	}
	else 
	{
		ocError << "Action cannot be saved!";
	}
}

void Core::Game::RestartAction(void)
{
	mActionRestarted = true;
	bool result = true;

	PauseAction();
	
	if (gResourceMgr.AddResourceFileToGroup(ActionFile, "Action", ResourceSystem::RESTYPE_AUTODETECT, ResourceSystem::BPT_SYSTEM))
	{
		gResourceMgr.LoadResourcesInGroup("Action");
		gEntityMgr.DestroyAllEntities(false, true);
		ResourceSystem::ResourcePtr resource = gResourceMgr.GetResource("Action", ActionFile);
		if (!LoadGameInfoFromResource(resource)) { result = false; }
		if (!gEntityMgr.LoadEntitiesFromResource(resource)) { result = false; }
		gResourceMgr.DeleteGroup("Action");
	}
	else
	{
		result = false;
	}

	if (result) ocInfo << "Action restarted from " << ActionFile;
	else ocError << "Action cannot be restarted!";
}

bool Core::Game::SaveGameInfoToStorage(ResourceSystem::XMLOutput& storage)
{
	storage.BeginElement("GameInfo");

	storage.BeginElement("CurrentTime");
	storage.WriteString(Utils::StringConverter::ToString(GetTimeMillis()));
	storage.EndElement();

	storage.EndElement();

	return true;
}

bool Core::Game::LoadGameInfoFromResource(ResourceSystem::ResourcePtr res)
{
	if (!res)
	{
		ocError << "XML: Can't load data; null resource pointer";
		return false;
	}
	
	ResourceSystem::XMLResourcePtr xml = res;
	uint64 currentTime = 0;

	for (ResourceSystem::XMLNodeIterator it = xml->IterateTopLevel(); it != xml->EndTopLevel(); ++it)
	{
		if ((*it).compare("GameInfo") != 0) { continue; }

		for (ResourceSystem::XMLNodeIterator entIt = it.IterateChildren(); entIt != it.EndChildren(); ++entIt)
		{
			if ((*entIt).compare("CurrentTime") == 0)
			{
				currentTime = entIt.GetValue<uint64>();
			}
		}
	}

	mTimer.Reset(currentTime);

	return true;
}
