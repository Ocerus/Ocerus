#ifndef IScreenResolutionChangeListener_h__
#define IScreenResolutionChangeListener_h__

namespace GfxSystem
{

	/// Callback listener from GfxRenderer.
	class IScreenResolutionChangeListener {
	public:
		virtual void EventResolutionChanged(int x, int y) = 0;
	};
}

#endif // IScreenResolutionChangeListener_h__