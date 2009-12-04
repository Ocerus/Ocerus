#include "Common.h"
#include "EntityPicker.h"

using namespace EntitySystem;


EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos ):
	mCursorWorldPosition(worldCursorPos),
	mResult()
{

}

EntitySystem::EntityPicker::EntityPicker( const int32 screenCursorX, const int32 screenCursorY ):
	mResult()
{
	//TODO:Gfx
	//mCursorWorldPosition.x = gGfxRenderer.ScreenToWorldX(screenCursorX);
	//mCursorWorldPosition.y = gGfxRenderer.ScreenToWorldY(screenCursorY);
}

void EntitySystem::EntityPicker::Update( const EntityHandle entity, const Vector2& circleCenter, const float32 circleRadius )
{
	if (MathUtils::DistanceSquared(circleCenter, mCursorWorldPosition) <= MathUtils::Sqr(circleRadius))
		PickIfHigherPriority(entity);
}

void EntitySystem::EntityPicker::Update( const EntityHandle entity, const float32 rectX1, const float32 rectY1, const float32 rectX2, const float32 rectY2 )
{
	if (mCursorWorldPosition.x>=rectX1 && mCursorWorldPosition.y>=rectY1
		&& mCursorWorldPosition.x<=rectX2 && mCursorWorldPosition.y<=rectY2)
		PickIfHigherPriority(entity);
}

EntitySystem::EntityHandle EntitySystem::EntityPicker::GetResult( void ) const
{
	return mResult;
}

void EntitySystem::EntityPicker::PickIfHigherPriority( const EntityHandle entity )
{
	mResult = entity;
}

