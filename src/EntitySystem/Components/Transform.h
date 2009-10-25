/// @file
/// Transformation of the entity in the game world.

#ifndef Transform_h__
#define Transform_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Transformation of the entity in the game world.
	class Transform : public RTTIGlue<Transform, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);
		static void RegisterReflection(void);

		/// Position of the entity in world coordinates.
		Vector2 GetPosition(void) const { return mPosition; }
		/// Position of the entity in world coordinates.
		void SetPosition(Vector2 pos) { mPosition = pos; }
		/// Scale of the entity with respect to the X and Y axes in the entity's space.
		Vector2 GetScale(void) const { return mScale; }
		/// Scale of the entity with respect to the X and Y axes in the entity's space.
		void SetScale(Vector2 value) { mScale = value; }
		/// Angle of the entity in radians.
		float32 GetAngle(void) const { return mAngle; }
		/// Angle of the entity in radians.
		void SetAngle(float32 value) { mAngle = value; }
		/// Depth of the entity (ala the Z coordinate).
		int32 GetDepth(void) const { return mDepth; }
		/// Depth of the entity (ala the Z coordinate).
		void SetDepth(int32 value) { mDepth = value; }

	private:
		Vector2 mPosition;
		Vector2 mScale;
		float32 mAngle;
		int32 mDepth;
	};
}

#endif