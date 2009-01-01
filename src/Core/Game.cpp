#include "Common.h"
#include "Game.h"
#include "Application.h"
#include "Box2D.h"
#include <iostream>
#include "../GfxSystem/ParticleResource.h"
//TODO odstranit az bude voda jina
#include "hgedistort.h"

using namespace Core;
using namespace EntitySystem;
using namespace InputSystem;

#define PHYSICS_TIMESTEP 0.016f
#define PHYSICS_ITERATIONS 10
#define ENGINE_VISIBLE_MAX_POWER 80
#define WEAPON_VISIBLE_ARC_RADIUS 80
#define CAMERA_SPEED_RATIO 10.0f
#define CAMERA_SCALE_RATIO 0.001f
#define WATER_TEXTURE_SCALE 0.01f
#define WEAPON_ANGLECHANGE_SPEED 2.0f
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



	//// TEST ////

	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/materials.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/platforms.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/engines.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/ammo.xml"));
	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("ShipParts/weapons.xml"));

	gEntityMgr.LoadFromResource(gResourceMgr.GetResource("Ships/ship0.xml"));

	// recompute mass of the ship's body
	gEntityMgr.BroadcastMessage(EntityMessage::TYPE_PHYSICS_UPDATE_MASS);



	// set camera
	Vector2 shipPos;
	EntityHandle ship = gEntityMgr.FindFirstEntity("ship0");
	if (ship.IsValid())
	{
		ship.PostMessage(EntityMessage::TYPE_GET_POSITION, &shipPos);
		gGfxRenderer.SetCameraPos(shipPos);
	}
	//gGfxRenderer.SetCameraX(50.0f* gGfxRenderer.GetScreenWidthHalf());
	//gGfxRenderer.SetCameraY(50.0f* gGfxRenderer.GetScreenHeightHalf());
	gGfxRenderer.SetCameraScale(50.0f);
	mCameraFocus.Invalidate();


	// water
	GfxSystem::TexturePtr tex = gResourceMgr.GetResource("Backgrounds", "water.png");
	mWaterDistRows = 16;
	mWaterDistCols = 16;
	mWaterDistCellW = tex->GetWidth() / mWaterDistCols;
	mWaterDistCellH = tex->GetHeight() / mWaterDistRows;
	mWaterDistMesh = new hgeDistortionMesh(mWaterDistCols, mWaterDistRows);
	mWaterDistMesh->SetTexture(tex->GetTexture());
	mWaterDistMesh->SetTextureRect(0, 0, (float32)gGfxRenderer.GetScreenWidth(), (float32)gGfxRenderer.GetScreenHeight());
	mWaterDistMesh->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_ZWRITE);
	mWaterDistMesh->Clear(0xFF000000);



	gInputMgr.AddInputListener(this);





	gApp.ResetStats();

	gLogMgr.LogMessage("Game inited");
}

void Core::Game::Deinit()
{

}

void Core::Game::Update( const float32 delta )
{
	//// Input reactions ////

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

	//TODO tady se predpoklada, ze vsechny vybrane entity jsou jednoho typu, melo by jit ale vybrat vic typu najednou

	// we want to do certain action even when the right button is still down
	if (gInputMgr.IsMouseButtonPressed(MBTN_RIGHT) && mSelectedEntities.size()>0)
		MouseButtonPressed(mouse, MBTN_RIGHT);

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
		physicsDelta -= stepSize;
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
	/*static float32 waterTime = 0;
	waterTime += delta;

	int32 cX = gGfxRenderer.WorldToScreenScalar(gGfxRenderer.GetCameraWorldBoxTopLeft().x);
	int32 cY = gGfxRenderer.WorldToScreenScalar(gGfxRenderer.GetCameraWorldBoxTopLeft().y);
	for(int32 i=1; i<mWaterDistCols-1; ++i)
	{
		for(int32 j=1; j<mWaterDistRows-1; ++j)
		{
			int32 wX = cX/2 + mWaterDistCellW*i;
			int32 wY = cY/2 + mWaterDistCellH*j;
			//mWaterDistMesh->SetDisplacement(j,i,-cosf(t*2+((i*cellw+j*cellh-cX-cY)/(Zoom*2*(cellw+cellh))))*2,sinf(t*2+((i*cellw+j*cellh-cX-cY)/(Zoom*2*(cellw+cellh))))*2,HGEDISP_NODE);
			//col=int((cosf(t*2+((i*cellw+j*cellh-cX-cY)))+1)*10);						
			int32 col = MathUtils::Round((MathUtils::Cos(2.0f*waterTime + 0.09f*(wX + wY)) + 1) * 10);
			mWaterDistMesh->SetColor(j,i,0xFF<<24 | col<<16 | col<<8 | col);
		}
	}
	mWaterDistMesh->Render(0, 0);*/
	
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
	/*if (ke.keyAction == KC_F)
	{
		gGfxRenderer.SetFullscreen(!gGfxRenderer.IsFullscreen());
	}
	if (ke.keyAction==KC_D)
	{
		gGfxRenderer.ChangeResolution(800,600);
	}*/

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
	}
	else if (btn == MBTN_RIGHT)
	{
		// set the engine power and angle if selected
		Vector2 cursor = gGfxRenderer.ScreenToWorld(GfxSystem::Point(mi.x, mi.y));
		for (EntityList::iterator i=mSelectedEntities.begin(); i!=mSelectedEntities.end(); ++i)
		{
			eEntityType type = i->GetType();
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
				dir = MathUtils::Dot(dir, unitDir) * unitDir;

				int32 screenDist = gGfxRenderer.WorldToScreenScalar(dir.Length());
				float32 powerRatio = (float32)screenDist / (float32)GetEnginePowerCircleRadius();
				i->PostMessage(EntityMessage::TYPE_SET_POWER_RATIO, &powerRatio);
			}
			else if (type == ET_WEAPON)
			{
				if ((gInputMgr.IsKeyDown(KC_LMENU) || gInputMgr.IsKeyDown(KC_RMENU)) && mHoveredEntity.IsValid())
				{
					PropertyHolder prop;
					prop = i->GetProperty("Target");
					prop.SetValue(mHoveredEntity);
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
						i->PostMessage(EntityMessage::TYPE_STOP_SHOOTING);
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