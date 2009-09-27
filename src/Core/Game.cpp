#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"
#include "../GfxSystem/ParticleSystem.h"
#include "WaterSurface.h"
#include "Properties.h"
#include "DataContainer.h"
#include "StringConverter.h"

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
	mMyTeam(-1),
	mLastClickTime(0),
	mWaterSurface(0) {}

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
	worldAABB.lowerBound.Set(-10000.0f, -10000.0f);
	worldAABB.upperBound.Set(10000.0f, 10000.0f);
	// turn off sleeping as we are moving in a space with no gravity
	mPhysics = DYN_NEW b2World(worldAABB, b2Vec2(0.0f, 0.0f), false);
	mPhysics->SetContactFilter(this);
	mPhysics->SetContactListener(this);
	mPhysicsResidualDelta = 0.0f;



	//// TEST ////

	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/materials.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/platforms.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/engines.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/ammo.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/weapons.xml"));

	//gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/old_ship0.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/ship0.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/ship1.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/ship2.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/ship3.xml"));


	// recompute mass of the ship's body
	gEntityMgr.BroadcastMessage(EntityMessage::TYPE_PHYSICS_UPDATE_MASS);



	// set camera
	mCameraFocus.Invalidate();
	Vector2 shipPos;
	EntityHandle ship = gEntityMgr.FindFirstEntity("ship0");
	if (ship.IsValid())
	{
		ship.PostMessage(EntityMessage::TYPE_GET_POSITION, &shipPos);
		gGfxRenderer.SetCameraPos(shipPos);
		mMyTeam = ship.GetTeam();
		mCameraFocus = ship;
	}
	gGfxRenderer.SetCameraScale(50.0f);


	// water
	GfxSystem::TexturePtr tex = gResourceMgr.GetResource("Backgrounds", "water2.png");
	mWaterSurface = DYN_NEW WaterSurface(tex, 0.05f, 0.4f, 0.4f, 128, 128);
	//mWaterSurface = DYN_NEW WaterSurface(tex, 0.05f, 1.0f, 1.0f, 8, 8);


	gInputMgr.AddInputListener(this);


	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");
}

void Core::Game::Deinit()
{
	if (mPhysics)
		DYN_DELETE mPhysics;
	for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		DYN_DELETE *i;
	mPhysicsEvents.clear();
	if (mWaterSurface)
		DYN_DELETE mWaterSurface;
}

void Core::Game::Update( const float32 delta )
{
	//// Input reactions ////

	// pick hover entity
	MouseState& mouse = gInputMgr.GetMouseState();
	EntityPicker picker(mouse.x, mouse.y);
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_MOUSE_PICK, &picker));
	mHoveredEntity = picker.GetResult();

	//TODO tady se predpoklada, ze vsechny vybrane entity jsou jednoho typu, melo by jit ale vybrat vic typu najednou

	// we want to do certain action even when the right button is still down
	if (gInputMgr.IsMouseButtonPressed(MBTN_RIGHT) && mSelectedEntities.size()>0)
		MouseButtonPressed(mouse, MBTN_RIGHT);

	// make sure we didn't lose any of the selected entities
	for (EntityList::const_iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end();)
	{
		if (i->GetTeam() != mMyTeam)
			i = mSelectedEntities.erase(i);
		else
			++i;
	}

	// control by using keys
	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
	{
		eEntityType type = i->GetType();
		if (type == ET_ENGINE)
		{
			if (gInputMgr.IsKeyDown(KC_NUMPAD4) || gInputMgr.IsKeyDown(KC_NUMPAD6))
			{
				float32 angle;
				i->PostMessage(EntityMessage::TYPE_GET_RELATIVE_ANGLE, &angle);
				angle += (gInputMgr.IsKeyDown(KC_NUMPAD4)?-1:1) * ENGINE_ANGLECHANGE_SPEED * delta;
				i->PostMessage(EntityMessage::TYPE_SET_RELATIVE_ANGLE, &angle);
			}
			if (gInputMgr.IsKeyDown(KC_NUMPAD8) || gInputMgr.IsKeyDown(KC_NUMPAD5))
			{
				float32 powerRatio;
				i->PostMessage(EntityMessage::TYPE_GET_POWER_RATIO, &powerRatio);
				powerRatio += (gInputMgr.IsKeyDown(KC_NUMPAD5)?-1:1) * ENGINE_POWERCHANGE_SPEED * delta;
				i->PostMessage(EntityMessage::TYPE_SET_POWER_RATIO, &powerRatio);
			}
		}
		else if (type == ET_WEAPON)
		{
			if (gInputMgr.IsKeyDown(KC_NUMPAD4) || gInputMgr.IsKeyDown(KC_NUMPAD6))
			{
				float32 angle;
				i->PostMessage(EntityMessage::TYPE_GET_RELATIVE_ANGLE, &angle);
				angle += (gInputMgr.IsKeyDown(KC_NUMPAD4)?-1:1) * WEAPON_ANGLECHANGE_SPEED * delta;
				i->PostMessage(EntityMessage::TYPE_SET_RELATIVE_ANGLE, &angle);
			}
		}
	}


	float32 physicsDelta = delta + mPhysicsResidualDelta;
	while (physicsDelta > PHYSICS_TIMESTEP)
	{
		float32 stepSize = PHYSICS_TIMESTEP;
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_SERVER, &stepSize));
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_UPDATE_PHYSICS_CLIENT, &stepSize));
		mPhysics->Step(stepSize, PHYSICS_ITERATIONS);

		// process physics events
		for (PhysicsEventList::const_iterator i=mPhysicsEvents.begin(); i!=mPhysicsEvents.end(); ++i)
		{
			ProcessPhysicsEvent(**i);
			DYN_DELETE *i;
		}
		mPhysicsEvents.clear();

		// destroy marked entities
		gEntityMgr.ProcessDestroyQueue();

		// if some of the selected entities were destroyed, remove it
		if (!mHoveredEntity.Exists())
			mHoveredEntity.Invalidate();
		for (EntityList::const_iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end();)
		{
			if (!it->Exists())
				it = mSelectedEntities.erase(it);
			else
				++it;
		}
		for (int32 i=0; i<MAX_SELECTED_GROUPS; ++i)
			for (EntityList::const_iterator it=mSelectedGroups[i].begin(); it!=mSelectedGroups[i].end();)
			{
				if (!it->Exists())
					it = mSelectedGroups[i].erase(it);
				else
					++it;
			}
		if (!mCameraFocus.Exists())
			mCameraFocus.Invalidate();

		physicsDelta -= stepSize;
	}
	mPhysicsResidualDelta = physicsDelta;

	// water
	char* waterPlatforms[] = {"platform5", "platform6", "platform1", "platform2", "platform20", "platform21"};
	for (int i=0; i<sizeof(waterPlatforms)/sizeof(char*); ++i)
	{
		EntityHandle platform = gEntityMgr.FindFirstEntity(waterPlatforms[i]);
		if (platform.Exists())
		{
			DataContainer cont;
			Vector2 pos, dir;
			float32 angle;
			platform.PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
			platform.PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
			platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
			mWaterSurface->LowerArea((Vector2*)cont.GetData(), cont.GetSize(), pos, angle);
		}
	}

	char* bubblePlatforms[] = {"platform20", "platform21", "platform22", "platform23"};
	const int len = sizeof(bubblePlatforms)/sizeof(char*);
	if (mBubbleEffects.size() == 0)
	{
		for (int32 i=0; i<len; ++i)
			mBubbleEffects.push_back(gPSMgr.SpawnPS("psi", "particle8.psi"));
	}
	for (int i=0; i<len; ++i)
	{
		EntityHandle platform = gEntityMgr.FindFirstEntity(bubblePlatforms[i]);
		if (platform.Exists())
		{
			PropertyHolder prop = platform.GetProperty("AbsolutePosition");
			Vector2 pos = prop.GetValue<Vector2>();
			prop = platform.GetProperty("ParentShip");
			EntityHandle ship = prop.GetValue<EntityHandle>();
			if (ship.Exists())
			{
				prop = ship.GetProperty("AbsolutePosition");
				Vector2 shipPos = prop.GetValue<Vector2&>();
				mBubbleEffects[i]->MoveTo(pos.x, pos.y, false);
				mBubbleEffects[i]->SetScale(0.2f);
				mBubbleEffects[i]->SetAngle(MathUtils::Angle(pos - shipPos));
				mBubbleEffects[i]->Fire();
			}
			else
			{
				mBubbleEffects[i]->Stop();
			}
		}
	}

	mWaterSurface->Update(delta);


	//particle effects
	gPSMgr.Update(delta);
};

void Core::Game::Draw( const float32 delta)
{
	// move camera in reaction to the user input
	if (gInputMgr.IsKeyDown(KC_LEFT))
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
		Vector2 pos;
		mCameraFocus.PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
		gGfxRenderer.SetCameraPos(pos);
	}

	// clear the screen
	gGfxRenderer.ClearScreen(GfxSystem::Color(0,0,0));

	// draw the water
	mWaterSurface->Draw();

	// draw bubbles
	for (vector<GfxSystem::ParticleSystemPtr>::const_iterator it=mBubbleEffects.begin(); it!=mBubbleEffects.end(); ++it)
		(*it)->Render();

	
	/*GfxSystem::TexturePtr waterTex = gResourceMgr.GetResource("Backgrounds", "water.png");
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
			gGfxRenderer.DrawImage(waterTex, GfxSystem::Rect(x, y, texW, texH));*/

	// draw ships
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM));
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PLATFORM_LINK));

	// draw direction lines to the mouse cursor
	if (mSelectedEntities.size() > 0)
	{
		MouseState& mouse = gInputMgr.GetMouseState();
		for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		{
			eEntityType type = i->GetType();
			if (type == ET_ENGINE || type == ET_WEAPON)
			{
				GfxSystem::Pen pen(GfxSystem::Color(100,100,100,100));
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

	// draw projectiles
	gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::TYPE_DRAW_PROJECTILE));
	
	// draw remaining (undrawn) particle effects
	gPSMgr.Render();
}

void Core::Game::KeyPressed( const KeyInfo& ke )
{
	if (ke.keyAction == KC_ESCAPE)
		gApp.Shutdown();

	if (ke.keyAction == KC_F)
	{
		gGfxRenderer.SetFullscreen(!gGfxRenderer.IsFullscreen());
	}
	if (ke.keyAction==KC_G)
	{
		gGfxRenderer.ChangeResolution(1280,1024);
	}
	if (ke.keyAction==KC_H)
	{
		gGfxRenderer.ChangeResolution(800,600);
	}

	if (ke.keyAction >= KC_1 && ke.keyAction <= KC_0)
	{
		int32 groupIndex = ke.keyAction - KC_1;
		if (gInputMgr.IsKeyDown(KC_LCONTROL) || gInputMgr.IsKeyDown(KC_RCONTROL))
			mSelectedGroups[groupIndex] = mSelectedEntities;
		else if (mSelectedGroups[groupIndex].size() > 0)
			mSelectedEntities = mSelectedGroups[groupIndex];
	}

	for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
	{
		eEntityType type = i->GetType();
		if (type == ET_WEAPON && gInputMgr.IsKeyDown(KC_SPACE))
		{
			PropertyHolder prop;
			prop = i->GetProperty("IsFiring");
			if (prop.GetValue<bool>())
				i->PostMessage(EntityMessage::TYPE_STOP_SHOOTING);
			else
				i->PostMessage(EntityMessage::TYPE_START_SHOOTING);
		}
	}
}

void Core::Game::KeyReleased( const KeyInfo& ke )
{

}

void Core::Game::MouseMoved( const MouseInfo& mi )
{
	if (gInputMgr.IsMouseButtonPressed(MBTN_MIDDLE))
	{
		mCameraFocus.Invalidate();
		Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
		gGfxRenderer.SetCameraPos(gGfxRenderer.GetCameraPos() - cursor + mCameraGrabWorldPos);
	}

	// zoom camera
	if (mi.wheelDelta)
		gGfxRenderer.ZoomCamera(CAMERA_SCALE_RATIO * gGfxRenderer.GetCameraScale() * mi.wheelDelta);
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
			// select all entities of the same type when doubleclicked
			if (doubleClick)
			{
				eEntityType type = mHoveredEntity.GetType();
				gEntityMgr.EnumerateEntities(mSelectedEntities, type, mMyTeam);
			}
			// use the hover entity as a focus
			else if (gInputMgr.IsKeyDown(KC_RCONTROL) || gInputMgr.IsKeyDown(KC_LCONTROL))
			{
				mCameraFocus = mHoveredEntity;
			}
			// select controllable entities
			else if (mHoveredEntity.GetTeam() == mMyTeam)
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
		Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
		for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		{
			eEntityType type = i->GetType();
			// set the engine power and angle if selected
			if (type == ET_ENGINE)
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
				float32 dot = MathUtils::Dot(dir, unitDir);

				int32 screenDist = gGfxRenderer.WorldToScreenScalar(dot);
				float32 powerRatio = (float32)screenDist / (float32)GetEnginePowerCircleRadius();
				i->PostMessage(EntityMessage::TYPE_SET_POWER_RATIO, &powerRatio);
			}
			else if (type == ET_WEAPON)
			{
				if (!gInputMgr.IsKeyDown(KC_LMENU) && !gInputMgr.IsKeyDown(KC_RMENU) && mHoveredEntity.IsValid())
				{
					PropertyHolder prop;
					prop = i->GetProperty("Target");
					prop.SetValue(mHoveredEntity);
					if (prop.GetValue<EntityHandle>().IsValid())
						i->PostMessage(EntityMessage::TYPE_START_SHOOTING);
				}
				else
				{
					PropertyHolder prop;
					prop = i->GetProperty("Target");
					EntityHandle curTarget = prop.GetValue<EntityHandle>();
					if (curTarget.IsValid())
					{
						prop.SetValue(EntityHandle::Null);
						// note: I removed this cos it caused the weapon to stop shooting even while the mouse moved (while the right button was pressed)
						//i->PostMessage(EntityMessage::TYPE_STOP_SHOOTING);
					}

					Vector2 pos;
					// if ALT is pressed, we want to set the direction from the ship center instead of the weapon center
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
				}
			}
		}
	}
	else if (btn == MBTN_MIDDLE)
	{
		mCameraGrabWorldPos = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
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

	EntityHandle firstEntity = *(EntityHandle*)shape1->GetUserData();
	EntityHandle secondEntity = *(EntityHandle*)shape2->GetUserData();

	TeamID team1 = gEntityMgr.GetEntityTeam(firstEntity);
	TeamID team2 = gEntityMgr.GetEntityTeam(secondEntity);

	if (team1 == 0 || team2 == 0)
		return true;

	return team1 != team2;
}

void Core::Game::Add( const b2ContactPoint* point )
{
	//TODO tady to chce pooling, zbytecna casta alokace malych objektu
	PhysicsEvent* evt = DYN_NEW PhysicsEvent();
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
	if (type1 == ET_PROJECTILE)
		gEntityMgr.PostMessage(ent1, EntityMessage(EntityMessage::TYPE_STRIKE, &ent2));
	else if (type2 == ET_PROJECTILE)
		gEntityMgr.PostMessage(ent2, EntityMessage(EntityMessage::TYPE_STRIKE, &ent1));

}
