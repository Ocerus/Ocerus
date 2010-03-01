#include "Common.h"
#include "EntityPicker.h"

using namespace EntitySystem;


EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos ):
	mCursorWorldPosition(worldCursorPos),
	mResult()
{

}

EntitySystem::EntityPicker::EntityPicker( const int32 screenCursorX, const int32 screenCursorY, const EntityHandle camera ):
	mResult()
{
	//TODO:Gfx
	//mCursorWorldPosition.x = gGfxRenderer.ScreenToWorldX(screenCursorX);
	//mCursorWorldPosition.y = gGfxRenderer.ScreenToWorldY(screenCursorY);
}

EntitySystem::EntityHandle EntitySystem::EntityPicker::PickSingleEntity( void )
{
	return EntityHandle::Null;
}

void EntitySystem::EntityPicker::PickMultipleEntities( vector<EntityHandle>& out )
{

}
