#include "Common.h"
#include "EntityPicker.h"

using namespace EntitySystem;


EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos ):
	mCursorWorldPosition(worldCursorPos),
	mResult()
{

}

EntitySystem::EntityHandle EntitySystem::EntityPicker::PickSingleEntity( void )
{
	return EntityHandle::Null;
}

void EntitySystem::EntityPicker::PickMultipleEntities( vector<EntityHandle>& out )
{

}
