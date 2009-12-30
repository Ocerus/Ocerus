/// @file
/// Sprite component.

#ifndef Camera_h__
#define Camera_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Representation of a camera to be used to display the scene to the user. The camera is used by the GfxSystem
	/// while rendering the scene.
	class Camera : public RTTIGlue<Camera, Component>
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
		
		float32 GetZoom(void) const { return mZoom; }
		void SetZoom(float32 value) { mZoom = value; }
		
		float32 GetRotation(void) const { return mRotation; }
		void SetRotation(float32 value) { mRotation = value; }

		Vector2 GetPosition(void) const { return mPosition; }
		void SetPosition(Vector2 value) { mPosition = value; }	

	private:
		float32 mZoom;
		float32 mRotation;
		Vector2 mPosition;
	};
}

#endif