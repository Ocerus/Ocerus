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
		void SetPolygon(Array<Vector2>* val);

		/// Density of the shape's material.
		float32 GetDensity(void) const;

		/// Density of the shape's material.
		void SetDensity(float32 val);

		/// Friction of the material.
		float32 GetFriction(void) const;
		
		/// Friction of the material.
		void SetFriction(float32 val);

		/// Restitution of the material.
		float32 GetRestitution(void) const;

		/// Restitution of the material.
		void SetRestitution(float32 val);

		/// Physical shape.
		PhysicalShape* GetShape(void) const { return mShape; }

	private:

		PhysicalShape* mShape;
		PhysicalBody* mSensorBody;
		Array<Vector2> mPolygon;
		Vector2 mPolygonScale;
		mutable float32 mDensity;
		mutable float32 mFriction;
		mutable float32 mRestitution;


		/// Creates the shape from scratch. If any previous shape existed it gets deleted.
		void RecreateShape(void);

		/// Removes the shape from world (body).
		void DestroyShape(void);

		/// Fills the array with raw vertices for Box2d.
		void GetBox2dVertices(Array<Vector2>* polygon, b2Vec2* vertices);

	};
}

#endif // Collidable_h__
