/// @file
/// Notifications from the GfxSystem of changes to the screen.

#ifndef IGfxWindowListener_h__
#define IGfxWindowListener_h__

namespace GfxSystem
{
	/// Receives notifications from the GfxSystem of changes to the screen.
	class IGfxWindowListener {
	public:

		/// Called before a screen resolution changes.
		virtual void ResolutionChanging(const uint32 width, const uint32 height) { OC_UNUSED(width); OC_UNUSED(height); }

		/// Called after a screen resolution changes.
		virtual void ResolutionChanged(const uint32 width, const uint32 height) = 0;

		/// Default destructor.
		virtual ~IGfxWindowListener(void) {}
	};
}

#endif // IGfxWindowListener_h__