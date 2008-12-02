#include "Common.h"
#include "CmpPlatformLogic.h"

using namespace EntitySystem;

void CmpPlatformLogic::Init(ComponentDescription& desc)
{
	SetBlueprints(desc.GetNextItem()->GetData<EntityHandle>());
	mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);
	SetParentShip(desc.GetNextItem()->GetData<EntityHandle>());
}

void CmpPlatformLogic::Deinit() 
{

}

EntityMessage::eResult CmpPlatformLogic::HandleMessage(const EntityMessage& msg)
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_BLUEPRINTS:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetBlueprints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PARENT:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetParentShip();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_ADD_PLATFORM_ITEM:
		assert(msg.data);
		mItems.push_back(*(EntityHandle*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_MOUSE_PICK:
		assert(msg.data);
		{
			Vector2 pos;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
			float32 angle;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
			((EntityPicker*)msg.data)->Update(GetOwner(), MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos, mPickCircleRadius);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_OVERLAY:
		assert(msg.data);
		DrawSelectionOverlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_POST_INIT:
		{
			// compute pick stuff
			DataContainer cont;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
			Vector2* shape = (Vector2*)cont.GetData();
			int32 shapeLen = cont.GetSize();
			// compute center
			//TODO najit ten stred tak, aby to udelalo nejlepsi bounding circle
			mPickCircleCenter.SetZero();
			for (int i=0; i<shapeLen; ++i)
				mPickCircleCenter += shape[i];
			mPickCircleCenter *= 1.0f/shapeLen;
			// compute radius
			mPickCircleRadius = 0.0f;
			for (int i=0; i<shapeLen; ++i)
			{
				float32 dist = MathUtils::Distance(mPickCircleCenter, shape[i]);
				if (dist > mPickCircleRadius)
					mPickCircleRadius = dist;
			}
		}
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformLogic::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentShip", &GetParentShip, &SetParentShip, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpPlatformLogic::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
	float32 angle;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
	GfxSystem::Color color(255,0,0,180);
	if (hover)
		color = GfxSystem::Color(255,255,255,180);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos), gGfxRenderer.WorldToScreenScalar(mPickCircleRadius), 
		GfxSystem::Color::NullColor, GfxSystem::Pen(color));
}