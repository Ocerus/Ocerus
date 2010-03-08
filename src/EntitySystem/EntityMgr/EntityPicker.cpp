#include "Common.h"
#include "EntityPicker.h"
#include <Box2D.h>

using namespace EntitySystem;

const size_t MAX_QUERY_SHAPES = 128;
b2Shape* gQueryShapes[MAX_QUERY_SHAPES];

EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos ):
	mCursorWorldPosition(worldCursorPos),
	mResult()
{

}

EntitySystem::EntityHandle EntitySystem::EntityPicker::PickSingleEntity( void )
{
	b2AABB cursorAABB;
	Vector2 delta(0.0001f, 0.0001f);
	cursorAABB.lowerBound = mCursorWorldPosition - delta;
	cursorAABB.upperBound = mCursorWorldPosition + delta;

	// get all shapes under the cursor
	int32 shapesCount = GlobalProperties::Get<b2World>("Physics").Query(cursorAABB, gQueryShapes, MAX_QUERY_SHAPES);

	// find the shape with the lowest depth value
	int32 lowestDepth = INT32_MAX;
	EntityHandle lowestDepthEntity = EntityHandle::Null;
	for (int32 i=0; i<shapesCount; ++i)
	{
		// make sure we did really hit it
		b2Body* shapeBody = gQueryShapes[i]->GetBody();
		OC_DASSERT(shapeBody);
		if (!gQueryShapes[i]->TestPoint(shapeBody->GetXForm(), mCursorWorldPosition)) continue;

		// check the depth
		EntityHandle entity = *(EntityHandle*)gQueryShapes[i]->GetUserData();
		int32 depth = entity.GetProperty("Depth").GetValue<int32>();
		if (depth < lowestDepth)
		{
			depth = lowestDepth;
			lowestDepthEntity = entity;
		}
	}

	return lowestDepthEntity;
}

void EntitySystem::EntityPicker::PickMultipleEntities( vector<EntityHandle>& out )
{
	OC_UNUSED(out);
}
