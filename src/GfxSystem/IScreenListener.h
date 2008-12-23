#ifndef IScreenResolutionChangeListener_h__
#define IScreenResolutionChangeListener_h__

namespace GfxSystem
{

	/// @name Callback listener from GfxRenderer.
	class IScreenListener {
	public:
		
		/// @name Called when a screen resolution changes.
		virtual void ResolutionChanged(int width, int height) = 0;

		/// @name Ensures virtual methods to work ok.
		virtual ~IScreenListener(void) {}
	};
}

#endif // IScreenResolutionChangeListener_h__