#include "Common.h"
#include "EntityPicker.h"
#include <Box2D.h>

using namespace EntitySystem;

class QueryCallback: public b2QueryCallback
{
public:

	QueryCallback(): count(0) {}

	virtual ~QueryCallback() {}

	virtual bool ReportFixture(PhysicalShape* shape)
	{
		if (count >= MAX_QUERY_SHAPES) return false;
		shapes[count++] = shape;
		return true;
	}

	static const int32 MAX_QUERY_SHAPES = 128;
	PhysicalShape* shapes[MAX_QUERY_SHAPES];
	int32 count;
};


EntitySystem::EntityPicker::EntityPicker( const Vector2& worldCursorPos, const int32 minLayer, const int32 maxLayer ):
	mCursorWorldPosition(worldCursorPos),
	mMinLayer(minLayer),
	mMaxLayer(maxLayer),
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
	QueryCallback query;
	Physics* physics = GlobalProperties::GetPointer<b2World>("Physics");
	physics->QueryAABB(&query, cursorAABB);

	// find the shape with the lowest depth value
	int32 lowestDepth = numeric_limits<int32>::max();
	EntityHandle lowestDepthEntity = EntityHandle::Null;
	for (int32 i=0; i<query.count; ++i)
	{
		// make sure we did really hit it
		PhysicalShape* shape = query.shapes[i];
		if (!shape->TestPoint(mCursorWorldPosition)) continue;

		// check the layer
		EntityHandle entity = *(EntityHandle*)shape->GetUserData();
		int32 depth = entity.GetProperty("Layer").GetValue<int32>();
		if (depth >= mMinLayer && depth <= mMaxLayer && depth < lowestDepth)
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
	
	Physics* physics = GlobalProperties::GetPointer<b2World>("Physics");

	// create the collision rectangle
	Vector2 vertices[4];
	vertices[0] = A + r;
	vertices[1] = B;
	vertices[2] = B - r;
	vertices[3] = A;
	// make sure the shape is not inside out
	if (MathUtils::Cross(vertices[1]-vertices[0], vertices[2]-vertices[1]) < 0.0f)
	{
		MathUtils::Swap(vertices[1], vertices[3]);
	}
	b2PolygonShape selectionShape;
	selectionShape.Set(vertices, 4);

	// get all shapes under the cursor
	b2AABB aabb;
	selectionShape.ComputeAABB(&aabb, XForm_Identity);
	QueryCallback query;
	physics->QueryAABB(&query, aabb);

	// fill the vector with results
	for (int32 i=0; i<query.count; ++i)
	{
		PhysicalShape* shape = query.shapes[i];

		b2Body* body = shape->GetBody();
		OC_ASSERT(body);

		if (b2TestOverlap(&selectionShape, shape->GetShape(), XForm_Identity, body->GetTransform()))
		{
			EntityHandle entity = *(EntityHandle*)shape->GetUserData();
			
			// check the layer
			int32 layer = entity.GetProperty("Layer").GetValue<int32>();
			if (layer >= mMinLayer && layer <= mMaxLayer)
			{
				out.push_back(entity);
			}
		}
	}

	return out.size();
}
