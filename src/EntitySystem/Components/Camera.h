/// @file
/// Sprite component.

#ifndef Camera_h__
#define Camera_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Sprite component.
	class Camera : public RTTIGlue<Camera, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);
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