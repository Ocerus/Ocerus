/// @file
/// Physical representation of a collidable entity.

#ifndef Collidable_h__
#define Collidable_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Physical representation of static entities. This component works as a layer between entities and the physics engine.
	/// It attaches itself to a physical body if there is any in the entity, or to a fixture (null body) otherwise.
	class PolygonCollider: public RTTIGlue<PolygonCollider, Component>
	{
	public:

		/// Called after the component is created.
		virtual void Create(void);

		/// Called before the component is destroyed.
		virtual void Destroy(void);

		/// Called when a new message arrives.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Called from RTTI when the component is allowed to set up its properties.
		static void RegisterReflection(void);

		/// Definition of the polygon.
		Array<Vector2>* GetPolygon(void) const { return const_cast<Array<Vector2>*>(&mPolygon); }

		/// Definition of the polygon.
		/// @todo Update the physics when the value changes.
		void SetPolygon(Array<Vector2>* val) { mPolygon.CopyFrom(*val); }

		/// Density of the shape's material.
		float32 GetDensity(void) const { return mDensity; }

		/// Density of the shape's material.
		/// @todo Update the physics when the value changes.
		void SetDensity(float32 val) { mDensity = val; }

	private:

		PhysicalShape* mShape;
		PhysicalBody* mSensorBody;
		Array<Vector2> mPolygon;
		float32 mDensity;

		/// Creates the shape from scratch. If any previous shape existed it gets deleted.
		void RecreateShape(void);

		/// Removes the shape from world (body).
		void DestroyShape(void);

	};
}

#endif // Collidable_h__
