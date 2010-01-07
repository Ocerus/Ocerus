/// @file
/// Notifications from the GfxSystem of changes to the screen.

#ifndef IGfxWindowListener_h__
#define IGfxWindowListener_h__

namespace GfxSystem
{
	/// Receives notifications from the GfxSystem of changes to the screen.
	class IGfxWindowListener {
	public:

		/// Called when a screen resolution changes.
		virtual void ResolutionChanged(int width, int height) = 0;

		virtual ~IGfxWindowListener(void) {}
	};
}

#endif // IGfxWindowListener_h__