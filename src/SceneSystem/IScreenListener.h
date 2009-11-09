/// @file
/// Notifications from the GfxSystem of changes to the screen.

#ifndef IScreenResolutionChangeListener_h__
#define IScreenResolutionChangeListener_h__

namespace SceneSystem
{
	/// Receives notifications from the GfxSystem of changes to the screen.
	class IScreenListener {
	public:
		
		/// Called when a screen resolution changes.
		virtual void ResolutionChanged(int width, int height) = 0;

		virtual ~IScreenListener(void) {}
	};
}

#endif // IScreenResolutionChangeListener_h__