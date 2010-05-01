/// @file
/// 3D model component.

#ifndef Model_h__
#define Model_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Visual 3D representation of the entity.
	/// It allows the entity to define its visual appearance as a 3D model loaded from a resource. It is automatically
	/// used by GfxSystem while rendering the scene.
	class Model : public RTTIGlue<Model, Component>
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

		/// Mesh used for rendering.
		ResourceSystem::ResourcePtr GetMesh(void) const { return mMeshHandle; }

		/// Mesh used for rendering.
		void SetMesh(ResourceSystem::ResourcePtr value);

		/// Transparency from 0 to 1.
		float32 GetTransparency(void) const { return mTransparency; }

		/// Transparency from 0 to 1.
		void SetTransparency(float32 value) { mTransparency = value; }

		/// Angle along the Z axis.
		float32 GetZAngle(void) const { return mZAngle; }

		/// Angle along the Z axis.
		void SetZAngle(float32 value) { mZAngle = value; }

	private:

		ResourceSystem::ResourcePtr mMeshHandle;
		float32 mTransparency;
		float32 mZAngle;
	};
}

#endif // Model_h__
