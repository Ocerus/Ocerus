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
		b2Body* body = gQueryShapes[i]->GetBody();
		OC_ASSERT(body);
		if (!gQueryShapes[i]->TestPoint(body->GetXForm(), mCursorWorldPosition)) continue;

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

uint32 EntitySystem::EntityPicker::PickMultipleEntities( const Vector2& worldCursorPos, const float32 rotation, vector<EntityHandle>& out )
{
	Vector2 A = mCursorWorldPosition;
	Vector2 B = worldCursorPos;
	if (A.x > B.x) MathUtils::Swap(A, B);

	Vector2 r;
	if (rotation != 0.0f)
	{
		// this calculation should be clear once you draw it on a paper :)
		Vector2 diagonal = B - A;
		Matrix22 rotator(rotation);
		r = MathUtils::Multiply(rotator, Vector2(1,0));
		r = MathUtils::Dot(r, diagonal) * r;
	}
	else if (A.y < B.y)
	{
		r = Vector2(B.x-A.x, 0);
	}
	else
	{
		r = Vector2(0, B.y-A.y);
	}
	
	///@TODO tohle nahradit fixturou v dalsi verzi box2d
	b2World* physics = GlobalProperties::GetPointer<b2World>("Physics");
	b2PolygonDef shapeDef;
	shapeDef.vertexCount = 4;
	shapeDef.vertices[0] = A + r;
	shapeDef.vertices[1] = B;
	shapeDef.vertices[2] = B - r;
	shapeDef.vertices[3] = A;
	// make sure the shape is not inside out
	if (MathUtils::Cross(shapeDef.vertices[1]-shapeDef.vertices[0], shapeDef.vertices[2]-shapeDef.vertices[1]) < 0.0f)
	{
		MathUtils::Swap(shapeDef.vertices[1], shapeDef.vertices[3]);
	}
	b2Body* groundBody = physics->GetGroundBody();
	b2Shape* selectionShape = groundBody->CreateShape(&shapeDef);

	// get all shapes under the cursor
	b2AABB aabb;
	selectionShape->ComputeAABB(&aabb, b2XForm_identity);
	int32 shapesCount = physics->Query(aabb, gQueryShapes, MAX_QUERY_SHAPES);

	// fill the vector with results
	for (int32 i=0; i<shapesCount; ++i)
	{
		b2Shape* shape = gQueryShapes[i];
		if (shape == selectionShape) continue;

		b2Body* body = shape->GetBody();
		OC_ASSERT(body);

		OC_ASSERT(shape->GetType() == e_polygonShape);
		b2Manifold manifold;
		b2CollidePolygons(&manifold, (b2PolygonShape*)selectionShape, b2XForm_identity, (b2PolygonShape*)shape, body->GetXForm());
		if (manifold.pointCount > 0)
		{
			EntityHandle entity = *(EntityHandle*)shape->GetUserData();
			out.push_back(entity);
		}
	}

	groundBody->DestroyShape(selectionShape);

	return out.size();
}
